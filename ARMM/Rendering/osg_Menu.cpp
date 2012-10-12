/*
 * osg_Menu.cpp
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

	void osg_Menu::CreatePlane()
	{
		const short resX(12), resY(9);
		osg::ref_ptr< osg::Geode > geode( new osg::Geode );
		const osg::Vec3 llCorner( 0.0, .0, 0.0 );
		const osg::Vec3 uVec( 100.0, 0.0, 0.0 );
		const osg::Vec3 vVec( 0.0, 100.0, 0.0 ); // Must be at a slight angle for wind to catch it.
		osg::Geometry* geom = osgwTools::makePlane( llCorner, uVec, vVec, osg::Vec2s( resX-1, resY-1 ) );
		geode->addDrawable( geom );

		mObjMenuNodeArray.push_back(geode.release());

		float scale = 1;
		mObjMenuTransformArray.push_back(new osg::PositionAttitudeTransform());
		const osg::Quat attitude = DEFAULTATTIDUTE;
		mObjMenuTransformArray.back()->addChild(geode.get());
		mObjMenuTransformArray.back()->setAttitude(attitude);
		mObjMenuTransformArray.back()->setPosition(osg::Vec3(250.0, -50.0, 30.0));
		mObjMenuTransformArray.back()->setScale(osg::Vec3d(scale,scale,scale));
		mObjMenuTransformArray.back()->getOrCreateStateSet()->setRenderBinDetails(2, "RenderBin");
	}

	void osg_Menu::CreateButtonUnit(const float & boxsize, osg::Vec4f color)
	{
		osg::ref_ptr<osg::Node> node;
		//create button unit with osg::Node
		float scale = 10;
		osg::ref_ptr<osg::Box> box= new osg::Box(osg::Vec3d(0,0,0), boxsize*scale);
		osg::ref_ptr<osg::ShapeDrawable> shape = new osg::ShapeDrawable( box.get() );
		shape->setColor(color);
		osg::ref_ptr<osg::Geode> geode = new osg::Geode;
		geode->addDrawable( shape.get() );
		node = geode.get();


		mObjMenuNodeArray.push_back(node.get());

		mObjMenuTransformArray.push_back(new osg::PositionAttitudeTransform());
		const osg::Quat attitude = DEFAULTATTIDUTE;
		mObjMenuTransformArray.back()->addChild(node.get());
		mObjMenuTransformArray.back()->setAttitude(attitude);
		mObjMenuTransformArray.back()->setPosition(osg::Vec3(250.0, -50.0, 0.0));
		mObjMenuTransformArray.back()->setScale(osg::Vec3d(scale,scale,scale));
		mObjMenuTransformArray.back()->getOrCreateStateSet()->setRenderBinDetails(2, "RenderBin");

	}

	void osg_Menu::CreateButtonFile(const char * buttonfilename, osg::Vec3d pos)
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
