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
#include <string>
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
	: mModelButtonState(true), mMenuButtonState(true), mInitPosZ(-40.0)
	{
		mObjMenuNodeArray.clear();
		mObjMenuTransformArray.clear();

		DEFAULTATTIDUTE = osg::Quat(
				osg::DegreesToRadians(0.f), osg::Vec3d(1.0, 0.0, 0.0),
				osg::DegreesToRadians(0.f), osg::Vec3d(0.0, 1.0, 0.0),
				osg::DegreesToRadians(0.f), osg::Vec3d(0.0, 0.0, 1.0)
		);

		AssignmentKeyinput("setting.txt");
	}

	osg_Menu::~osg_Menu()
	{
		mObjMenuNodeArray.clear();
		mObjMenuTransformArray.clear();
		mMenuModelObjectArray.clear();
		mMenuModelTransArray.clear();
		mKeyAssignment.clear();
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
		CreateUnit("Controller.3ds", osg::Vec3d( 25, 0, 80));
		CreateUnit("model.3ds", osg::Vec3d(25, 0, 0));			//keyboard input = 201
		CreateUnit("reset.3ds", osg::Vec3d(25, 0, 30));			//keyboard input = 202
		CreateUnit("StartTransfer.3ds", osg::Vec3d(25, 0, 60));	//keyboard input = 203
		//CreateUnit("SphereButton.3ds");
		//CreateUnit("car1.3ds", osg::Vec3d(-25, 0, 0));
		//CreateUnit("car2.3ds", osg::Vec3d(-50, 0, 0));
		//CreateUnit("ScaleButton.3ds", osg::Vec3d(0, 0, 30));
		//CreateUnit("ScaleButton2.3ds", osg::Vec3d(-25, 0, 30));
		//CreateUnit("RollButton.3ds", osg::Vec3d(0, 0, 60));
		//CreateUnit("PitchButton.3ds", osg::Vec3d(-25, 0, 60));
		//CreateUnit("YawButton.3ds", osg::Vec3d(-50, 0, 60));
	}

	void osg_Menu::CreateModelButtonCloud( void )
	{
		const int modelSize	= static_cast<int>(mKeyAssignment.size());
		const int modelCol	=  3;
		const double xOffset = 150;
		const double yOffset = -100;

		int id=0;
		for(;id<modelSize; id++)
		{
			CreateModelButton( mKeyAssignment.at(id).second.c_str(), osg::Vec3( (id%modelCol)*xOffset, (id/modelCol)*yOffset, mInitPosZ));
		}
		CreateModelButton("cancel", osg::Vec3(0, -250, mInitPosZ));
	}

	unsigned int osg_Menu::GetKeyAssignment(const unsigned int & idx) const
	{
		if( mKeyAssignment.size() <= idx)
		{
			std::cerr << "Error: you designed the number out of range in key assignment" << std::endl;
			return 0;
		}
		return mKeyAssignment.at(idx).first;
	}

	void osg_Menu::AssignmentKeyinput(const char * settingFilename)
	{
		std::ostringstream setInput;
		setInput <<  ConstParams::DATABASEDIR << settingFilename;

		std::ifstream input(setInput.str().c_str());

		if(!input.is_open())
		{
			std::cerr << "Setting file cannot be opened!!" << std::endl;
			std::cerr << "Filename is " << setInput.str().c_str() << std::endl;
			exit(EXIT_SUCCESS);
		}

		while(input)
		{
			char line[1024] ;
			input.getline(line, 1024) ;
			std::stringstream line_input(line) ;

			std::pair<unsigned int, std::string> tmpKeyAssignment;

			//first word means a value assignment
			unsigned int value;
			line_input >> value;
			tmpKeyAssignment.first = value;

			//second word means a name of model
			std::string keyword;
			line_input >> keyword;
			tmpKeyAssignment.second = keyword;

			mKeyAssignment.push_back(tmpKeyAssignment);
		}
		mKeyAssignment.pop_back();
	}

	void osg_Menu::CreateUnit(const char * buttonfilename, osg::Vec3d pos)
	{
		const osg::Vec3d UNITBASEPOSITION(50, -200, 10);
		const osg::Quat UNITATTITUDE = osg::Quat(
			osg::DegreesToRadians(-90.f), osg::Vec3d(1.0, 0.0, 0.0),
			osg::DegreesToRadians(0.f), osg::Vec3d(0.0, 1.0, 0.0),
			osg::DegreesToRadians(0.f), osg::Vec3d(0.0, 0.0, 1.0)
		);

		//create button unit with osg::Node
		std::string str(ConstParams::MENUDATADIR);
		str += buttonfilename;
		osg::ref_ptr<osg::Node> node = osgDB::readNodeFile(str.c_str());
		LoadCheck(node.get(), str.c_str());

		node->setName(str.c_str());
		mObjMenuNodeArray.push_back(node);

		float scale = 10;

		mObjMenuTransformArray.push_back(new osg::PositionAttitudeTransform());

		mObjMenuTransformArray.back()->addChild(node.get());
		mObjMenuTransformArray.back()->setAttitude(UNITATTITUDE);
		mObjMenuTransformArray.back()->setPosition(pos + UNITBASEPOSITION);
		mObjMenuTransformArray.back()->setScale(osg::Vec3d(scale,scale,scale));
		mObjMenuTransformArray.back()->getOrCreateStateSet()->setRenderBinDetails(2, "RenderBin");

	}

	void osg_Menu::CreateModelButton(const char * buttonfilename, osg::Vec3d pos)
	{
		//create model unit with osg::Node
		std::ostringstream str;
		const char * format = ".3ds";
		str << ConstParams::DATABASEDIR << buttonfilename << "/" << buttonfilename << format;
		osg::ref_ptr<osg::Node> node = osgDB::readNodeFile(str.str().c_str());
		LoadCheck(node.get(), str.str().c_str());

		node->setName(str.str().c_str());
		mMenuModelObjectArray.push_back(node);

		float scale = 10;

		mMenuModelTransArray.push_back(new osg::PositionAttitudeTransform());

		mMenuModelTransArray.back()->addChild(node.get());
		mMenuModelTransArray.back()->setAttitude(DEFAULTATTIDUTE);
		mMenuModelTransArray.back()->setPosition(pos);
		mMenuModelTransArray.back()->setScale(osg::Vec3d(scale,scale,scale));
		mMenuModelTransArray.back()->getOrCreateStateSet()->setRenderBinDetails(2, "RenderBin");

	}
}
