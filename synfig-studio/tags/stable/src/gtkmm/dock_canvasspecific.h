/* === S I N F G =========================================================== */
/*!	\file dock_canvasspecific.h
**	\brief Template Header
**
**	$Id: dock_canvasspecific.h,v 1.1.1.1 2005/01/07 03:34:36 darco Exp $
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

#ifndef __SINFG_STUDIO_DOCK_CANVASSPECIFIC_H
#define __SINFG_STUDIO_DOCK_CANVASSPECIFIC_H

/* === H E A D E R S ======================================================= */

#include "dockable.h"
#include <gtkmm/treeview.h>
#include "instance.h"

/* === M A C R O S ========================================================= */

/* === T Y P E D E F S ===================================================== */

/* === C L A S S E S & S T R U C T S ======================================= */

namespace studio {

class CanvasView;
class Instance;
	
class Dock_CanvasSpecific : public Dockable
{		
	SigC::Connection canvas_delete_connection;
protected:
	virtual void init_instance_vfunc(etl::loose_handle<Instance> instance);

	virtual void init_canvas_view_vfunc(etl::loose_handle<CanvasView> canvas_view);
	virtual void changed_canvas_view_vfunc(etl::loose_handle<CanvasView> canvas_view);

private:
	void init_canvas_view(CanvasView* canvas_view);
	void init_instance(etl::handle<Instance> instance);
	void delete_instance(etl::handle<Instance> instance);
	void canvas_view_changed();
	void changed_canvas_view(etl::loose_handle<CanvasView> canvas_view) { return changed_canvas_view_vfunc(canvas_view); }
public:

	etl::loose_handle<studio::CanvasView> get_canvas_view();
	etl::loose_handle<sinfgapp::CanvasInterface> get_canvas_interface();

	Dock_CanvasSpecific(const sinfg::String& name,const sinfg::String& local_name,Gtk::StockID stock_id_=Gtk::StockID(" "));
	virtual ~Dock_CanvasSpecific();
}; // END of Dock_CanvasSpecific

}; // END of namespace studio

/* === E N D =============================================================== */

#endif
