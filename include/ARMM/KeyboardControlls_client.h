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

static const char * DATABASEDIR = "/home/umakatsu/Dropbox/Lab/ModelDatabase/";

using namespace std;

class KeyboardController_client
{
public:
	KeyboardController_client(){};
	int check_input()
	{
		if (getKey(78)) {//N
			return 78;
		} else if (getKey(83)) {//S
			return 83;
		} else if (getKey(65)) {//A
			return 65;
		} else if (getKey(68)) {//D
			return 68;
		}
		//A 65 S 83 D 68 W 87 F 70 V 86
		return 0;
	}

	void set_input(const int & key)
	{

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
//				ostringstream str;
//				const char * FILENAME = "ColorTorus";
//				const char * FORMAT = ".3ds";
//				str << DATABASEDIR << FILENAME << "/" << FILENAME << FORMAT;
//				osg::ref_ptr<osg::Node> obj = osgDB::readNodeFile(str.str().c_str());
//				LoadCheck(obj.get(), str.str().c_str());
////				float scale = 0.005;//GreenTable
//				float scale = 10;//GreenTorus
//
//				//安原モデルの場合、テクスチャ座標の調整が必要
				string str(DATABASEDIR);
				const char * FORMAT = ".3ds";
				const char * FILENAME = "keyboard";
				str += FILENAME; str+="/";
				string modelFileName(FILENAME);
				modelFileName += FORMAT;
				osg::ref_ptr<osg::Node> obj = (osg3DSFileFromDiorama(modelFileName.c_str(), str.c_str()));
				double scale = 300; //サーバ側の値から決定した for (keyboard) for 安原モデル

				osgAddObjectNode(obj.get(), scale);
				Virtual_Objects_Count++;
				break;
			}

			case 80: {//p
				ostringstream str;
				const char * FORMAT = ".3ds";
				const char * FILENAME = "WoodTable";
				str << DATABASEDIR << FILENAME << "/" << FILENAME << FORMAT;
				osg::ref_ptr<osg::Node> obj = osgDB::readNodeFile(str.str().c_str());
				LoadCheck(obj.get(), str.str().c_str());
				float scale = 10;

////				//安原モデルの場合、テクスチャ座標の調整が必要
//				string str(DATABASEDIR);
//				const char * FILENAME = "cube";
//				str += FILENAME; str+="/";
//				string modelFileName(FILENAME);
//				modelFileName += FORMAT;
//				osg::ref_ptr<osg::Node> obj = (osg3DSFileFromDiorama2(modelFileName.c_str(), str.c_str()));
//				double scale = 20; //for (Cube) for 安原モデル ただしosg3DSFileFromDioramaで調整されていること
////				double scale = 300; //for (keyboard) for 安原モデル
				osgAddObjectNode(obj.get(), scale);
				Virtual_Objects_Count++;
				break;
			}


			case 68:
				break;
			case 83:
				break;

			case 100: {// This function is called after texture transferred

				ostringstream str;
				const char * FILENAME = "Newcow";
				const char * FORMAT = ".3ds";
				str << DATABASEDIR << FILENAME << "/" << FILENAME << FORMAT;

				//remove the texture image object
				shadowedScene->removeChild(objTexture);
				obj_texture->removeChild(objTexture);

				//getting the index of the child node
				int childInd = obj_transform_array[collidedNodeInd]
				               ->getChildIndex(obj_node_array[collidedNodeInd]);

				//swap a child of the objects node with new child node
				obj_transform_array[collidedNodeInd]->setChild(childInd, osgDB::readNodeFile(str.str().c_str()));
				double scale = 10; //
//				double scale = 20; //安原Cube
//				double scale = 300;
				obj_transform_array[collidedNodeInd]->setScale(osg::Vec3d(scale,scale,scale));
				collision = false;
				break;
			}

			//creating a texture sticked in hand when collision occurred
			case 101:
			{
				//parts node
				string str(DATABASEDIR);
				const char * file = "key2.bmp";
				str += file;
				objTexture = osgCreateSoft(str.c_str());
				LoadCheck(objTexture.get(), str.c_str());


				osg::ref_ptr<osg::PositionAttitudeTransform> obj_pat = new osg::PositionAttitudeTransform();
//				double scale = 80.1012; //サーバ側の値から決定した
//				obj_pat->setScale(osg::Vec3d(scale,scale,scale));
//				obj_pat->setAttitude(osg::Quat(
//					osg::DegreesToRadians(50.f), osg::Vec3d(1.0, 0.0, 0.0),
//					osg::DegreesToRadians(-50.f), osg::Vec3d(0.0, 1.0, 0.0),
//					osg::DegreesToRadians(0.f), osg::Vec3d(0.0, 0.0, 1.0)
//					));
//				obj_pat->setPosition(osg::Vec3d(0.0, 0.0, 3.0));//shift body higher 3 units
				obj_pat->addChild(objTexture);
				obj_texture = obj_pat;
				shadowedScene->addChild(obj_texture);

				//				shadowedScene->addChild(objTexture);
				obj_texture->getOrCreateStateSet()->setRenderBinDetails(2, "RenderBin");

				cout << "<<Create a texture image with ocurring collision>>" << endl;

				break;
			}

		}
		//A 65 S 83 D 68 W 87 F 70 V 86
	}

private:
	inline bool getKey(int key) {
//		return GetAsyncKeyState(key)& 0x8000;
		return false;
	}
};

#endif
