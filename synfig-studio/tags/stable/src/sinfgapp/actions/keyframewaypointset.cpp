/* === S I N F G =========================================================== */
/*!	\file keyframeset.cpp
**	\brief Template File
**
**	$Id: keyframewaypointset.cpp,v 1.1.1.1 2005/01/07 03:34:37 darco Exp $
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

#include "keyframewaypointset.h"
#include <sinfgapp/canvasinterface.h>
#include <sinfg/valuenode_dynamiclist.h>
#include <sinfg/valuenode_animated.h>
#include "activepointsetsmart.h"
#include "waypointsetsmart.h"

#endif

using namespace std;
using namespace etl;
using namespace sinfg;
using namespace sinfgapp;
using namespace Action;

/* === M A C R O S ========================================================= */

ACTION_INIT(Action::KeyframeWaypointSet);
ACTION_SET_NAME(Action::KeyframeWaypointSet,"keyframe_waypoint_set");
ACTION_SET_LOCAL_NAME(Action::KeyframeWaypointSet,"Set Waypoints at Keyframe");
ACTION_SET_TASK(Action::KeyframeWaypointSet,"set");
ACTION_SET_CATEGORY(Action::KeyframeWaypointSet,Action::CATEGORY_KEYFRAME);
ACTION_SET_PRIORITY(Action::KeyframeWaypointSet,0);
ACTION_SET_VERSION(Action::KeyframeWaypointSet,"0.0");
ACTION_SET_CVS_ID(Action::KeyframeWaypointSet,"$Id: keyframewaypointset.cpp,v 1.1.1.1 2005/01/07 03:34:37 darco Exp $");

/* === G L O B A L S ======================================================= */

/* === P R O C E D U R E S ================================================= */

/* === M E T H O D S ======================================================= */

Action::KeyframeWaypointSet::KeyframeWaypointSet()
{
	keyframe.set_time(Time::begin()-1);
	set_dirty(false);
}

Action::ParamVocab
Action::KeyframeWaypointSet::get_param_vocab()
{
	ParamVocab ret(Action::CanvasSpecific::get_param_vocab());
	
	ret.push_back(ParamDesc("keyframe",Param::TYPE_KEYFRAME)
		.set_local_name(_("Keyframe"))
	);

	ret.push_back(ParamDesc("model",Param::TYPE_WAYPOINTMODEL)
		.set_local_name(_("Waypoint Model"))
	);

	return ret;
}

bool
Action::KeyframeWaypointSet::is_canidate(const ParamList &x)
{
	return canidate_check(get_param_vocab(),x);
}

bool
Action::KeyframeWaypointSet::set_param(const sinfg::String& name, const Action::Param &param)
{
	if(name=="keyframe" && param.get_type()==Param::TYPE_KEYFRAME)
	{
		keyframe=param.get_keyframe();
		
		return true;
	}
	if(name=="model" && param.get_type()==Param::TYPE_WAYPOINTMODEL)
	{
		waypoint_model=param.get_waypoint_model();
		
		return true;
	}

	return Action::CanvasSpecific::set_param(name,param);
}

bool
Action::KeyframeWaypointSet::is_ready()const
{
	if(keyframe.get_time()==(Time::begin()-1) || waypoint_model.is_trivial())
		return false;
	return Action::CanvasSpecific::is_ready();
}

void
Action::KeyframeWaypointSet::prepare()
{
	clear();

	try { get_canvas()->keyframe_list().find(keyframe);}
	catch(sinfg::Exception::NotFound)
	{
		throw Error(_("Unable to find the given keyframe"));
	}	

	{
		std::vector<sinfgapp::ValueDesc> value_desc_list;
		get_canvas_interface()->find_important_value_descs(value_desc_list);
		while(!value_desc_list.empty())
		{
			process_value_desc(value_desc_list.back());
			value_desc_list.pop_back();
		}
	}
}

void
Action::KeyframeWaypointSet::process_value_desc(const sinfgapp::ValueDesc& value_desc)
{	
	if(value_desc.is_value_node())
	{
		ValueNode_Animated::Handle value_node(ValueNode_Animated::Handle::cast_dynamic(value_desc.get_value_node()));
	
		if(value_node)
		{			
			Action::Handle action(WaypointSetSmart::create());
			
			action->set_param("canvas",get_canvas());
			action->set_param("canvas_interface",get_canvas_interface());
			action->set_param("value_node",ValueNode::Handle(value_node));
			
			Waypoint waypoint;
			try
			{
				waypoint=*value_node->find(keyframe.get_time());
			}
			catch(...)
			{
				waypoint.set_time(keyframe.get_time());
				waypoint.set_value((*value_node)(keyframe.get_time()));
			}
			waypoint.apply_model(waypoint_model);
			
			action->set_param("waypoint",waypoint);
	
			assert(action->is_ready());
			if(!action->is_ready())
				throw Error(Error::TYPE_NOTREADY);
		
			add_action_front(action);						
		}
	}
}
