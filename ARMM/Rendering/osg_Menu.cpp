/*
 * osg_Menu.cpp
 *
 * This file should be copied with Server osg_Menu.cpp
 * Otherwise, menu buttons must have been operated properly because collision is detected in server part
 *
 *  Created on: 2012/09/27
 *      Author: umakatsu
 */

#include "ARMM/Rendering/osg_Menu.h"
#include "constant.h"

#include <osg/ShapeDrawable>
#include <osgDB/ReadFile>
#include <osgwTools/Shapes.h>

#include <cstring>
#include <iostream>

namespace
{
	inline void LoadCheck(void * ptr, const char * filename)
	{
		if( ptr == NULL){
			std::cerr << "Error : No such file in the directory or incorrect path.  " << filename << std::endl;
		}
	}
}

namespace ARMM
{
	osg_Menu::osg_Menu()
	{
		mObjMenuNodeArray.clear();
		mObjMenuTransformArray.clear();

		DEFAULTATTIDUTE = osg::Quat(
				osg::DegreesToRadians(0.f), osg::Vec3d(1.0, 0.0, 0.0),
				osg::DegreesToRadians(0.f), osg::Vec3d(0.0, 1.0, 0.0),
				osg::DegreesToRadians(0.f), osg::Vec3d(0.0, 0.0, 1.0)
		);
	}

	osg_Menu::~osg_Menu()
	{

	}

	void osg_Menu::PushObjMenuNodeArray(osg::Node* n)
	{
		mObjMenuNodeArray.push_back(n);
	}

	void osg_Menu::PushObjMenuTransArray(osg::PositionAttitudeTransform* t)
	{
		mObjMenuTransformArray.push_back(t);
	}

	void osg_Menu::CreateMenuPane()
	{
		//register some components
		CreateUnit("Controller.3ds", osg::Vec3d( 0, -80, 0));
		CreateUnit("SphereButton.3ds");
		CreateUnit("car1.3ds", osg::Vec3d(-25, -0, 0));
		CreateUnit("car2.3ds", osg::Vec3d(-50, -0, 0));
		CreateUnit("ScaleButton.3ds", osg::Vec3d(0, -30, 0));
		CreateUnit("ScaleButton2.3ds", osg::Vec3d(-25, -30, 0));
		CreateUnit("RollButton.3ds", osg::Vec3d(0, -60, 0));
		CreateUnit("PitchButton.3ds", osg::Vec3d(-25, -60, 0));
		CreateUnit("YawButton.3ds", osg::Vec3d(-50, -60, 0));
	}

	void osg_Menu::CreateUnit(const char * buttonfilename, osg::Vec3d pos)
	{
		//create button unit with osg::Node
		std::string str(ConstParams::MENUDATADIR);
		str += buttonfilename;
		osg::ref_ptr<osg::Node> node = osgDB::readNodeFile(str.c_str());
		LoadCheck(node.get(), str.c_str());

		mObjMenuNodeArray.push_back(node.get());

		float scale = 10;
		mObjMenuTransformArray.push_back(new osg::PositionAttitudeTransform());
		const osg::Quat attitude = DEFAULTATTIDUTE;
		mObjMenuTransformArray.back()->addChild(node.get());
		mObjMenuTransformArray.back()->setAttitude(attitude);
		mObjMenuTransformArray.back()->setPosition(pos);
		mObjMenuTransformArray.back()->setScale(osg::Vec3d(scale,scale,scale));
		mObjMenuTransformArray.back()->getOrCreateStateSet()->setRenderBinDetails(2, "RenderBin");

	}

}
