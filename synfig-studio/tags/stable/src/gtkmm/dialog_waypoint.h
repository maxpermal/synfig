/* === S I N F G =========================================================== */
/*!	\file dialog_waypoint.h
**	\brief Template Header
**
**	$Id: dialog_waypoint.h,v 1.1.1.1 2005/01/07 03:34:36 darco Exp $
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

#ifndef __SINFG_GTKMM_DIALOG_WAYPOINT_H
#define __SINFG_GTKMM_DIALOG_WAYPOINT_H

/* === H E A D E R S ======================================================= */

#include <gtk/gtk.h>
#include <gtkmm/ruler.h>
#include <gtkmm/arrow.h>
#include <gtkmm/image.h>
#include <gdkmm/pixbufloader.h>
#include <gtkmm/viewport.h>
#include <gtkmm/adjustment.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/table.h>
#include <gtkmm/statusbar.h>
#include <gtkmm/button.h>
#include <gtkmm/progressbar.h>
#include <atkmm/stateset.h>
#include <gtkmm/paned.h>
#include <gtkmm/treeview.h>
#include <gtkmm/treestore.h>
#include <gtkmm/adjustment.h>
#include <gtkmm/box.h>
#include <gtkmm/scrollbar.h>
#include <gtkmm/cellrenderer.h>

#include <gtkmm/dialog.h>
#include <gtkmm/menu.h>


#include <sinfgapp/value_desc.h>
#include <sinfg/valuenode_animated.h>
#include <sinfg/valuenode_dynamiclist.h>
#include <sinfg/string.h>
#include <sinfg/time.h>

/* === M A C R O S ========================================================= */

/* === T Y P E D E F S ===================================================== */


/* === C L A S S E S & S T R U C T S ======================================= */

namespace studio {

class Widget_ValueBase;
class Widget_Waypoint;

class Dialog_Waypoint : public Gtk::Dialog
{
	Widget_Waypoint *waypointwidget;
	etl::handle<sinfg::Canvas> canvas;
	sinfg::ValueNode_Animated::WaypointList::iterator waypoint;
	sinfgapp::ValueDesc value_desc_;
		
	sigc::signal<void> signal_changed_;

	sigc::signal<void> signal_delete_;
	void on_ok_pressed();
	void on_apply_pressed();
	void on_delete_pressed();

public:
	Dialog_Waypoint(Gtk::Window& parent,etl::handle<sinfg::Canvas> canvas);
	~Dialog_Waypoint();

    void reset();

	void set_value_desc(sinfgapp::ValueDesc value_desc);
	sinfgapp::ValueDesc get_value_desc()const { return value_desc_; }

	void set_waypoint(sinfg::ValueNode_Animated::Waypoint x);
	const sinfg::ValueNode_Animated::Waypoint &get_waypoint()const;

	sigc::signal<void> &signal_changed()
	{return signal_changed_; }

	sigc::signal<void> &signal_delete()
	{return signal_delete_; }
}; // END of Dialog_Waypoint

}; // END of namespace studio

/* === E N D =============================================================== */

#endif
