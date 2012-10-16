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

#include <vector>

namespace ARMM
{
	class osg_Menu
	{
	public:
		osg_Menu();
		~osg_Menu();

		std::vector<osg::Node*> getObjMenuNodeArray() const {
			return mObjMenuNodeArray;
		}

		std::vector<osg::PositionAttitudeTransform*> getObjMenuTransformArray() const {
			return mObjMenuTransformArray;
		}

		void PushObjMenuNodeArray(osg::Node* n);
		void PushObjMenuTransArray(osg::PositionAttitudeTransform* t);

		void setObjMenuNodeArray(std::vector<osg::Node*> objMenuNodeArray) {
			mObjMenuNodeArray = objMenuNodeArray;
		}

		void setObjMenuTransformArray(
				std::vector<osg::PositionAttitudeTransform*> objMenuTransformArray) {
			mObjMenuTransformArray = objMenuTransformArray;
		}

		void CreateMenuPane( void );

	private:
		void CreateUnit(const char * buttonfilename, osg::Vec3d pos = osg::Vec3d(0,0,0));

	private:
		osg::Quat DEFAULTATTIDUTE;

		std::vector<osg::Node*> mObjMenuNodeArray;
		std::vector<osg::PositionAttitudeTransform*> mObjMenuTransformArray;


	};
}


#endif /* OSG_MENU_H_ */
