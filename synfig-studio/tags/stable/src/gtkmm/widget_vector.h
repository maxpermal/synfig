/* === S I N F G =========================================================== */
/*!	\file widget_vector.h
**	\brief Template Header
**
**	$Id: widget_vector.h,v 1.1.1.1 2005/01/07 03:34:37 darco Exp $
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

/* === S T A R T =========================================================== */

#ifndef __SINFG_STUDIO_WIDGET_VECTOR_H
#define __SINFG_STUDIO_WIDGET_VECTOR_H

/* === H E A D E R S ======================================================= */

#include <gtkmm/box.h>
#include <gtkmm/adjustment.h>
#include <sinfg/vector.h>
#include <sinfg/distance.h>
#include <sinfg/canvas.h>

/* === M A C R O S ========================================================= */

/* === T Y P E D E F S ===================================================== */

/* === C L A S S E S & S T R U C T S ======================================= */

namespace Gtk { class SpinButton; };

namespace studio {

class Widget_Distance;
	
class Widget_Vector : public Gtk::HBox
{
	Gtk::SpinButton* spinbutton_x;
	Gtk::SpinButton* spinbutton_y;

	Widget_Distance* distance_x;
	Widget_Distance* distance_y;
	
	Gtk::Adjustment x_adjustment;
	Gtk::Adjustment y_adjustment;

	sinfg::Vector vector;

	sigc::signal<void> signal_value_changed_;

	sigc::signal<void> signal_activate_;
	
	sinfg::Canvas::LooseHandle canvas_;

public:
	
	void activate() { signal_activate_(); }
	
	void set_canvas(sinfg::Canvas::LooseHandle);
	sinfg::Canvas::LooseHandle get_canvas()const { return canvas_; }
	
	sigc::signal<void>& signal_value_changed() { return signal_value_changed_; }

	sigc::signal<void>& signal_activate() { return signal_activate_; }

	void on_value_changed();
	void on_grab_focus();

	void set_value(const sinfg::Vector &data);
	const sinfg::Vector &get_value();
	void set_has_frame(bool x);
	void set_digits(int x);
	Widget_Vector();
	~Widget_Vector();
	
protected:
	void show_all_vfunc();
}; // END of class Widget_Vector

}; // END of namespace studio

/* === E N D =============================================================== */

#endif
