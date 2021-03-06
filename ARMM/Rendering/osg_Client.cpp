/*
 * osg_Client.cpp
 *
 *  Created on: 2012/09/16
 *      Author: umakatsu
 */

#include "ARMM/Rendering/osg_Client.h"
#include "ARMM/Rendering/osg_Object.h"
#include "ARMM/Rendering/osg_Menu.h"

#include "ARMM/MyShadowMap.h"

//Standard API
#include <sstream>
#include <cassert>
#include <map>

//Tracking library
#include "OPIRALibraryMT.h"
#include "RegistrationAlgorithms/OCVSurf.h"

#include "constant.h"

#include <osgUtil/SmoothingVisitor>
#include <osgShadow/ShadowMap>

namespace ARMM
{
	const osg::Quat DEFAULTATTIDUTE = osg::Quat(
			osg::DegreesToRadians(0.f), osg::Vec3d(1.0, 0.0, 0.0),
			osg::DegreesToRadians(0.f), osg::Vec3d(0.0, 1.0, 0.0),
			osg::DegreesToRadians(0.f), osg::Vec3d(0.0, 0.0, 1.0)
			);

	const
	//---------------------------------------------------------------------------
	// Global
	//---------------------------------------------------------------------------
	//to show terrain(this variable don't be needed)
	bool WIREFRAME_MODE = false;

	//marker origin
	CvPoint2D32f center_trimesh;
	float TrimeshScale = 1;

	int  collidedNodeInd = 0;
	bool collision = false;
	bool softTexture = false;

	bool handAppear = false;

	//hand manipulation
	std::vector<osg::PositionAttitudeTransform*> hand_object_global_array;
	std::vector<osg::Drawable*> hand_object_drawble_array[ConstParams::MAX_NUM_HANDS];
	std::vector<osg::PositionAttitudeTransform*> hand_object_transform_array[ConstParams::MAX_NUM_HANDS];

	//soft texture manipulation
	extern osg::Vec3d softTexCoord[ConstParams::resX*ConstParams::resY];

	class ARTrackedNode : public osg::Group
	{
		private:
			OPIRALibrary::Registration *r;
			typedef std::map<std::string, osg::ref_ptr<osg::MatrixTransform> > MarkerMap;
			MarkerMap mMarkers;
			osg::PositionAttitudeTransform* osgTrans;
			osg::Light* light;

		public:
		ARTrackedNode(): osg::Group()
		{
			r = new RegistrationOPIRAMT(new OCVSurf());
			osg::Group* lightGroup = new osg::Group;
			// Create a local light.
			light = new osg::Light();
			light->setLightNum(0);
			light->setAmbient(osg::Vec4(0.4f,0.4f,0.4f,1.0f));
			light->setDiffuse(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
			light->setSpecular(osg::Vec4(0.4f,0.4f,0.4f,1.0f));
			light->setConstantAttenuation(0.2f);
			light->setLinearAttenuation(0.05f);
			light->setQuadraticAttenuation(0.05f);
			light->setPosition(osg::Vec4(30, -5, 5, 0.0));

			osg::LightSource* lightSource = new osg::LightSource;
			lightSource->setLight(light);
			lightSource->setLocalStateSetModes(osg::StateAttribute::ON);
			lightSource->setStateSetModes(*this->getOrCreateStateSet(), osg::StateAttribute::ON);
			lightGroup->addChild(lightSource);

			this->addChild(lightGroup);
		}

		osg::Light *getLight()
		{
			return light;
		}

		void stop() {
			delete r;
		}

		void setVisible(int index, bool visible)
		{
			osg::ref_ptr<osg::Switch> s = (osg::Switch*)this->getChild(index);
			visible? s->setAllChildrenOn() : s->setAllChildrenOff();
		}

		void processFrame(IplImage *mFrame, CvMat *cParams, CvMat *cDistort)
		{
			for (MarkerMap::iterator iter = mMarkers.begin(); iter != mMarkers.end(); iter++)
			{
				osg::ref_ptr<osg::Switch> s = (osg::Switch*)iter->second->getChild(0); s.get()->setAllChildrenOff();
			}
			std::vector<MarkerTransform> mTransforms = r->performRegistration(mFrame, cParams, cDistort);
			for (std::vector<MarkerTransform>::iterator iter = mTransforms.begin(); iter != mTransforms.end(); iter++) {
				MarkerMap::iterator mIter = mMarkers.find(iter->marker.name);
				if (mIter != mMarkers.end()) {
					osg::ref_ptr<osg::MatrixTransform> m = mIter->second;
					m.get()->setMatrix(osg::Matrixd(iter->transMat));
					((osg::Switch*)m->getChild(0))->setAllChildrenOn();
				}
			}
			for (uint i=0; i<mTransforms.size(); i++) {
				free(mTransforms.at(i).viewPort); free(mTransforms.at(i).projMat); 	free(mTransforms.at(i).transMat);
			}
			mTransforms.clear();
		}

		int addMarkerContent(string imageFile, int maxLengthSize, int maxLengthScale, osg::Node *node) {
			r->removeMarker(imageFile);
			if (r->addResizedScaledMarker(imageFile, maxLengthSize, maxLengthScale)) {
				Marker m = r->getMarker(imageFile);

				osgTrans = new osg::PositionAttitudeTransform();
				osgTrans->setAttitude(osg::Quat(
							osg::DegreesToRadians(0.f), osg::Vec3d(1.0, 0.0, 0.0),
							osg::DegreesToRadians(180.f), osg::Vec3d(0.0, 1.0, 0.0),
							osg::DegreesToRadians(180.f), osg::Vec3d(0.0, 0.0, 1.0)
							));
				osgTrans->setPosition(osg::Vec3d(0.0, 0.0, 0.0));
				osgTrans->getOrCreateStateSet()->setMode(GL_NORMALIZE, GL_TRUE);
				osgTrans->addChild(node);

				osg::ref_ptr<osg::Switch> foundObject = new osg::Switch();
				foundObject->addChild(osgTrans);

				osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform();
				mt->addChild(foundObject);

				osg::ref_ptr<osg::Switch> trackObject = new osg::Switch();
				trackObject->addChild(mt);
				this->addChild(trackObject);

				mMarkers[imageFile] = mt;
				return this->getChildIndex(trackObject);
			} else {
				return -1;
			}
		}

		osg::Node* getOsgTrans() {
			return osgTrans;
		}

		void addModel(osg::Node *node) {
			osgTrans->addChild(node);
		}

		void removeModel(osg::Node *node) {
			osgTrans->removeChild(node);
		}
	};

	osg::ref_ptr<osg::Image> mVideoImage;
	cv::Ptr<IplImage> mGLImage;

	osg::ref_ptr<ARTrackedNode> arTrackedNode;
	osg::ref_ptr<osg::Node> HFNode;

	//HeightField
	osg::Vec3Array* HeightFieldPoints = new osg::Vec3Array;
	osg::Geometry* HeightFieldGeometry_quad = new osg::Geometry();
	osg::Geometry* HeightFieldGeometry_line = new osg::Geometry();
	osg::Vec4Array* groundQuadColor = new osg::Vec4Array();
	osg::Vec4Array* groundLineColor = new osg::Vec4Array();

	int celicaIndex;
	std::vector<osg::PositionAttitudeTransform*> car_transform;
	std::vector<osg::PositionAttitudeTransform*> wheel_transform[2];

	//main viewer
	osgViewer::Viewer		viewer;

	//---------------------------------------------------------------------------
	// Code
	//---------------------------------------------------------------------------

	void osg_Client::osg_init(double *projMatrix)
	{
		mOsgObject	= boost::shared_ptr<osg_Object>(new osg_Object());
		mOsgMenu	= boost::shared_ptr<osg_Menu>(new osg_Menu());

		//setting shadow mask
		invisibleMask		= 0x0;
		rcvShadowMask		= 0x1;
		castShadowMask	= 0x2;

		//background image
		mVideoImage = new osg::Image();
		mGLImage = cvCreateImage(cvSize(512,512), IPL_DEPTH_8U, 3);

		// run optimization over the scene graph
		osg::ref_ptr<osg::Group> root = new osg::Group();
		osgUtil::Optimizer optimzer;
		optimzer.optimize(root.get());

		//setting parameters for osg viewer
		viewer.addEventHandler(new osgViewer::WindowSizeHandler());
		viewer.setUpViewInWindow(100, 100, ConstParams::WINDOW_WIDTH, ConstParams::WINDOW_HEIGHT);
		viewer.setThreadingModel(osgViewer::Viewer::SingleThreaded);
		viewer.setKeyEventSetsDone(0);
		viewer.setSceneData(root.get());

		HFNode = new osg::Node;
		HeightFieldPoints = new osg::Vec3Array;

		//Create Height Field
		//TODO サーバ側のdepth perception結果を使う
		for (int i=0; i<159; i++) {
			for (int j=0; j<240; j++) {
				HeightFieldPoints->push_back(osg::Vec3(i-19, j-119, -2));
				HeightFieldPoints->push_back(osg::Vec3(i-18, j-119, -2));
				HeightFieldPoints->push_back(osg::Vec3(i-18, j-118, -2));
				HeightFieldPoints->push_back(osg::Vec3(i-19, j-118, -2));
			}
		}

		// ----------------------------------------------------------------
		// Video background
		// ----------------------------------------------------------------
		osg::ref_ptr<osg::Camera> bgCamera = new osg::Camera();
		bgCamera->getOrCreateStateSet()->setRenderBinDetails(0, "RenderBin");
		bgCamera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
		bgCamera->setClearMask(GL_DEPTH_BUFFER_BIT);
		bgCamera->getOrCreateStateSet()->setMode(GL_LIGHTING, GL_TRUE);
		bgCamera->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, GL_TRUE);
		bgCamera->setProjectionMatrixAsOrtho2D(0, ConstParams::WINDOW_WIDTH, 0, ConstParams::WINDOW_HEIGHT);

		osg::ref_ptr<osg::Geometry> geom = osg::createTexturedQuadGeometry(osg::Vec3(0, 0, 0), osg::X_AXIS * ConstParams::WINDOW_WIDTH, osg::Y_AXIS * ConstParams::WINDOW_HEIGHT, 0, 1, 1, 0);
		geom->getOrCreateStateSet()->setTextureAttributeAndModes(0, new osg::Texture2D(mVideoImage.get()));

		osg::ref_ptr<osg::Geode> geode = new osg::Geode();
		geode->addDrawable(geom.get());
		bgCamera->addChild(geode.get());
		bgCamera->setNodeMask(rcvShadowMask);
		root->addChild(bgCamera.get());

		// ----------------------------------------------------------------
		// Foreground 3D content
		// ----------------------------------------------------------------
		osg::ref_ptr<osg::Camera> fgCamera = new osg::Camera();
		fgCamera->getOrCreateStateSet()->setRenderBinDetails(1, "RenderBin");
		fgCamera->setReferenceFrame(osg::Transform::ABSOLUTE_RF_INHERIT_VIEWPOINT);
		fgCamera->setClearMask(GL_DEPTH_BUFFER_BIT);
		fgCamera->setProjectionMatrix(osg::Matrix(projMatrix));
		root->addChild(fgCamera.get());
		arTrackedNode = new ARTrackedNode();
		fgCamera->addChild(arTrackedNode.get());

		//for AR menu button animation
		gAddModelAnimation = 0.0;
		ResetModelButtonPos();
	};


	void osg_Client::osg_inittracker(string markerName, int maxLengthSize, int maxLengthScale)
	{
		static bool hasInit = false;

		if (hasInit)
		{
			arTrackedNode->removeChildren(0,arTrackedNode->getNumChildren());
			celicaIndex = arTrackedNode->addMarkerContent(markerName, maxLengthSize, maxLengthScale, shadowedScene);
			arTrackedNode->setVisible(celicaIndex, true);
			return;
		}

		arTrackedNode->removeChildren(0,arTrackedNode->getNumChildren());

		// Set shadow node
		//osg::ref_ptr<osgShadow::ShadowTexture> sm = new osgShadow::ShadowTexture; //V
		//osg::ref_ptr<osgShadow::MyShadowMap> sm = new osgShadow::MyShadowMap; //Adrian
		osg::ref_ptr<osgShadow::ShadowMap> sm = new osgShadow::ShadowMap; //Atsushi
		//sm->setLight(arTrackedNode->getLight()); //Atsushi
		sm->setTextureSize( osg::Vec2s(1024, 1024) ); //Adrian
		sm->setTextureUnit( 1 );

		shadowedScene = new osgShadow::ShadowedScene;
		shadowedScene->setShadowTechnique( sm.get() );
		shadowedScene->setReceivesShadowTraversalMask( rcvShadowMask );
		shadowedScene->setCastsShadowTraversalMask( castShadowMask );
		shadowedScene->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON); //Adrian

//		osg::ref_ptr<osg::LightSource> source = new osg::LightSource; //V
//		source->getLight()->setPosition( osg::Vec4(4.0, 8.0, 10.0, 0.0) ); //V
//		source->getLight()->setAmbient( osg::Vec4(0.1, 0.1, 0.1, 1.0) ); //V
//		source->getLight()->setDiffuse( osg::Vec4(0.9, 0.9, 0.9, 1.0) ); //V
//		shadowedScene->addChild(source);


		celicaIndex = arTrackedNode->addMarkerContent(markerName, maxLengthSize, maxLengthScale, shadowedScene);
		arTrackedNode->setVisible(celicaIndex, true);


	/*Adrian */
		{
			// Create the Heightfield Geometry
			HeightFieldGeometry_quad->setVertexArray(HeightFieldPoints);
			HeightFieldGeometry_quad->addPrimitiveSet(new osg::DrawArrays( GL_QUADS, 0, HeightFieldPoints->size()));
			HeightFieldGeometry_quad->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);

			HeightFieldGeometry_line->setVertexArray(HeightFieldPoints);
			HeightFieldGeometry_line->addPrimitiveSet(new osg::DrawArrays( GL_LINES, 0, HeightFieldPoints->size()));
			HeightFieldGeometry_line->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);

			//Set the Heightfield to be alpha invisible
			HeightFieldGeometry_quad->setColorBinding(osg::Geometry::BIND_OVERALL);
			HeightFieldGeometry_line->setColorBinding(osg::Geometry::BIND_OVERALL);
			//osg::Vec4Array* col = new osg::Vec4Array();
			HeightFieldGeometry_quad->setColorArray(groundQuadColor);
			HeightFieldGeometry_line->setColorArray(groundLineColor);
			groundQuadColor->push_back(osg::Vec4(1,1,1,0.1));
			groundLineColor->push_back(osg::Vec4(1,0.2,0,0.0));

			//Create the containing geode
			osg::ref_ptr< osg::Geode > geode = new osg::Geode();
			geode->addDrawable(HeightFieldGeometry_quad);
			geode->addDrawable(HeightFieldGeometry_line);

			//Create the containing transform
			float scale = 10; float x = 0; float y = 0; float z = 0;
			osg::ref_ptr< osg::PositionAttitudeTransform > mt = new osg::PositionAttitudeTransform();
			mt->setScale(osg::Vec3d(scale,scale,scale));
			mt->setAttitude(osg::Quat(0,0,0,1));
			mt->setPosition(osg::Vec3d(x, y, z));
//			osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform;
//			mt->setMatrix( osg::Matrix::translate(x,y,z));
			mt->addChild(geode.get());
			mt->setNodeMask(rcvShadowMask);

			//Set up the depth testing for the landscale
			osg::Depth * depth = new osg::Depth();
			depth->setWriteMask(true);
			depth->setFunction(osg::Depth::LEQUAL);
			mt->getOrCreateStateSet()->setAttributeAndModes(depth, osg::StateAttribute::ON);

			//Set up the shadow masks
//			mt->getOrCreateStateSet()->setRenderBinDetails(0, "RenderBin");

			//At the heightmap twice, once for shadowing and once for occlusion
			arTrackedNode->addModel(mt);
			shadowedScene->addChild(mt);
			shadowedScene->getOrCreateStateSet()->setRenderBinDetails(2, "RenderBin");
		}
	/*Adrian*/

		//Added by Atsushi
		// TODO this value depends on the state of Kinect Calibration
		REP(i,ConstParams::MAX_NUM_HANDS)
		{
			osg_createHand(i, 0.58, 1.666667); //second: world scale, 3rd:ratio
		}
		hasInit = true;
	}

	void osg_Client::OsgInitMenu()
	{
		mOsgMenu->CreateMenuPane();

		std::vector< osg::ref_ptr<osg::PositionAttitudeTransform> > pTransArray = mOsgMenu->getObjMenuTransformArray();

		//add menu object into the AR scene
		osg::ref_ptr<osg::Group> menu = new osg::Group;
		REP(idx, pTransArray.size())
		{
			pTransArray.at(idx)->setNodeMask(castShadowMask);
			menu->addChild(pTransArray.at(idx));
		}

		osg::ref_ptr<osg::PositionAttitudeTransform> menuTrans = new osg::PositionAttitudeTransform;
		const osg::Vec3d BASEPOSITION(0,0,0);
		const osg::Quat BASEATTITUDE = osg::Quat(
				osg::DegreesToRadians(0.f), osg::Vec3d(1.0, 0.0, 0.0),
				osg::DegreesToRadians(0.f), osg::Vec3d(0.0, 1.0, 0.0),
				osg::DegreesToRadians(0.f), osg::Vec3d(0.0, 0.0, 1.0)
		);
		menuTrans->setAttitude(BASEATTITUDE);
		menuTrans->setPosition(BASEPOSITION);
		menuTrans->addChild(menu.get());

		shadowedScene->addChild(menuTrans.get());

		mOsgMenu->setObjMenuTransformArray(pTransArray);
	}

	void osg_Client::OsgInitModelButton()
	{
		mOsgMenu->CreateModelButtonCloud();

		std::vector< osg::ref_ptr<osg::PositionAttitudeTransform> > pTransArray = mOsgMenu->getMenuModelTransArray();
		//add menu object into the AR scene
		osg::ref_ptr<osg::Group> menu = new osg::Group;
		REP(idx, pTransArray.size())
		{
			pTransArray.at(idx)->setNodeMask(castShadowMask);
			menu->addChild(pTransArray.at(idx));
		}

		osg::ref_ptr<osg::PositionAttitudeTransform> menuTrans = new osg::PositionAttitudeTransform;
		const osg::Vec3d BASEPOSITION(0,0,0);
		const osg::Quat BASEATTITUDE = osg::Quat(
				osg::DegreesToRadians(0.f), osg::Vec3d(1.0, 0.0, 0.0),
				osg::DegreesToRadians(0.f), osg::Vec3d(0.0, 1.0, 0.0),
				osg::DegreesToRadians(0.f), osg::Vec3d(0.0, 0.0, 1.0)
		);
		menuTrans->setAttitude(BASEATTITUDE);
		menuTrans->setPosition(BASEPOSITION);
		menuTrans->addChild(menu.get());

		shadowedScene->addChild(menuTrans.get());

		mOsgMenu->setMenuModelTransArray(pTransArray);

		//change the condition of regular state if lists of models is shown in AR space into invisible condition
		if(IsModelButtonVisibiilty())
		{
			ToggleModelButtonVisibility();
		}

	}

	void osg_Client::osg_client_render(IplImage *newFrame, CvMat *cParams, CvMat *cDistort)
	{
		assert( newFrame != NULL);

		if(gAddModelAnimation >0.001 || gAddModelAnimation < -0.001) //means "!= 0.0"
		{
			ModelButtonAnimation();
		}

		cvResize(newFrame, mGLImage);
		cvCvtColor(mGLImage, mGLImage, CV_BGR2RGB);
		mVideoImage->setImage(mGLImage->width, mGLImage->height, 0, 3, GL_RGB, GL_UNSIGNED_BYTE, (unsigned char*)mGLImage->imageData, osg::Image::NO_DELETE);

		if(mOsgObject->isSoftTexture())
		{
			osg_UpdateSoftTexture();
		}

		if (!viewer.done())
		{
			if (CAPTURE_SIZE.width != REGISTRATION_SIZE.width || CAPTURE_SIZE.height != REGISTRATION_SIZE.height) {
				double scale = double(REGISTRATION_SIZE.width)/double(CAPTURE_SIZE.width);
				IplImage *scaledImage = cvCreateImage(cvSize(newFrame->width*scale, newFrame->height*scale), newFrame->depth, newFrame->nChannels); cvResize(newFrame, scaledImage);
				arTrackedNode->processFrame(scaledImage, cParams, cDistort);
				cvReleaseImage(&scaledImage);
			}
			else
			{
				arTrackedNode->processFrame(newFrame, cParams, cDistort);
			}
			viewer.frame();
		}
	}

	void osg_Client::osg_client_render(IplImage *newFrame, CvMat *cParams, CvMat *cDistort, std::vector <osg::Quat> q_array,std::vector <osg::Vec3d>  v_array)
	{
		assert( newFrame != NULL);

		if(gAddModelAnimation >0.001 || gAddModelAnimation < -0.001) //means "!= 0.0"
		{
			ModelButtonAnimation();
		}

		cvResize(newFrame, mGLImage);
		cvCvtColor(mGLImage, mGLImage, CV_BGR2RGB);
		mVideoImage->setImage(mGLImage->width, mGLImage->height, 0, 3, GL_RGB, GL_UNSIGNED_BYTE, (unsigned char*)mGLImage->imageData, osg::Image::NO_DELETE);

		//virtual objects part
		for(uint i = 0; i < v_array.size(); i++)
		{
			mOsgObject->SetObjTransformArrayIndex(i, q_array.at(i), v_array.at(i));
		}

		//soft texture part
		if(mOsgObject->isSoftTexture())
		{
			osg_UpdateSoftTexture();
		}

		if (!viewer.done())
		{
			if (CAPTURE_SIZE.width != REGISTRATION_SIZE.width || CAPTURE_SIZE.height != REGISTRATION_SIZE.height)
			{
				double scale = double(REGISTRATION_SIZE.width)/double(CAPTURE_SIZE.width);
				IplImage *scaledImage = cvCreateImage(cvSize(newFrame->width*scale, newFrame->height*scale), newFrame->depth, newFrame->nChannels); cvResize(newFrame, scaledImage);
				arTrackedNode->processFrame(scaledImage, cParams, cDistort);
				cvReleaseImage(&scaledImage);
			}
			else
			{
				arTrackedNode->processFrame(newFrame, cParams, cDistort);
			}

			//update main OSG window
			viewer.frame();
		}
	}

	void osg_Client::osg_uninit()
	{
		arTrackedNode->stop();
	}

	void osg_Client::osg_UpdateHeightfieldTrimesh(float *ground_grid)
	{
		int index =0;
		for(int i = 0; i < 159; i++) {
			for(int j = 0; j < 119; j++) {
				float x = (float)(i- center_trimesh.x)*TrimeshScale;
				float y = (float)(j- (120-center_trimesh.y))*TrimeshScale;
				HeightFieldPoints->at(index++) = osg::Vec3(x, y, ground_grid[j*160+i]);
				HeightFieldPoints->at(index++) = osg::Vec3(x+TrimeshScale, y, ground_grid[j*160+i+1]);
				HeightFieldPoints->at(index++) = osg::Vec3(x+TrimeshScale, y+TrimeshScale, ground_grid[(j+1)*160+i+1]);
				HeightFieldPoints->at(index++) = osg::Vec3(x, y+TrimeshScale, ground_grid[(j+1)*160+i]);
			}
		}
		HeightFieldGeometry_quad->dirtyDisplayList();
		HeightFieldGeometry_line->dirtyDisplayList();
	}

	void osg_Client::osgAddObjectNode(osg::Node* n)
	{
		mOsgObject->PushObjNodeArray(n);

		std::vector<osg::PositionAttitudeTransform*> pTransArray = mOsgObject->getObjTransformArray();
		pTransArray.push_back(new osg::PositionAttitudeTransform());

		int index = pTransArray.size()-1;
		pTransArray.at(index)->setAttitude(DEFAULTATTIDUTE);
		pTransArray.at(index)->setPosition(osg::Vec3d(0.0, 0.0, 0.0));
		pTransArray.at(index)->setNodeMask(castShadowMask );

		pTransArray.at(index)->addChild(mOsgObject->GetObjNodeIndex(index));
		shadowedScene->addChild( pTransArray.at(index) );
		pTransArray.at(index)->getOrCreateStateSet()->setRenderBinDetails(1, "RenderBin");
		shadowedScene->getOrCreateStateSet()->setRenderBinDetails(1, "RenderBin");

		mOsgObject->IncrementObjIndex();

		printf("Client Object(%s). Number: %d, All of Ind=%d\n", n->getName().c_str(), index+1, mOsgObject->getObjectIndex());
		mOsgObject->IncrementObjCount();

		mOsgObject->setObjTransformArray(pTransArray);
	}

	void osg_Client::osgAddObjectNode(osg::Node* n, const double & scale)
	{
		osgAddObjectNode(n);

		std::vector<osg::PositionAttitudeTransform*> pTransArray = mOsgObject->getObjTransformArray();
		int index = pTransArray.size() - 1;

		pTransArray.at(index)->setScale(osg::Vec3d(scale,scale,scale));

		mOsgObject->setObjTransformArray(pTransArray);
	}

	void osg_Client::osgAddObjectNode(osg::Node* n, const double & scale, const osg::Quat & quat)
	{
		osgAddObjectNode(n);

		std::vector<osg::PositionAttitudeTransform*> pTransArray =
				mOsgObject->getObjTransformArray();
		int index = pTransArray.size()-1;

		pTransArray.at(index)->setScale(osg::Vec3d(scale,scale,scale));
		pTransArray.at(index)->setAttitude(quat);

		mOsgObject->setObjTransformArray(pTransArray);
	}

	//----->Hand creating and updating
	void osg_Client::osg_createHand(int index, float world_scale, float ratio)
	{
		//for virtual hands
		const osg::Vec4 HANDSPHERECOLOR =		osg::Vec4(0.9451, 0.7333, 0.5765, 0.5);
		const osg::Vec4 HANDSPHERECOLLIDECOL =	osg::Vec4(1.0, 1.0, 1.0, 1.0);

		float sphere_size = world_scale * ratio;
		//sphere_size = 0.984286;
		const int scale = 10;
		cout << "Sphere size = " << world_scale*ratio << endl;
		hand_object_global_array.push_back(new osg::PositionAttitudeTransform());

		for(int i = 0; i < ConstParams::MIN_HAND_PIX; i++)
		{
			for(int j = 0; j < ConstParams::MIN_HAND_PIX; j++)
			{
				//create a part of hands
				osg::ref_ptr< osg::Sphere > sphere = new osg::Sphere(osg::Vec3d(0,0,0), sphere_size);
				osg::ref_ptr< osg::ShapeDrawable > shape = new osg::ShapeDrawable( sphere.get() );

				hand_object_drawble_array[0].push_back(shape); //reserve a drawable object

				shape->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
				shape->setColor(HANDSPHERECOLOR);
				osg::ref_ptr< osg::Geode> geode = new osg::Geode();
				geode->addDrawable( shape.get() );

				// register hand's index
				int curr = hand_object_transform_array[index].size();
				hand_object_transform_array[index].push_back(new osg::PositionAttitudeTransform());

				//set hand's attributes
				hand_object_transform_array[index].at(curr)->setScale(osg::Vec3d(scale,scale,scale));
				hand_object_transform_array[index].at(curr)->setPosition(osg::Vec3d(j*scale, i*scale, -10*scale));

				// set a created hand to the graphics tree
				hand_object_transform_array[index].at(curr)->addChild( geode.get() );
				hand_object_global_array.at(index)->addChild( hand_object_transform_array[index].at(curr));
				hand_object_transform_array[index].at(curr)->setNodeMask(rcvShadowMask);
			}
		}
		hand_object_global_array.at(index)->setPosition(osg::Vec3d(0,0,0));
//		hand_object_global_array.at(index)->setNodeMask(0); // added by Atsushi 2012/06/06
		shadowedScene->addChild( hand_object_global_array.at(index) );

		//set rendering order
		hand_object_global_array.at(index)->getOrCreateStateSet()->setRenderBinDetails(2, "RenderBin");
	}

	void osg_Client::osg_UpdateHand(int index, float *x, float *y, float *grid)
	{
		const int scale = 10; // to convert cm to mm scale
		const float HEIGHT_LIMITATION = 500;
		hand_object_global_array.at(index)->setPosition(osg::Vec3d(0, 0, 0));
		for(int i = 0; i < ConstParams::MIN_HAND_PIX; i++) {
			for(int j = 0; j < ConstParams::MIN_HAND_PIX; j++) {
				int curr = i*ConstParams::MIN_HAND_PIX + j;
				if(grid[curr] > 0 && grid[curr] < HEIGHT_LIMITATION )
				{
//					cout << "HAND" << curr << " : "<< x[curr]*scale << "," << y[curr]*scale << "," << grid[curr]*scale << endl;
					if(!handAppear)
					{
						mOsgObject->IncrementObjIndex(ConstParams::MIN_HAND_PIX*ConstParams::MIN_HAND_PIX);
						cout << "Object Index = " << mOsgObject->getObjectIndex() << endl;
						handAppear = true;
					}

					hand_object_transform_array[index].at(curr)->setPosition(osg::Vec3d(x[curr]*scale, y[curr]*scale, grid[curr]*scale));
				}
				else
				{
	//				hand_object_transform_array[index].at(curr)->setPosition(osg::Vec3d(0,0,100));
					hand_object_transform_array[index].at(curr)->setPosition(osg::Vec3d(x[curr]*scale, y[curr]*scale, -500*scale));
				}
			}
		}

		//移動させるテクスチャを手に追従してうごかすために手の先端位置に移動させる
	//	if(collision){
	//		for(int i = 0; i < MIN_HAND_PIX; i++) {
	//			for(int j = 0; j < MIN_HAND_PIX; j++) {
	//				int curr = i*MIN_HAND_PIX + j;
	//
	//				if(grid[curr] > 0 && grid[curr] < HEIGHT_LIMITATION )
	//				{
	////					cout << "Finger : " << x[curr]*scale << "," << y[curr]*scale << "," << grid[curr]*scale << endl;
	////					osg::Node * n = hand_object_transform_array[0].at(curr)->getChild(0);
	////					osg::Geode * curGeode = dynamic_cast<osg::Geode*>(n);
	////					osg::ShapeDrawable* shapeDraw = dynamic_cast<osg::ShapeDrawable*>(curGeode->getDrawable(0));
	////					shapeDraw->setColor(HANDSPHERECOLLIDECOL);
	//
	//					obj_texture->setPosition(osg::Vec3d(x[curr]*scale, y[curr]*scale, grid[curr]*scale));
	//					obj_texture->setAttitude(obj_texture->getAttitude());
	//					return;
	//				}
	//			}
	//		}
	//	}
	}

	void osg_Client::osg_UpdateSoftTexture()
	{
		if(mOsgObject->getObjTexturePosAtt() == NULL) return;

		osg::Geode * geode(mOsgObject->getObjTexturePosAtt()->asGeode());
		if( geode == NULL) return;

		osg::Drawable * draw = geode->getDrawable(0);
		if( draw == NULL) return;

		osg::Geometry * geom(draw->asGeometry());
		osg::Vec3Array* verts( dynamic_cast< osg::Vec3Array* >( geom->getVertexArray() ) );

		osg::Vec3Array::iterator it( verts->begin() );

		//set the position of each node
		REP(idx, ConstParams::resX*ConstParams::resY)
		{
//			printf("(%f,%f,%f)\n",softTexCoord[idx].x(), softTexCoord[idx].y(), softTexCoord[idx].z());
			*it++ = softTexCoord[idx];
		}
		verts->dirty();
		draw->dirtyBound();

		// Generate new normals.
		osgUtil::SmoothingVisitor smooth;
		smooth.smooth( *geom );
		geom->getNormalArray()->dirty();
	}


	void osg_Client::ToggleMenuVisibility()
	{
		std::vector< osg::ref_ptr<osg::PositionAttitudeTransform> > pMenuTransArray  = mOsgMenu->getObjMenuTransformArray();

		double shiftVal = 1;
		REP(idx, pMenuTransArray.size())
		{
			if(pMenuTransArray.at(idx)->getNodeMask() == castShadowMask)
			{
				pMenuTransArray.at(idx)->setNodeMask(invisibleMask);

				//appear ar model buttons
				gAddModelAnimation = shiftVal;
			}
			else
			{
				pMenuTransArray.at(idx)->setNodeMask(castShadowMask);
			}
		}

		mOsgMenu->setObjMenuTransformArray(pMenuTransArray);
		mOsgMenu->ToggleMenuButtonState();
	}

	void osg_Client::ToggleModelButtonVisibility()
	{
		std::vector< osg::ref_ptr<osg::PositionAttitudeTransform> > pModelTransArray = mOsgMenu->getMenuModelTransArray();

		REP(idx, pModelTransArray.size())
		{
			if(pModelTransArray.at(idx)->getNodeMask() == castShadowMask)
			{
				pModelTransArray.at(idx)->setNodeMask(invisibleMask);
			}
			else
			{
				pModelTransArray.at(idx)->setNodeMask(castShadowMask);
			}
		}

		mOsgMenu->setMenuModelTransArray(pModelTransArray);
		mOsgMenu->ToggleModelButtonState();
	}

	void osg_Client::ToggleVirtualObjVisibility()
	{
		std::vector<osg::PositionAttitudeTransform*> pTransArray = mOsgObject->getObjTransformArray();

		REP(i,pTransArray.size())
		{
			unsigned int mask = pTransArray.at(i)->getNodeMask() ^ (castShadowMask);
			pTransArray.at(i)->setNodeMask(mask);
		}
		mOsgObject->setObjTransformArray(pTransArray);
	}

	bool osg_Client::IsMenuVisibiilty()
	{
		return mOsgMenu->isMenuButtonState();
	}

	bool osg_Client::IsModelButtonVisibiilty()
	{
		return mOsgMenu->isModelButtonState();
	}

	void osg_Client::ResetAllNodes()
	{
		mOsgObject->getObjNodeArray().clear();
		mOsgObject->getObjTransformArray().clear();
	}

	void osg_Client::ModelButtonAnimation()
	{
		std::vector< osg::ref_ptr<osg::PositionAttitudeTransform> > pModelTransArray = mOsgMenu->getMenuModelTransArray();

		//check if valid models are found?
		if(pModelTransArray.empty())
		{
			cerr << "No model button is found in osg.h" << endl;
			return;
		}

		//set the action in current frame
		double posZ = pModelTransArray.at(0)->getPosition().z();
		const double zThreshold = 6.0;
		if(posZ > zThreshold)
		{
			gAddModelAnimation = 0;
			return;
		}

		//set the pos of each model in current frame
		REP(idx, pModelTransArray.size())
		{
			osg::Vec3 newPos = pModelTransArray.at(idx)->getPosition();
			newPos.set(newPos.x(), newPos.y(), newPos.z() + gAddModelAnimation);
			pModelTransArray.at(idx)->setPosition(newPos);
		}

	}

	void osg_Client::ResetModelButtonPos()
	{
		std::vector< osg::ref_ptr<osg::PositionAttitudeTransform> > pModelTransArray = mOsgMenu->getMenuModelTransArray();

		if(pModelTransArray.empty())
		{
			cerr << "No model button is found in osg.h" << endl;
			return;
		}

		REP(idx, pModelTransArray.size())
		{
			osg::Vec3 newPos = pModelTransArray.at(idx)->getPosition();
			newPos.set(newPos.x(), newPos.y(), mOsgMenu->GetInitPosZ());
			pModelTransArray.at(idx)->setPosition(newPos);
		}

	}

	int osg_Client::GetOsgMenuAllObjectNum() const
	{
		return( mOsgMenu->getMenuModelObjectArray().size() + mOsgMenu->getObjMenuNodeArray().size());
	}
}
