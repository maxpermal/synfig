/* === S I N F G =========================================================== */
/*!	\file main.cpp
**	\brief Template File
**
**	$Id: main.cpp,v 1.1.1.1 2005/01/07 03:34:37 darco Exp $
**
**	\legal
**	Copyright (c) 2002 Robert B. Quattlebaum Jr.
**
**	This software and associated documentation
**	are CONFIDENTIAL and PROPRIETARY property of
**	the above-mentioned copyright holder.
**
**	You may not copy, print, publish, or in any
**	other way distribute this software without
**	a prior written agreement with
**	the copyright holder.
**	\endlegal
*/
/* ========================================================================= */

/* === H E A D E R S ======================================================= */

#ifdef USING_PCH
#	include "pch.h"
#else
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif

#include "main.h"
#include "action.h"

#include <sinfg/color.h>
#include <sinfg/gradient.h>

#include <ETL/trivial>

#include <list>

#endif

/* === U S I N G =========================================================== */

using namespace std;
using namespace etl;
using namespace sinfg;
using namespace sinfgapp;

/* === M A C R O S ========================================================= */

/* === S T A T I C S ======================================================= */

static etl::reference_counter sinfgapp_ref_count_(0);
static sinfgapp::Action::Main* action_main;

static Color foreground_;
static Color background_;
static Gradient gradient_;
static bool gradient_default_colors_;

static sinfg::Distance bline_width_;

static Color::BlendMethod blend_method_;
static Real opacity_;

static sinfgapp::InputDevice::Handle selected_input_device_;
static list<sinfgapp::InputDevice::Handle> input_devices_;

trivial<sigc::signal<void> > signal_foreground_color_changed_;
trivial<sigc::signal<void> > signal_background_color_changed_;
trivial<sigc::signal<void> > signal_gradient_changed_;
trivial<sigc::signal<void> > signal_bline_width_changed_;
trivial<sigc::signal<void> > signal_blend_method_changed_;
trivial<sigc::signal<void> > signal_opacity_changed_;
trivial<sigc::signal<void> > signal_interpolation_changed_;

trivial<Settings> settings_;

static sinfg::Waypoint::Interpolation interpolation_;

/* === P R O C E D U R E S ================================================= */

/* === M E T H O D S ======================================================= */

sinfgapp::Main::Main(const sinfg::String &basepath,ProgressCallback *cb):
	sinfg::Main(basepath,cb),
	ref_count_(sinfgapp_ref_count_)
{
	if(ref_count_.count())
		return;

	sinfgapp_ref_count_.reset();
	ref_count_=sinfgapp_ref_count_;
	
	// Add initialization after this point
	action_main=new sinfgapp::Action::Main();

	settings_.construct();
	
	signal_foreground_color_changed_.construct();
	signal_background_color_changed_.construct();
	signal_gradient_changed_.construct();
	signal_opacity_changed_.construct();
	signal_blend_method_changed_.construct();
	signal_interpolation_changed_.construct();
	
	set_foreground_color(Color::black());
	set_background_color(Color::white());
	set_gradient_default_colors();
	set_bline_width(Distance(1,Distance::SYSTEM_POINTS));
	set_opacity(1.0);
	set_blend_method(Color::BLEND_COMPOSITE);
}

sinfgapp::Main::~Main()
{
	ref_count_.detach();
	if(!sinfgapp_ref_count_.unique())
		return;
	sinfgapp_ref_count_.detach();
	
	// Add deinitialization after this point

	delete action_main;	
	
	selected_input_device_=0;
	input_devices_.clear();
	
	settings_.destruct();
	signal_foreground_color_changed_.destruct();
	signal_background_color_changed_.destruct();
	signal_gradient_changed_.destruct();

	signal_opacity_changed_.destruct();
	signal_blend_method_changed_.destruct();
	signal_interpolation_changed_.destruct();
}

Settings&
sinfgapp::Main::settings()
{
	return settings_;
}

sigc::signal<void>&
sinfgapp::Main::signal_foreground_color_changed()
{
	return signal_foreground_color_changed_;
}

sigc::signal<void>&
sinfgapp::Main::signal_background_color_changed()
{
	return signal_background_color_changed_;
}

sigc::signal<void>&
sinfgapp::Main::signal_gradient_changed()
{
	return signal_gradient_changed_;
}

sigc::signal<void>&
sinfgapp::Main::signal_bline_width_changed()
{
	return signal_bline_width_changed_;
}

sigc::signal<void>&
sinfgapp::Main::signal_blend_method_changed()
{
	return signal_blend_method_changed_;
}

sigc::signal<void>&
sinfgapp::Main::signal_opacity_changed()
{
	return signal_opacity_changed_;
}

sigc::signal<void>&
sinfgapp::Main::signal_interpolation_changed()
{
	return signal_interpolation_changed_;
}

const sinfg::Color&
sinfgapp::Main::get_foreground_color()
{
	return foreground_;
}

const sinfg::Color&
sinfgapp::Main::get_background_color()
{
	return background_;
}

const sinfg::Gradient&
sinfgapp::Main::get_gradient()
{
	return gradient_;
}

const sinfg::Real&
sinfgapp::Main::get_opacity()
{
	return opacity_;
}

sinfg::Color::BlendMethod
sinfgapp::Main::get_blend_method()
{
	return blend_method_;
}

void
sinfgapp::Main::set_foreground_color(sinfg::Color color)
{
	foreground_=color;
	signal_foreground_color_changed()();
	if(selected_input_device_)
		selected_input_device_->set_foreground_color(foreground_);
	if(gradient_default_colors_)
	{
		gradient_=Gradient(foreground_,background_);
		signal_gradient_changed()();
	}
}

void
sinfgapp::Main::set_background_color(sinfg::Color color)
{
	background_=color;
	signal_background_color_changed()();

	if(selected_input_device_)
		selected_input_device_->set_background_color(background_);

	if(gradient_default_colors_)
	{
		gradient_=Gradient(foreground_,background_);
		signal_gradient_changed()();
	}
}

void
sinfgapp::Main::set_gradient(sinfg::Gradient gradient)
{
	gradient_=gradient;
	gradient_default_colors_=false;
	signal_gradient_changed()();
}

void
sinfgapp::Main::set_gradient_default_colors()
{
	gradient_default_colors_=true;
	gradient_=Gradient(foreground_,background_);
	signal_gradient_changed()();
}

void
sinfgapp::Main::color_swap()
{
	const Color tmp(foreground_);
	foreground_=background_;
	background_=tmp;

	if(selected_input_device_)
	{
		selected_input_device_->set_foreground_color(foreground_);
		selected_input_device_->set_background_color(background_);
	}

	signal_foreground_color_changed()();
	signal_background_color_changed()();

	if(gradient_default_colors_)
	{
		gradient_=Gradient(foreground_,background_);
		signal_gradient_changed()();
	}
}

sinfg::Waypoint::Interpolation
sinfgapp::Main::get_interpolation()
{
	return interpolation_;
}


void
sinfgapp::Main::set_interpolation(sinfg::Waypoint::Interpolation x)
{
	if(interpolation_!=x)
	{
		interpolation_=x;
		
		signal_interpolation_changed();
	}
}

const sinfg::Distance&
sinfgapp::Main::get_bline_width()
{
	return bline_width_;
}

void
sinfgapp::Main::set_bline_width(sinfg::Distance x)
{
	if(x<0)x=0;
	if(x!=bline_width_)
	{
		bline_width_=x;
		
		if(selected_input_device_)
			selected_input_device_->set_bline_width(x);
		
		signal_bline_width_changed()();
	}
}

void
sinfgapp::Main::set_opacity(sinfg::Real x)
{
	opacity_=x;
	if(selected_input_device_)
		selected_input_device_->set_opacity(opacity_);
	signal_opacity_changed()();
}

void
sinfgapp::Main::set_blend_method(sinfg::Color::BlendMethod x)
{
	blend_method_=x;
	if(selected_input_device_)
		selected_input_device_->set_blend_method(x);
	signal_blend_method_changed()();
}


InputDevice::Handle
sinfgapp::Main::add_input_device(const sinfg::String id, InputDevice::Type type)
{
	input_devices_.push_back(new InputDevice(id,type));
	return input_devices_.back();
}

InputDevice::Handle
sinfgapp::Main::find_input_device(const sinfg::String id)
{
	list<InputDevice::Handle>::iterator iter;
	for(iter=input_devices_.begin();iter!=input_devices_.end();++iter)
		if((*iter)->get_id()==id)
			return *iter;
	return 0;
}

InputDevice::Handle
sinfgapp::Main::select_input_device(const sinfg::String id)
{
	InputDevice::Handle input_device(find_input_device(id));
	if(!input_device)
		return 0;
	if(!select_input_device(input_device))
		return 0;
	return input_device;
}

bool
sinfgapp::Main::select_input_device(InputDevice::Handle input_device)
{
	assert(input_device);
	
	sinfg::info("Input device changed to \"%s\"",input_device->get_id().c_str());
	
	selected_input_device_=input_device;

	set_bline_width(input_device->get_bline_width());	
	set_foreground_color(input_device->get_foreground_color());
	set_opacity(input_device->get_opacity());	
	set_blend_method(input_device->get_blend_method());	
	
	return true;
}

InputDevice::Handle
sinfgapp::Main::get_selected_input_device()
{
	return selected_input_device_;	
}

void
sinfgapp::Main::set_state(sinfg::String state)
{
	if(selected_input_device_)
		selected_input_device_->set_state(state);
}
