/* === S I N F G =========================================================== */
/*!	\file keyframeactionmanager.cpp
**	\brief Template File
**
**	$Id: keyframeactionmanager.cpp,v 1.1.1.1 2005/01/07 03:34:36 darco Exp $
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

#include "keyframeactionmanager.h"
#include "keyframetree.h"
#include <sinfgapp/action_param.h>
#include "instance.h"

#endif

/* === U S I N G =========================================================== */

using namespace std;
using namespace etl;
using namespace sinfg;
using namespace studio;

static const guint no_prev_popup((guint)-1);

/* === M A C R O S ========================================================= */

//#define ONE_ACTION_GROUP 1

/* === G L O B A L S ======================================================= */

/* === P R O C E D U R E S ================================================= */

/* === M E T H O D S ======================================================= */

KeyframeActionManager::KeyframeActionManager():
	action_group_(Gtk::ActionGroup::create()),
	popup_id_(no_prev_popup),
	queued(false)
{
}

KeyframeActionManager::~KeyframeActionManager()
{
}

void
KeyframeActionManager::set_ui_manager(const Glib::RefPtr<Gtk::UIManager> &x)
{
	clear();

#ifdef ONE_ACTION_GROUP
	if(ui_manager_)	get_ui_manager()->remove_action_group(action_group_);
	ui_manager_=x;
	if(ui_manager_)	get_ui_manager()->insert_action_group(action_group_);
#else
	ui_manager_=x;
#endif
}

void
KeyframeActionManager::set_keyframe_tree(KeyframeTree* x)
{
	selection_changed_connection.disconnect();
	keyframe_tree_=x;
	if(keyframe_tree_)
	{
		selection_changed_connection=keyframe_tree_->get_selection()->signal_changed().connect(
			sigc::mem_fun(*this,&KeyframeActionManager::queue_refresh)
		);
	}
}

void
KeyframeActionManager::set_canvas_interface(const etl::handle<sinfgapp::CanvasInterface> &x)
{
	time_changed_connection.disconnect();
	canvas_interface_=x;
	if(canvas_interface_)
	{
		canvas_interface_->signal_time_changed().connect(
			sigc::mem_fun(*this,&KeyframeActionManager::queue_refresh)
		);
	}
}

void
KeyframeActionManager::clear()
{
	if(ui_manager_)
	{
		// Clear out old stuff
		if(popup_id_!=no_prev_popup)
		{
			get_ui_manager()->remove_ui(popup_id_);
			popup_id_=no_prev_popup;
#ifdef ONE_ACTION_GROUP
			while(!action_group_->get_actions().empty())action_group_->remove(*action_group_->get_actions().begin());
#else
			get_ui_manager()->remove_action_group(action_group_);
			action_group_=Gtk::ActionGroup::create();
#endif
		}
	}
}

void
KeyframeActionManager::queue_refresh()
{
	if(queued)
		return;
	
	//queue_refresh_connection.disconnect();
	queue_refresh_connection=Glib::signal_idle().connect(
		sigc::bind_return(
			sigc::mem_fun(*this,&KeyframeActionManager::refresh),
			false
		)
	);
	
	queued=true;
}

void
KeyframeActionManager::on_keyframe_properties()
{
	signal_show_keyframe_properties_();
}

void
KeyframeActionManager::on_add_keyframe()
{
	sinfgapp::Action::Handle action(sinfgapp::Action::create("keyframe_add"));

	if(!action)
		return;
	
	action->set_param("canvas",canvas_interface_->get_canvas());
	action->set_param("canvas_interface",canvas_interface_);
	action->set_param("keyframe",Keyframe(canvas_interface_->get_time()));

	canvas_interface_->get_instance()->perform_action(action);
}

void
KeyframeActionManager::refresh()
{
	KeyframeTreeStore::Model model;
	
	if(queued)
	{
		queued=false;
		//queue_refresh_connection.disconnect();
	}


	clear();
	
	// Make sure we are ready
	if(!ui_manager_ || !keyframe_tree_ || !canvas_interface_)
	{
		sinfg::error("KeyframeActionManager::refresh(): Not ready!");
		return;
	}
		
	String ui_info;
	
	{
		sinfgapp::Action::ParamList param_list;
		param_list.add("time",get_canvas_interface()->get_time());
		param_list.add("canvas",get_canvas_interface()->get_canvas());
		param_list.add("canvas_interface",get_canvas_interface());
		if(keyframe_tree_->get_selection()->count_selected_rows()==1)
		{
			Keyframe keyframe((*keyframe_tree_->get_selection()->get_selected())[model.keyframe]);
			param_list.add("keyframe",keyframe);
		}

		handle<studio::Instance>::cast_static(
			get_canvas_interface()->get_instance()
		)->add_actions_to_group(
			action_group_,
			ui_info,
			param_list,
			sinfgapp::Action::CATEGORY_KEYFRAME
		);
	}
	if(action_group_->get_action("action-keyframe_add"))
	{
		action_group_->remove(action_group_->get_action("action-keyframe_add"));
	}
	
		action_group_->add(Gtk::Action::create(
			"action-keyframe_add",
			Gtk::StockID("gtk-add"),
			_("Add new Keyframe"),_("Add new Keyframe")
		),
			sigc::mem_fun(*this,&KeyframeActionManager::on_add_keyframe)
		);
	
	try
	{
		canvas_interface_->get_canvas()->keyframe_list().find(canvas_interface_->get_time());
		action_group_->get_action("action-keyframe_add")->set_sensitive(false);
		if(action_group_->get_action("action-keyframe_duplicate"))
			action_group_->get_action("action-keyframe_duplicate")->set_sensitive(false);
	}
	catch(...)
	{
	}
	
	{
		Glib::RefPtr<Gtk::Action> action(Gtk::Action::create("keyframe-properties", Gtk::StockID("gtk-properties"), _("Keyframe Properties")));
		action_group_->add(action,sigc::mem_fun(*this,&KeyframeActionManager::on_keyframe_properties));
		if(keyframe_tree_->get_selection()->count_selected_rows()==0)
			action->set_sensitive(false);
	}

	ui_info="<ui><menubar action='menu-main'><menu action='menu-keyframe'>"+ui_info+"</menu></menubar></ui>";
	popup_id_=get_ui_manager()->add_ui_from_string(ui_info);	
#ifdef ONE_ACTION_GROUP
#else
	get_ui_manager()->insert_action_group(action_group_);
#endif
}
