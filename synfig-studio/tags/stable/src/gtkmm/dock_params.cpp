/* === S I N F G =========================================================== */
/*!	\file dock_params.cpp
**	\brief Template File
**
**	$Id: dock_params.cpp,v 1.1.1.1 2005/01/07 03:34:36 darco Exp $
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

#include "dock_params.h"
#include "app.h"

#include <gtkmm/scrolledwindow.h>
#include <cassert>
#include "instance.h"
#include <sigc++/signal.h>
#include <sigc++/hide.h>
#include <sigc++/slot.h>
#include "canvasview.h"
#include "layerparamtreestore.h"
#include "workarea.h"

#endif

/* === U S I N G =========================================================== */

using namespace std;
using namespace etl;
using namespace sinfg;
using namespace studio;

/* === M A C R O S ========================================================= */

/* === G L O B A L S ======================================================= */

/* === P R O C E D U R E S ================================================= */

/* === M E T H O D S ======================================================= */

Dock_Params::Dock_Params():
	Dock_CanvasSpecific("params",_("Params"),Gtk::Stock::INDEX/*Gtk::StockID("sinfg-params")*/),
	action_group(Gtk::ActionGroup::create())
{
/*
	App::ui_manager()->insert_action_group(action_group_layer_ops);

    Glib::ustring ui_info =
	"<ui>"
	"	<toolbar action='toolbar-palette'>"
	"	<toolitem action='amount-increase' />"
	"	<toolitem action='amount-decrease' />"
	"	</toolbar>"
	"</ui>"
	;

	App::ui_manager()->add_ui_from_string(ui_info);
	*/
}

Dock_Params::~Dock_Params()
{
}


void
Dock_Params::init_canvas_view_vfunc(etl::loose_handle<CanvasView> canvas_view)
{
	DEBUGPOINT();
	Gtk::TreeView* tree_view(
		static_cast<Gtk::TreeView*>(canvas_view->get_ext_widget(get_name()))
	);
	
	if(tree_view)
	{
		tree_view->get_selection()->signal_changed().connect(
			sigc::mem_fun(
				*this,
				&Dock_Params::refresh_selected_param
			)
		);
	}
}

void
Dock_Params::refresh_selected_param()
{
	Gtk::TreeView* tree_view(
		static_cast<Gtk::TreeView*>(get_canvas_view()->get_ext_widget(get_name()))
	);
	Gtk::TreeModel::iterator iter(tree_view->get_selection()->get_selected());
	
	if(iter)
	{
		LayerParamTreeStore::Model model;
		get_canvas_view()->work_area->set_selected_value_node(
			(sinfg::ValueNode::Handle)(*iter)[model.value_node]
		);
	}
	else
	{
		get_canvas_view()->work_area->set_selected_value_node(0);
	}
}

void
Dock_Params::changed_canvas_view_vfunc(etl::loose_handle<CanvasView> canvas_view)
{
	if(canvas_view)
	{
		Gtk::Widget* tree_view(canvas_view->get_ext_widget(get_name()));
		
		add(*tree_view);
		tree_view->show();
		show_all();
	}
	else clear_previous();
}
