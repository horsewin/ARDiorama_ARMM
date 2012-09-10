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
				string str(MODELDIR);
				const char * file = "Newcow.3ds";
				str += file;
				osg::ref_ptr<osg::Node> obj = osgDB::readNodeFile(str.c_str());
				LoadCheck(obj.get(), str.c_str());
				float scale = 10;
				osgAddObjectNode(obj.get(), scale);
				Virtual_Objects_Count++;
				break;
			}

			case 80: {//p
//				const char * str = "ARMM/Data/Model2/BlueCow.3ds";
//				osg::ref_ptr<osg::Node> obj = osgDB::readNodeFile(str);
//				LoadCheck(obj.get(), str);
//				double scale = 10; //サーバ側の値から決定した for cow
				osg::ref_ptr<osg::Node> obj = (osg3DSFileFromDiorama("ARMM/Data/ModelApple/apple.3ds", "ARMM/Data/ModelApple/"));
//				osg::ref_ptr<osg::Node> obj = (osg3DSFileFromDiorama("/home/umakatsu/TextureTransfer/TextureTransfer/Model3DS/Keyboard/keyboard.3ds", "/home/umakatsu/TextureTransfer/TextureTransfer/Model3DS/Keyboard/"));
				double scale = 300; //サーバ側の値から決定した for apple
				osgAddObjectNode(obj.get(), scale);
				Virtual_Objects_Count++;
				break;
			}

			case 68:
				break;
			case 83:
				break;

			case 100: {// This function is called after texture transferred

				string str(MODELDIR);
				const char * file = "Newapple.3ds";
				str += file;

				//remove the texture image object
				shadowedScene->removeChild(objTexture);
				obj_texture->removeChild(objTexture);

				//getting the index of the child node
				int childInd = obj_transform_array[collidedNodeInd]
				               ->getChildIndex(obj_node_array[collidedNodeInd]);

				//swap a child of the objects node with new child node
				obj_transform_array[collidedNodeInd]->setChild(childInd, osgDB::readNodeFile(str.c_str()));
				obj_transform_array[collidedNodeInd]->setScale(osg::Vec3d(10,10,10));
				collision = false;
				break;
			}

			//creating a texture sticked in hand when collision occurred
			case 101:
			{
				//parts node
				string str(MODELDIR);
				const char * file = "texture1.bmp";
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
