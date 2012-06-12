/*
#ifndef KEYBOARD_CONTROLSCLIENT_H
#define KEYBOARD_CONTROLSCLIENT_H

#define SIM_MICROMACHINE 1

#include <iostream>

//OpenCV
#include "opencv/cv.h"

const float CUBE_SIZE = 4;
const float SPHERE_SIZE = 2;//FULL SIZE

using namespace std;

class KeyboardController_client
{
public:
	KeyboardController_client();
	int		check_input();
	void	set_input(const int & key);

private:
	inline bool getKey(int key);

};
#endif
*/


#ifndef KEYBOARD_CONTROLSCLIENT_H
#define KEYBOARD_CONTROLSCLIENT_H

#define SIM_MICROMACHINE 1

#include <iostream>

//OpenCV
#include "opencv/cv.h"

int	  collisionInd;
int	  collisionInd2;

extern int collide_counter;
extern double prev_collide_clock;
extern int collidedNodeInd;

const float CUBE_SIZE = 4;
const float SPHERE_SIZE = 2;//FULL SIZE

using namespace std;

class KeyboardController_client{
public:
	KeyboardController_client(){};
	int check_input() {

#ifdef SIM_MICROMACHINE
		if (getKey(78)) {//N
			return 78;
		} else if (getKey(83)) {//S
			return 83;
		} else if (getKey(65)) {//A
			return 65;
		} else if (getKey(68)) {//D
			return 68;
		}
#endif /*SIM_MICROMACHINE*/

		//A 65 S 83 D 68 W 87 F 70 V 86
		return 0;
	}

	void set_input(const int & key) {

#ifdef SIM_MICROMACHINE
		switch(key){
			case 78://N
//				osgAddObjectNode(osgSphereNode(SPHERE_SIZE));
//				Virtual_Objects_Count++;
				break;

			case 65:
				break;

			case 66: //B
				osgAddObjectNode(osgBoxNode(CUBE_SIZE));
				Virtual_Objects_Count++;
				break;

			case 79: {//o
//				const char * str = "TextureTransfer/Model3DS//HatuneMiku.3ds";
//				osg::ref_ptr<osg::Node> obj = osgDB::readNodeFile(str);
//				LoadCheck(obj.get(), str);
//
//				osg::ref_ptr<osg::PositionAttitudeTransform> obj_pat = new osg::PositionAttitudeTransform();
//				double scale = 1.61828; //サーバ側の値から決定した
//				obj_pat->setScale(osg::Vec3d(scale,scale,scale));
//				obj_pat->setAttitude(osg::Quat(
//					osg::DegreesToRadians(0.f), osg::Vec3d(1.0, 0.0, 0.0),
//					osg::DegreesToRadians(0.f), osg::Vec3d(0.0, 1.0, 0.0),
//					osg::DegreesToRadians(0.f), osg::Vec3d(0.0, 0.0, 1.0)
//					));
//				obj_pat->setPosition(osg::Vec3d(0.0, 0.0, 3.0));//shift body higher 3 units
//				obj_pat->addChild(obj);
//
//				osgAddObjectNode(dynamic_cast<osg::Node *>(obj_pat.release()));
////				osgAddObjectNode(osg3DSFileFromDiorama("ARMM/Data/rec/apple.3ds"));
//				Virtual_Objects_Count++;
				osgAddObjectNode(osgBoxNode(CUBE_SIZE));
				Virtual_Objects_Count++;

				break;
			}

			case 80: {//p
				const char * str = "ARMM/Data/rec/NewTorus.3ds";
				osg::ref_ptr<osg::Node> obj = osg3DSFileFromDiorama(str);
				LoadCheck(obj.get(), str);

				osg::ref_ptr<osg::PositionAttitudeTransform> obj_pat = new osg::PositionAttitudeTransform();
				double scale = 80.1812; //サーバ側の値から決定した for Torus
//				double scale = 1.5; //サーバ側の値から決定した
				obj_pat->setScale(osg::Vec3d(scale,scale,scale));
				obj_pat->setAttitude(osg::Quat(
					osg::DegreesToRadians(0.0f), osg::Vec3d(1.0, 0.0, 0.0),
					osg::DegreesToRadians(0.0f), osg::Vec3d(0.0, 1.0, 0.0),
					osg::DegreesToRadians(180.f), osg::Vec3d(0.0, 0.0, 1.0)
					));
				obj_pat->setPosition(osg::Vec3d(0.0, 0.0, 3.0));//shift body higher 3 units
				obj_pat->addChild(obj);

				//re-create new node
				osgAddObjectNode(dynamic_cast<osg::Node *>(obj_pat.release()));
				Virtual_Objects_Count++;
				break;
			}

			case 68:
				break;
			case 83:
				break;

			case 100: {//衝突判定のけーすとして用いる

				//remove the texture image object
				vector<osg::PositionAttitudeTransform*>::iterator it = obj_transform_array.begin() + collidedNodeInd;
				vector<osg::ref_ptr<osg::Node> >::iterator it2 = obj_node_array.begin() + collidedNodeInd;
				shadowedScene->removeChild(obj_transform_array.at(collidedNodeInd));
				obj_transform_array.erase(it);
				obj_node_array.erase(it2);

				//Operation
				int ind = objectIndex - collisionInd;
				int childInd = obj_transform_array[ind]->getChildIndex(obj_node_array[ind]); //子ノードのインデックスを取得
				obj_transform_array[ind]->setChild(childInd, osgSphereNode(5));				//子ノードを別のノードでセットしなおす
				break;
			}
			//衝突判定があった後、手に追従させるテクスチャを生成
			case 101: {

				//parts node
				const char * str = "ARMM/Data/rec/NewTorus.3ds";
				osg::ref_ptr<osg::Node> obj = osgTexture(str);
				LoadCheck(obj.get(), str);

				osg::ref_ptr<osg::PositionAttitudeTransform> obj_pat = new osg::PositionAttitudeTransform();
				double scale = 81.1812; //サーバ側の値から決定した
				obj_pat->setScale(osg::Vec3d(scale,scale,scale));
				obj_pat->setAttitude(osg::Quat(
					osg::DegreesToRadians(50.f), osg::Vec3d(1.0, 0.0, 0.0),
					osg::DegreesToRadians(-50.f), osg::Vec3d(0.0, 1.0, 0.0),
					osg::DegreesToRadians(0.f), osg::Vec3d(0.0, 0.0, 1.0)
					));
				obj_pat->setPosition(osg::Vec3d(0.0, 0.0, 3.0));//shift body higher 3 units
				obj_pat->addChild(obj);

				osgAddObjectNode(dynamic_cast<osg::Node *>(obj_pat.release()));
				cout << "<<Create a texture image with ocurring collision>>" << endl;

				break;
			}

		}
#endif /*SIM_MICROMACHINE*/

		//A 65 S 83 D 68 W 87 F 70 V 86
	}

private:
	inline bool getKey(int key) {
//		return GetAsyncKeyState(key)& 0x8000;
		return false;
	}
};

#endif
