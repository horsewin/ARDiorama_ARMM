/*
 * osg_Menu.h
 *
 *  Created on: 2012/09/27
 *      Author: umakatsu
 */

#ifndef OSG_MENU_H_
#define OSG_MENU_H_


#include <osg/Geode>
#include <osg/Geometry>
#include <osg/PolygonMode>
#include <osg/MatrixTransform>
#include <osg/PositionAttitudeTransform>
#include <osg/ref_ptr>

#include <vector>

namespace ARMM
{
	class osg_Menu
	{
	public:
		osg_Menu();
		~osg_Menu();

		void PushObjMenuNodeArray(osg::Node* n);
		void PushObjMenuTransArray(osg::PositionAttitudeTransform* t);

		void CreateMenuPane( void );
		void CreateModelButtonCloud( void );

	bool isMenuButtonState() const {
		return mMenuButtonState;
	}

	void ToggleMenuButtonState() {
		mMenuButtonState = !mMenuButtonState;
	}

	bool isModelButtonState() const {
		return mModelButtonState;
	}

	void ToggleModelButtonState()
	{
		mModelButtonState = !mModelButtonState;
	}

		std::vector<osg::ref_ptr<osg::Node> > getMenuModelObjectArray() const {
		return mMenuModelObjectArray;
	}

	void setMenuModelObjectArray(
			std::vector<osg::ref_ptr<osg::Node> > menuModelObjectArray) {
		mMenuModelObjectArray = menuModelObjectArray;
	}

	std::vector<osg::ref_ptr<osg::PositionAttitudeTransform> > getMenuModelTransArray() const {
		return mMenuModelTransArray;
	}

	void setMenuModelTransArray(
			std::vector<osg::ref_ptr<osg::PositionAttitudeTransform> > menuModelTransArray) {
		mMenuModelTransArray = menuModelTransArray;
	}

	std::vector<osg::ref_ptr<osg::Node> > getObjMenuNodeArray() const {
			return mObjMenuNodeArray;
		}

		void setObjMenuNodeArray(
				std::vector<osg::ref_ptr<osg::Node> > objMenuNodeArray) {
			mObjMenuNodeArray = objMenuNodeArray;
		}

		std::vector<osg::ref_ptr<osg::PositionAttitudeTransform> > getObjMenuTransformArray() const {
			return mObjMenuTransformArray;
		}

		void setObjMenuTransformArray(
				std::vector<osg::ref_ptr<osg::PositionAttitudeTransform> > objMenuTransformArray) {
			mObjMenuTransformArray = objMenuTransformArray;
		}

	private:
		void CreateUnit(const char * buttonfilename, osg::Vec3d pos = osg::Vec3d(0,0,0));
		void CreateModelButton(const char * buttonfilename, osg::Vec3d pos = osg::Vec3d(0,0,0));
		void AssignmentKeyinput(const char * settingFilename);

	private:
		osg::Quat DEFAULTATTIDUTE;

		std::vector<osg::ref_ptr<osg::Node> > mObjMenuNodeArray;
		std::vector<osg::ref_ptr<osg::PositionAttitudeTransform> > mObjMenuTransformArray;

		//for menu button of virtual objects
		std::vector<osg::ref_ptr<osg::Node> > mMenuModelObjectArray;
		std::vector<osg::ref_ptr<osg::PositionAttitudeTransform> >  mMenuModelTransArray;

		std::vector<std::pair<unsigned int, std::string> > mKeyAssignment;

		bool mModelButtonState;
		bool mMenuButtonState;

	};
}


#endif /* OSG_MENU_H_ */
