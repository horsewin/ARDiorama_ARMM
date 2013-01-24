/*
 * osg_Object.cpp
 *
 *  Created on: 2012/09/16
 *      Author: umakatsu
 */

#include "ARMM/Rendering/osg_Object.h"
#include "constant.h"

#include <cassert>

namespace ARMM
{
	osg_Object::osg_Object()
	:Virtual_Objects_Count(0), softTexture(false)
	{
		objectIndex = 0;
	}

	osg_Object::~osg_Object()
	{

	}

	void osg_Object::SetObjTransformArrayIndex( const int & idx, osg::Quat quat, osg::Vec3d vec)
	{
		obj_transform_array.at(idx)->setAttitude(quat);
		obj_transform_array.at(idx)->setPosition(vec);
	}

	osg::ref_ptr<osg::Node>  osg_Object::GetObjNodeIndex(const int & idx) const
	{
		assert( idx >= 0 && idx < this->obj_node_array.size());
		return obj_node_array[idx];
	}

	int osg_Object::SizeObjNodeArray(void) const
	{
		return obj_node_array.size();
	}

	int osg_Object::SizeObjTransformArray(void) const
	{
		return obj_transform_array.size();
	}

}
