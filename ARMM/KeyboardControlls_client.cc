/*

 * KeyboardControlls_client.cc
 *
 *  Created on: 2012/06/06
 *      Author: umakatsu


//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "ARMM/KeyboardControlls_client.h"

#include "constant.h"

//Graphics calls
#include "ARMM/osg_Client.h"

namespace ARMM
{

//---------------------------------------------------------------------------
// Global
//---------------------------------------------------------------------------
	extern int collide_counter;
	extern double prev_collide_clock;

//---------------------------------------------------------------------------
// Constant/Define
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------

	//A 65 S 83 D 68 W 87 F 70 V 86
	int KeyboardController_client::check_input()
	{
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
#endif SIM_MICROMACHINE

		return 0;
	}

	//A 65 S 83 D 68 W 87 F 70 V 86
	void KeyboardController_client::set_input(const int & key)
	{
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
				const char * str = "TextureTransfer/Model3DS//HatuneMiku.3ds";
				osg::ref_ptr<osg::Node> obj = osgDB::readNodeFile(str);
				LoadCheck(obj.get(), str);

				osg::ref_ptr<osg::PositionAttitudeTransform> obj_pat = new osg::PositionAttitudeTransform();
				double scale = 1.61828; //サーバ側の値から決定した
				obj_pat->setScale(osg::Vec3d(scale,scale,scale));
				obj_pat->setAttitude(osg::Quat(
					osg::DegreesToRadians(0.f), osg::Vec3d(1.0, 0.0, 0.0),
					osg::DegreesToRadians(0.f), osg::Vec3d(0.0, 1.0, 0.0),
					osg::DegreesToRadians(0.f), osg::Vec3d(0.0, 0.0, 1.0)
					));
				obj_pat->setPosition(osg::Vec3d(0.0, 0.0, 3.0));//shift body higher 3 units
				obj_pat->addChild(obj);

				osgAddObjectNode(dynamic_cast<osg::Node *>(obj_pat.release()));
//				osgAddObjectNode(osg3DSFileFromDiorama("ARMM/Data/rec/apple.3ds"));
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

				//erase old node
				int nObjTrans = obj_transform_array.size();
				if( nObjTrans > 0){
					vector<osg::PositionAttitudeTransform*>::iterator it = obj_transform_array.begin();
					vector<osg::ref_ptr<osg::Node> >::iterator it2 = obj_node_array.begin();
					shadowedScene->removeChild(obj_transform_array.at(0));
					obj_transform_array.erase(it);
					obj_node_array.erase(it2);
//		return GetAsyncKeyState(key)& 0x8000;
//					return false;
//				}

					//re-create new node
					const char * str = "ARMM/Data/rec/NewTorus.3ds";
					osg::ref_ptr<osg::Node> obj = osg3DSFileFromDiorama(str);
					LoadCheck(obj.get(), str);

					osg::ref_ptr<osg::PositionAttitudeTransform> obj_pat = new osg::PositionAttitudeTransform();
					double scale = 80.1812; //サーバ側の値から決定した
					obj_pat->setScale(osg::Vec3d(scale,scale,scale));
					obj_pat->setAttitude(osg::Quat(
						osg::DegreesToRadians(50.f), osg::Vec3d(1.0, 0.0, 0.0),
						osg::DegreesToRadians(-50.f), osg::Vec3d(0.0, 1.0, 0.0),
						osg::DegreesToRadians(0.f), osg::Vec3d(0.0, 0.0, 1.0)
						));
					obj_pat->setPosition(osg::Vec3d(0.0, 0.0, 3.0));//shift body higher 3 units
					obj_pat->addChild(obj);

//					osgAddObjectNode(dynamic_cast<osg::Node *>(obj_pat.release()));


 it cannot be worked
					obj_node_array[1] = obj;
					obj_transform_array[1] = obj_pat;


					int childInd = obj_transform_array[1]->getChildIndex(obj_node_array[1]);
					obj_transform_array[1]->setChild(childInd, osgSphereNode(5));

					cout << "<<Replace objects>>" << endl;
				}
				else
				{
					cerr << "No object should be deleted!!" << endl;
				}
				break;
			}
			//衝突判定があった後、手に追従させるテクスチャを生成
			case 101: {

				//re-create new node
				const char * str = "ARMM/Data/rec/NewTorus.3ds";
				osg::ref_ptr<osg::Node> obj = osgTexture(str);
				LoadCheck(obj.get(), str);

				osg::ref_ptr<osg::PositionAttitudeTransform> obj_pat = new osg::PositionAttitudeTransform();
				double scale = 90; //適当
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
#endif SIM_MICROMACHINE

	}


	inline bool KeyboardController_client::getKey(int key) {
//		return GetAsyncKeyState(key)& 0x8000;
		return false;
	}

}  End of namespace ARMM
*/
