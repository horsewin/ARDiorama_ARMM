/*
 * KeyboardControlls_client.cpp
 *
 *  Created on: 2012/09/16
 *      Author: umakatsu
 */
#include "ARMM/KeyboardControlls_client.h"
#include "ARMM/Rendering/osg_Client.h"
#include "ARMM/Rendering/osg_Object.h"
#include "ARMM/Rendering/osg_geom_data.h"
#include "constant.h"

//OpenCV
#include <opencv/cv.h>

//Standard API
#include <iostream>
#include <cstring>

namespace ARMM
{

	using namespace std;

	extern int		collidedNodeInd;
	extern bool	collision;
	const float CUBE_SIZE = 4;
	const float SPHERE_SIZE = 2;//FULL SIZE

	extern std::string pDstModel; //actually this should be deal with member of Keyboard class, but some error occurred so could not do it...
	int KeyboardController_client::check_input()
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

	void KeyboardController_client::set_input(const int & key, boost::shared_ptr<osg_Client> osgrender)
	{
		if(!osgrender)
		{
			cerr << "Error: No rendering object is detected!" << endl;
			return;
		}

		const int offset = 79;
		switch(key) 		//A 65 S 83 D 68 W 87 F 70 V 86
		{
			case 78://N
				osgrender->osgAddObjectNode(osgSphereNode(SPHERE_SIZE));
				break;

			case 66: //B
				osgrender->osgAddObjectNode(osgBoxNode(CUBE_SIZE));
				break;

			// This function is called after texture transferred
			case 100: {
				SwapingObject(osgrender);
				break;
			}

			//creating a texture sticked in hand when collision occurred
			case 101:
			{
				const char * file = "LSCM_Newcube0.bmp";
				if(!RegisteringSoftObject(file, osgrender))
				{
					cerr << "Error: register soft object(texture) into rendering components" << endl;
				}
				break;
			}

			default:
				//adding a virtual object into the AR environment
				if( key > 78 && (key - offset) < static_cast<int>(mKeyAssignment.size()) )
				{
					if(!RegisteringObject(mKeyAssignment.at(key-offset).second.c_str(), osgrender))
					{
						cerr << "Error: register object into rendering components" << endl;
					}

					//初期設定はinvisibleにする
					osgrender->getOsgObject()->getObjTransformArray().back()->setNodeMask(0);

					#if USE_OSGMENU == 1
					osgrender->ToggleMenuVisibility();
					osgrender->ToggleModelButtonVisibility();
					osgrender->ToggleVirtualObjVisibility();
					osgrender->ResetModelButtonPos();
					#endif
				}
				//button input
				else if( key > 200 && key < 300)
				{
					if(key == ConstParams::ADDMODELBUTTON)
					{
						//rendering list of models
						//rendering new button for cancal action
						osgrender->ToggleModelButtonVisibility();

						//disappearing all buttons and virtual objects temporary
						osgrender->ToggleMenuVisibility();
						osgrender->ToggleVirtualObjVisibility();
					}
					else if(key == ConstParams::RESETBUTTON)
					{
						osgrender->ResetAllNodes();
					}
					else if(key == ConstParams::STARTTRANSBUTTON)
					{
						//Either source or destination model are not found
						if( osgrender->getOsgObject()->getObjNodeArray().size() < 2)
						{
							cerr << "No enough model is found : need two at least" << endl;
						}
						//having two models at least in AR env
						else
						{
							osgrender->ToggleMenuVisibility();	//menu should be disappeared
						}
					}
				}
				//model button input
				else if( key > 300 && key < 400)
				{
					if( key == ConstParams::CANCELMODELBUTTON)
					{
						osgrender->ToggleMenuVisibility();
						osgrender->ToggleModelButtonVisibility();
						osgrender->ToggleVirtualObjVisibility();
						osgrender->ResetModelButtonPos();
					}
				}
				break;

		}
	}

	bool KeyboardController_client::getKey(int key)
	{
		return false;
	}

	bool KeyboardController_client::RegisteringObject(const char * filename, boost::shared_ptr<osg_Client> osgrender)
	{
		float scale = 10;
		ostringstream str;
		const char * format = ".3ds";

		str << ConstParams::DATABASEDIR << filename << "/" << filename << format;
		osg::ref_ptr<osg::Node> obj = osgDB::readNodeFile(str.str().c_str());
		if(!LoadCheck(obj.get(), str.str().c_str()))
		{
			return false;
		}

		string tmpFilename(filename);
		tmpFilename += format;
		obj->setName(tmpFilename);

////				//安原モデルの場合、テクスチャ座標の調整が必要(Cube)
//				string str(DATABASEDIR);
//				const char * FILENAME = "cube";
//				str += FILENAME; str+="/";
//				string modelFileName(FILENAME);
//				modelFileName += FORMAT;
//				osg::ref_ptr<osg::Node> obj = (osg3DSFileFromDiorama2(modelFileName.c_str(), str.c_str()));
//				obj->setName(str);
//				double scale = 20; //for (Cube) for 安原モデル ただしosg3DSFileFromDioramaで調整されていること
////				double scale = 300; //for (keyboard) for 安原モデル

//				//安原モデルの場合、テクスチャ座標の調整が必要(Keyboard)
//				const char * FILENAME = "keyboard";
//				double scale = 300; //サーバ側の値から決定した for (keyboard) for 安原モデル
		osgrender->osgAddObjectNode(obj.get(), scale);
		return true;
	}

	void KeyboardController_client::SwapingObject( boost::shared_ptr<osg_Client> osgrender )
	{
		if(pDstModel.empty())
		{
			cerr << "No name of dst model" << endl;
			return;
		}

		printf("Destination model = %s\n", pDstModel.c_str());

		ostringstream str;
//				const char * FILENAME = "Newcow";
//				str << ConstParams::DATABASEDIR << FILENAME << "/" << FILENAME << FORMAT;
		string dir = pDstModel.substr(0, pDstModel.size()-4);
		str << ConstParams::DATABASEDIR << "New" <<  dir.c_str() << "/" << "New" << pDstModel.c_str();

		//remove the texture image object
		osg::ref_ptr<osg::Node> pObjTexture = osgrender->getOsgObject()->getObjTexture();
		osg::ref_ptr<osg::PositionAttitudeTransform> pObjTexturePosAtt = osgrender->getOsgObject()->getObjTexturePosAtt();
		osgrender->getShadowedScene()->removeChild(pObjTexture);
		pObjTexturePosAtt->removeChild(pObjTexture);
		osgrender->getOsgObject()->setSoftTexture(false);

		//getting the index of the child node
		vector<osg::Node*> pObjNodeArray
			= osgrender->getOsgObject()->getObjNodeArray();
		vector<osg::PositionAttitudeTransform *> pObjTransformArray
			= osgrender->getOsgObject()->getObjTransformArray();
		int childInd = pObjTransformArray[collidedNodeInd]
					   ->getChildIndex(pObjNodeArray[collidedNodeInd]);

		//swap a child of the objects node with new child node
		osg::ref_ptr<osg::Node> obj = osgDB::readNodeFile(str.str().c_str());
		LoadCheck(obj.get(), str.str().c_str());
		str.clear();
		str << "New" << pDstModel.c_str();
		pObjTransformArray[collidedNodeInd]->setChild(childInd, obj.get());
//		pObjNodeArray[collidedNodeInd]->setName(str.str());
		double scale = 10; //
//				double scale = 20; //安原Cube
//				double scale = 300;
		pObjTransformArray[collidedNodeInd]->setScale(osg::Vec3d(scale,scale,scale));
		collision = false;
	}

	bool KeyboardController_client::RegisteringSoftObject(const char * filename, boost::shared_ptr<osg_Client> osgrender)
	{
		osg::ref_ptr<osg::Node> pObjTexture = osgrender->getOsgObject()->getObjTexture();

		//parts node
		string str(ConstParams::DATABASEDIR);
		str += filename;
		pObjTexture = osgCreateSoft(str.c_str());
		if(!LoadCheck(pObjTexture.get(), str.c_str()))
		{
			return false;
		}

		osg::ref_ptr<osg::PositionAttitudeTransform> pObjTexturePosAtt = new osg::PositionAttitudeTransform();
		pObjTexturePosAtt->addChild(pObjTexture);
		pObjTexturePosAtt->getOrCreateStateSet()->setRenderBinDetails(2, "RenderBin");

		osgrender->getShadowedScene()->addChild(pObjTexture);
		osgrender->getOsgObject()->setObjTexture(pObjTexture);

//				osgrender->getShadowedScene()->addChild(pObjTexturePosAtt);
		osgrender->getOsgObject()->setObjTexturePosAtt(pObjTexturePosAtt);
//				osgrender->getOsgObject()->setSoftTexture(true);

		return true;
	}

	void KeyboardController_client::AssignmentKeyinput(const char * settingFilename)
	{
		ostringstream setInput;
		setInput <<  ConstParams::DATABASEDIR << settingFilename;

		std::ifstream input(setInput.str().c_str());

		if(!input.is_open())
		{
			cerr << "Setting file cannot be openned!!" << endl;
			cerr << "Filename is " << setInput.str().c_str() << endl;
			exit(EXIT_SUCCESS);
		}

		while(input)
		{
			char line[1024] ;
			input.getline(line, 1024) ;
			std::stringstream line_input(line) ;

			pair<unsigned int, string> tmpKeyAssignment;

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

		cout << mKeyAssignment.size() << endl;
	}

	KeyboardController_client::KeyboardController_client()
	{
		AssignmentKeyinput("setting.txt");
	}

	KeyboardController_client::~KeyboardController_client()
	{

	}
}

