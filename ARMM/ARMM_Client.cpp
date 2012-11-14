//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "ARMM/ARMM_Client.h"

#ifdef POINTGREYCAMERA
#include "ARMM/PointgreyCamera.h"
#include <gvars3/GStringUtil.h>
#include <gvars3/instances.h>
#include <cvd/image.h>
#include <cvd/rgb.h>
#include <cvd/byte.h>
#endif

//OpenCV
#include <opencv/cv.h>
#include <opencv/highgui.h>

//OPIRA
#include "OPIRAlibrary.h"
#include "OPIRALibraryMT.h"
#include "RegistrationAlgorithms/OCVSurf.h"

//Graphics calls
#include "ARMM/Rendering/osg_Client.h"
#include "ARMM/Rendering/osg_Object.h"

#include "ARMM/ARMM_vrpn.h"
#include "ARMM/leastsquaresquat.h"

//Network using VRPN
#include "vrpn_Tracker.h"
#include "vrpn_Connection.h"

//Keyboard controller
#include "ARMM/KeyboardControlls_client.h"

//to get FPS calculation
//#include "TickCounter.h"

//standard API
#include <cstdio>

namespace ARMM
{
	using namespace std;
	using namespace CVD;
	using namespace GVars3;

	//---------------------------------------------------------------------------
	// Global
	//---------------------------------------------------------------------------
	//for collision detection between objects
	int	  collisionInd;
	int	  collisionInd2;

	extern int  collidedNodeInd;
	extern bool collision;

	bool running = true;
	bool stroke = false;
	bool touch = false;

	float pCollision[3];
	int transfer;
	char filename[] = "/home/umakatsu/TextureTransfer/TextureTransfer/Model3DS/debug_remote.txt";
	std::ofstream output(filename);

	std::string pSrcModel;
	std::string pDstModel;

//	osgText::Text * fontText;

	//these coord are considered as Bullet coord
	//so you have to correct to use in OSG coord
	float hand_coord_x[ConstParams::MAX_NUM_HANDS][ConstParams::HAND_GRID_SIZE];
	float hand_coord_y[ConstParams::MAX_NUM_HANDS][ConstParams::HAND_GRID_SIZE];
	float hand_coord_z[ConstParams::MAX_NUM_HANDS][ConstParams::HAND_GRID_SIZE];
	osg::Vec3d softTexCoord[ConstParams::resX*ConstParams::resY];

	CvMat		*RegistrationParams;
	#ifdef USE_CLIENT_SENDER
	//send message to server
	vrpn_Connection_IP* m_Connection;
	ARMMClientSender* ARMM_client_sender;
	#endif

	//osg objects
#if CAR_SIMULATION == 1
	osg::Quat		CarsArrayQuat[ConstParams::NUM_CARS];
	osg::Quat		WheelsArrayQuat[ConstParams::NUM_CARS][4];
	osg::Vec3d		CarsArrayPos[ConstParams::NUM_CARS];
	osg::Vec3d		WheelsArrayPos[ConstParams::NUM_CARS][4];
#endif /* #if CAR_SIMULATION == 1 */
	osg::Vec3d		ObjectsArrayPos[ConstParams::NUM_OBJECTS];
	osg::Quat		ObjectsArrayQuat[ConstParams::NUM_OBJECTS];

	//---------------------------------------------------------------------------
	// Constant/Define
	//---------------------------------------------------------------------------
	const float OSG_SCALE = 10;
	const char *ARMM_SERVER_IP = "ARMM_Comm@192.168.100.106"; //this value depends on IP address assigned computer

	//---------------------------------------------------------------------------
	// Code
	//---------------------------------------------------------------------------
	///////// Sending message to server for client //////////
	#ifdef USE_CLIENT_SENDER
	ARMMClientSender::ARMMClientSender( vrpn_Connection_IP *c )
		:vrpn_Tracker( "ARMM_Client_Server", c )
	{
		m_pass = 0;
	}

	ARMMClientSender::~ARMMClientSender()
	{
	}

	void ARMMClientSender::mainloop()
	{
		vrpn_gettimeofday(&_timestamp, NULL);
		vrpn_Tracker::timestamp = _timestamp;

		d_sensor = m_pass;
		char msgbuf[1000];
		int  len = vrpn_Tracker::encode_to(msgbuf);

		//printf("Pos, sensor %d = %f, %f, %f\n", d_sensor, (float) pos[0],(float) pos[1],(float) pos[2]);

		if (d_connection->pack_message(len, _timestamp, position_m_id, d_sender_id, msgbuf,vrpn_CONNECTION_LOW_LATENCY)) {
			fprintf(stderr,"can't write message: tossing\n");
		}

		//update server main loop
	  server_mainloop();
	}

	void ARMMClientSender::SetPasskey(const int & pass)
	{
		m_pass = pass;
	}

	int ARMMClientSender::GetPasskey( void )const
	{
		return m_pass;
	}

	/////////////////////////////////////////////////////////
	#else
	int m_pass;
	#endif
	///////// End of ARMMClientSender definition ////////////////

	///////// Receiving message from ARMM server //////////
	ARMMClient::ARMMClient (const char * name, vrpn_Connection *cn)
		: vrpn_Tracker_Remote (name, cn)
	{
		register_types();

		if( d_connection != NULL){
			// Register a handler for the acceleration change callback.
			if( register_autodeleted_handler( hand_m_id, handle_hand_change_message, this, d_sender_id) )
			{
				fprintf(stderr, "ARMMClient: can't register hand handler\n");
				d_connection = NULL;
			}

			if( register_autodeleted_handler( softtexture_m_id, handle_softtexture_change_message, this, d_sender_id) )
			{
				fprintf(stderr, "ARMMClient: can't register soft texture handler\n");
				d_connection = NULL;
			}

		}
		else
		{
			fprintf(stderr, "ARMMClient: can't get connection!!\n");
		}

		//init parameter
		REP(i,ConstParams::HAND_SIZE)
		{
			hand_coord_x[0][i] = 0.0;
			hand_coord_y[0][i] = 0.0;
			hand_coord_z[0][i] = 0.0;
		}

		mDataType = REGULAR;
	}

	//virtual
	ARMMClient::~ARMMClient( void )
	{
	}

	void ARMMClient::mainloop( void )
	{
		if( d_connection )
		{
			d_connection->mainloop();
		}
		client_mainloop();
	}


	int ARMMClient::register_change_handler(void *userdata, vrpn_TRACKERHANDCHANGEHANDLER handler, vrpn_int32 whichSensor)
	{
		if (whichSensor < vrpn_ALL_SENSORS) {
			fprintf(stderr, "ARMMClient::register_change_handler: bad sensor index\n");
			return -1;
		}
		// Ensure that the handler is non-NULL
		if (handler == NULL) {
			fprintf(stderr,
			   "ARMMClient::register_change_handler: NULL handler\n");
			return -1;
		}

	  // If this is the ALL_SENSORS value, put it on the all list; otherwise,
	  // put it into the normal list.
	  if (whichSensor == vrpn_ALL_SENSORS)
	  {
			hand_callback_list.register_handler(userdata, handler);
		  //return all_sensor_callbacks.d_handchange.register_handler(userdata, handler);

	  } else if (ensure_enough_sensor_callbacks(whichSensor)) {
			hand_callback_list.register_handler(userdata, handler);
			//return sensor_callbacks[whichSensor].d_handchange.register_handler(userdata, handler);

	  } else {
			fprintf(stderr,"ARMMClient::register_change_handler: Out of memory\n");
			return -1;

	  }
	  return 0;
	}

	int ARMMClient::unregister_change_handler(void *userdata,
			vrpn_TRACKERHANDCHANGEHANDLER handler, vrpn_int32 whichSensor)
	{
	 // put it into the normal list.
	  if (whichSensor == vrpn_ALL_SENSORS) {
			hand_callback_list.unregister_handler(userdata, handler);
		  //return all_sensor_callbacks.d_handchange.register_handler(userdata, handler);
		} else if (ensure_enough_sensor_callbacks(whichSensor)) {
			hand_callback_list.unregister_handler(userdata, handler);
			//return sensor_callbacks[whichSensor].d_handchange.register_handler(userdata, handler);
		} else {
			fprintf(stderr,"ARMMClient::register_change_handler: Out of memory\n");
			return -1;
		}
	  return 0;
	}

	int ARMMClient::register_change_handler(void *userdata, vrpn_TRACKERSOFTTEXTURECHANGEHANDLER handler, vrpn_int32 whichSensor)
	{
		if (whichSensor < vrpn_ALL_SENSORS) {
			fprintf(stderr, "ARMMClient::register_change_handler: bad sensor index\n");
			return -1;
		}
		// Ensure that the handler is non-NULL
		if (handler == NULL) {
			fprintf(stderr,
			   "ARMMClient::register_change_handler: NULL handler\n");
			return -1;
		}

	  // If this is the ALL_SENSORS value, put it on the all list; otherwise,
	  // put it into the normal list.
	  if (whichSensor == vrpn_ALL_SENSORS) {
			st_callback_list.register_handler(userdata, handler);
		  //return all_sensor_callbacks.d_handchange.register_handler(userdata, handler);
		} else if (ensure_enough_sensor_callbacks(whichSensor)) {
			st_callback_list.register_handler(userdata, handler);
			//return sensor_callbacks[whichSensor].d_handchange.register_handler(userdata, handler);
		} else {
			fprintf(stderr,"ARMMClient::register_change_handler: Out of memory\n");
			return -1;

		}
	  return 0;
	}

	int ARMMClient::unregister_change_handler(void *userdata,
			vrpn_TRACKERSOFTTEXTURECHANGEHANDLER handler, vrpn_int32 whichSensor)
	{
	 // put it into the normal list.
	  if (whichSensor == vrpn_ALL_SENSORS) {
			st_callback_list.unregister_handler(userdata, handler);
		  //return all_sensor_callbacks.d_handchange.register_handler(userdata, handler);
		} else if (ensure_enough_sensor_callbacks(whichSensor)) {
			st_callback_list.unregister_handler(userdata, handler);
			//return sensor_callbacks[whichSensor].d_handchange.register_handler(userdata, handler);
		} else {
			fprintf(stderr,"ARMMClient::register_change_handler: Out of memory\n");
			return -1;
		}
	  return 0;
	}

	//Atsushi
	int ARMMClient::register_types(void)
	{
		//vrpn_Tracker_Remote::register_types();

		// to handle hand state changes
		hand_m_id  = d_connection->register_message_type("vrpn_Tracker Hand");
		softtexture_m_id = d_connection->register_message_type("vrpn_Tracker SoftTexture");
		return 0;
	}

	int VRPN_CALLBACK ARMMClient::handle_hand_change_message(
		void *userdata, vrpn_HANDLERPARAM p)
	{

		ARMMClient *me = (ARMMClient *)userdata;
		const char *params = (p.buffer);
		vrpn_TRACKERHANDCB	tp;

		// Fill in the parameters to the tracker from the message

		// this value depends on data size
		// you are supposed to send
		// In this case, you can count on the number of vrpn_float32 variablef
		int data_size = ConstParams::UDP_LIMITATION* 3 *sizeof(vrpn_float32) + sizeof(vrpn_int32);
		//int data_size = sizeof(vrpn_float32) + sizeof(vrpn_int32);
		if (p.payload_len !=  data_size) {
			fprintf(stderr,"ARMMClient: change message payload error\n");
			fprintf(stderr,"             (got %d, expected %d)\n",
				p.payload_len, data_size );
			return -1;

		}

		tp.msg_time = p.msg_time;
		vrpn_unbuffer(&params, &tp.sensor);
		REP(i,ConstParams::UDP_LIMITATION){
			REP(j,3){
				vrpn_unbuffer(&params, &tp.hand[i][j]);
			}
		}
		//vrpn_unbuffer(&params, &tp.hand);

		// Go down the list of callbacks that have been registered.
		// Fill in the parameter and call each.
		//me->all_sensor_callbacks.d_handchange.call_handlers(tp);
		me->hand_callback_list.call_handlers(tp);

	 // // Go down the list of callbacks that have been registered for this
		//// particular sensor
		//if (tp.sensor < 0) {
		//    fprintf(stderr,"ARMMClient:pos sensor index is negative!\n");
		//    return -1;
		//} else if (me->ensure_enough_sensor_callbacks(tp.sensor)) {
		//	me->sensor_callbacks[tp.sensor].d_handchange.call_handlers(tp);
		//} else {
		//    fprintf(stderr,"ARMMClient:pos sensor index too large\n");
		//    return -1;
		//}
		return 0;
	}

	int VRPN_CALLBACK ARMMClient::handle_softtexture_change_message(
		void *userdata, vrpn_HANDLERPARAM p)
	{

		ARMMClient *me = (ARMMClient *)userdata;
		const char *params = (p.buffer);
		vrpn_TRACKERSOFTTEXTURECB	tp;

		// Fill in the parameters to the tracker from the message

		// this value depends on data size
		// you are supposed to send
		// In this case, you can count on the number of vrpn_float32 variablef
		int data_size = (ConstParams::resX*ConstParams::resY)* 3 *sizeof(vrpn_float32) + sizeof(vrpn_int32);
		//int data_size = sizeof(vrpn_float32) + sizeof(vrpn_int32);
		if (p.payload_len !=  data_size) {
			fprintf(stderr,"ARMMClient(SoftTexture): change message payload error\n");
			fprintf(stderr,"             (got %d, expected %d)\n",
				p.payload_len, data_size );
			return -1;

		}

		tp.msg_time = p.msg_time;
		vrpn_unbuffer(&params, &tp.sensor);
		REP(i,ConstParams::resX*ConstParams::resY){
			REP(j,3){
				vrpn_unbuffer(&params, &tp.softT[i][j]);
			}
		}
		//vrpn_unbuffer(&params, &tp.hand);

		// Go down the list of callbacks that have been registered.
		// Fill in the parameter and call each.
		//me->all_sensor_callbacks.d_handchange.call_handlers(tp);
		me->st_callback_list.call_handlers(tp);

	 // // Go down the list of callbacks that have been registered for this
		//// particular sensor
		//if (tp.sensor < 0) {
		//    fprintf(stderr,"ARMMClient:pos sensor index is negative!\n");
		//    return -1;
		//} else if (me->ensure_enough_sensor_callbacks(tp.sensor)) {
		//	me->sensor_callbacks[tp.sensor].d_handchange.call_handlers(tp);
		//} else {
		//    fprintf(stderr,"ARMMClient:pos sensor index too large\n");
		//    return -1;
		//}
		return 0;
	}

	///////// End of ARMM_Client definition ////////////////

	inline CvMat* scaleParams(CvMat *cParams, double scaleFactor) {
		CvMat *sParams = cvCloneMat(cParams);
		sParams->data.db[0]*= scaleFactor;	sParams->data.db[4]*= scaleFactor;
		sParams->data.db[2]*= scaleFactor;	sParams->data.db[5]*= scaleFactor;
		return sParams;
	}

	//-----> Callback function
	void VRPN_CALLBACK handle_pos (void * userData, const vrpn_TRACKERCB t)
	{
		if( t.sensor < ConstParams::CAR_PARAM){
#if CAR_SIMULATION == 1
			if(t.sensor % 5 == 0) {// pos and orientation of cars
				int j = (int) t.sensor / 5;
				CarsArrayPos[j] = osg::Vec3d((float) t.pos[0],(float) t.pos[1],(float) t.pos[2]);
				CarsArrayQuat[j] = osg::Quat((float) t.quat[0], (float) t.quat[1], (float) t.quat[2], (float) t.quat[3]);
			} else { //pos and orientation of car's wheels
				int j = (int) floor((float) t.sensor/5);
				int k = (t.sensor % 5) -1;
				WheelsArrayPos[j][k] = osg::Vec3d((float) t.pos[0],(float) t.pos[1],(float) t.pos[2]);
				WheelsArrayQuat[j][k]= osg::Quat((float) t.quat[0], (float) t.quat[1], (float) t.quat[2], (float) t.quat[3]);
			}
#endif /* CAR_SIMULATION == 1 */
		}

		//----->Keyboard input checker
		else if( t.sensor == ConstParams::CAR_PARAM){
			if ( static_cast<int>(t.pos[0]) != 0){
				m_pass = static_cast<int>(t.pos[0]);
			}
		}

		//----->Collision checker
		else if( t.sensor > ConstParams::CAR_PARAM && t.sensor <= ConstParams::COLLISION_PARAM)
		{
			REP(p,3){
				pCollision[p] = (float)t.pos[p];			// the position of collision
			}
			if( t.quat[0] > 0.5) stroke = true;		// flag whether stroke on or off
			else					stroke = false;
			if( t.quat[1] > 0.5) touch  = true;		// flag whether touch on or off
			else					touch  = false;
			collisionInd = static_cast<int>( t.quat[2]);  //meaning index of a collided object
		}

		//----->Receive objects info
		else{
			int index = t.sensor - (ConstParams::COLLISION_PARAM+1);
			ObjectsArrayPos[index].set(  osg::Vec3d((float)t.pos[0]*OSG_SCALE , (float)t.pos[1]*OSG_SCALE , (float)t.pos[2]*OSG_SCALE));
			osg::Quat quat = osg::Quat((double)t.quat[0]*OSG_SCALE, (double)t.quat[1]*OSG_SCALE, (double)t.quat[2]*OSG_SCALE, (double)t.quat[3]*OSG_SCALE);
			ObjectsArrayQuat[index].set(quat.x(), quat.y(), quat.z(), quat.w());
//			printf("(%f,%f,%f)\n",(float)t.pos[0]*OSG_SCALE , (float)t.pos[1]*OSG_SCALE , (float)t.pos[2]*OSG_SCALE);
		}
	}
	void VRPN_CALLBACK handle_hands (void * userData, const vrpn_TRACKERHANDCB h)
	{
		int count = 0;
		REP(i,ConstParams::HAND_SIZE){
			assert(h.sensor <= ConstParams::UDP_LIMITATION);
			if(count < h.sensor){
				hand_coord_x[0][i] = h.hand[count][0];
				hand_coord_y[0][i] = h.hand[count][1];
				hand_coord_z[0][i] = h.hand[count][2];
				count++;
			}else{
				hand_coord_x[0][i] = 0;
				hand_coord_y[0][i] = 0;
				hand_coord_z[0][i] = 1000;
			}
		}
	}
	void VRPN_CALLBACK handle_softtextures(void * userData, const vrpn_TRACKERSOFTTEXTURECB h)
	{

		REP(count,ConstParams::resX*ConstParams::resY)
		{
			softTexCoord[count].set(
					static_cast<double>(h.softT[count][0]),
					static_cast<double>(h.softT[count][1]),
					static_cast<double>(h.softT[count][2])
			);
		}
	}
	//<----- Callback function


	//////////////////// Entry point ////////////////////
	ARMM::ARMM( ::Capture *camera)
#ifdef POINTGREYCAMERA
		: mCapture(static_cast<PointgreyCamera *>(camera))
#else
	   : mCapture = new CvCamera(0, CAPTURE_SIZE, CAMERA_PARAMS_FILENAME);

#endif
	{
		Init();

		collisionInd  = -1;
		collisionInd2 = -1;
		transfer = 0;
	}

	ARMM::~ARMM()
	{
		cvReleaseMat(&RegistrationParams);

		if(mCapture) delete mCapture;
		delete mKC;
	}

//TODO acquiring the mesh data of environment from server program
// to display proper occlusion rendering
	void ARMM::Run()
	{

//		TickCountAverageBegin();
		mARMMClient->mainloop();
//		TickCountAverageEnd();
		IplImage *arImage = mCapture->getFrame();
		RenderScene(arImage, mCapture);
		cvReleaseImage(&arImage);
		//		//change the condition of regular state if lists of models is shown in AR space into invisible condition
		//		if(mOsgRender->IsModelButtonVisibiilty())
		//		{
		//			mOsgRender->ToggleModelButtonVisibility();
		//		}

#ifdef USE_CLIENT_SENDER
		ARMM_client_sender->SetPasskey( mKC->check_input() );
		if( ARMM_client_sender->GetPasskey()  != 0){
			ARMM_client_sender->mainloop();
		}
#else
		if( m_pass != 0)
		{
			cout << m_pass << endl;
			mKC->set_input(m_pass, mOsgRender);
			m_pass = 0;
		}

		DecideCollisionCondition();

		const int objIndex = mOsgRender->getOsgObject()->getObjectIndex();

		if( touch && stroke )
		{
			collidedNodeInd = (mOsgRender->getOsgObject()->SizeObjNodeArray()-1) - (objIndex  - collisionInd);
			GetCollisionCoordinate(collidedNodeInd);
		}
		if(transfer == 1)
		{
			transfer = 2; //this value means texture have been already transferred

//			cout << "Last Collided obj index = " << collisionInd	 << endl;
//			cout << "All of Object index = " << objectIndex  << endl;
			collidedNodeInd = (mOsgRender->getOsgObject()->SizeObjTransformArray()-1) - (objIndex  - collisionInd);
			output << "d " << endl; //as delimitar
			GetCollisionCoordinate(collidedNodeInd);
			pDstModel = mOsgRender->getOsgObject()->getObjNodeArray().at(collidedNodeInd)->getName();

			//Running "Texture Transfer exe"
			std::ofstream shellFile("TT.sh");
			shellFile << "#!/bin/sh" << endl;
			shellFile << "cd ~/TextureTransfer/TextureTransfer" << endl;
			shellFile << "./TT Model3DS/debug_remote.txt " << pSrcModel.c_str() << " " << pDstModel.c_str();
			shellFile.close();
			stringstream str("sh TT.sh ");
			cout << "Execute shell command!!" << endl;
			if(system(str.str().c_str()) < 0)
			{
				cerr << "Occurring command error!" << endl;
			}

			//swap the collided object
			mKC->set_input(100, mOsgRender);
		}

#endif
	}

/*	void ARMM::Run( ImageType imRGB)
	{

		IplImage *arImage = capture->getFrame();
		armm_client->mainloop();
		RenderScene(arImage, capture);
		cvReleaseImage(&arImage);
#ifdef USE_CLIENT_SENDER
		ARMM_client_sender->SetPasskey( kc->check_input() );
		if( ARMM_client_sender->GetPasskey()  != 0){
			ARMM_client_sender->mainloop();
		}
#else
		if( m_pass != 0){
			kc->set_input(m_pass);
			m_pass = 0;
		}
		#endif
	}*/


	void ARMM::Init( void )
	{
		// initialize values of virtual objects
#if CAR_SIMULATION == 1
		for(int i =0; i < ConstParams::NUM_CARS; i++) {
			CarsArrayPos[i] = osg::Vec3d(0,0,0);
			CarsArrayQuat[i] = osg::Quat(0,0,0,1);
			for(int j =0; j < ConstParams::NUM_WHEELS; j++)
			{
				WheelsArrayPos[i][j] = osg::Vec3d(0,0,0);
				WheelsArrayQuat[i][j] = osg::Quat(0,0,0,1);
			}
		}
#endif
		mOsgRender = boost::shared_ptr<osg_Client>(new osg_Client());

		RegistrationParams = scaleParams(mCapture->getParameters(), double(REGISTRATION_SIZE.width)/double(CAPTURE_SIZE.width));
		mOsgRender->osg_init(calcProjection(RegistrationParams, mCapture->getDistortion(), REGISTRATION_SIZE));
		mOsgRender->osg_inittracker(ConstParams::MARKER_FILENAME, 400, 400);
#if USE_OSGMENU==1
		mOsgRender->OsgInitMenu();
		mOsgRender->OsgInitModelButton();
#endif
	//	m_Connection = new vrpn_Connection_IP();
	//	ARMM_Client = new vrpn_Tracker_Remote ("ARMM_Comm", m_Connection);

		//----->Client part
//		mARMMClient = new ARMMClient (ARMM_SERVER_IP);
		mARMMClient = boost::shared_ptr<ARMMClient>( new ARMMClient(ARMM_SERVER_IP));

		mARMMClient->vrpn_Tracker_Remote::register_change_handler(NULL, handle_pos);
		mARMMClient->ARMMClient::register_change_handler(NULL, handle_hands);
		mARMMClient->ARMMClient::register_change_handler(NULL, handle_softtextures);
		//<-----

		mKC = new KeyboardController_client;

		#ifdef USE_CLIENT_SENDER
		//----->Client_sender part
		m_Connection = new vrpn_Connection_IP();
		ARMM_client_sender = new ARMMClientSender(m_Connection);
		cout << "Created VRPN server." << endl;
		//<-----
		#endif
		m_pass = 0;
	}

	void ARMM::RenderScene(IplImage *arImage, Capture *capture)
	{

//	if(Virtual_Objects_Count > 0)
//	{
//		osg::Geode * geode = model3ds->asGeode();
//				uint numGeodes = geode->getNumDrawables();
//
////		cout << "num of geodes = " << numGeodes << endl;
//
//		REP(geodeIdx, numGeodes)
//		{
//			osg::Geometry *tmpGeo = geode->getDrawable( geodeIdx )->asGeometry();
//			if( tmpGeo ){
//				cout<< tmpGeo->getNumTexCoordArrays() << endl;
//			}
//	//		if( tmpGeo ){
//	//			osg::Vec2Array * texArrays = dynamic_cast< osg::Vec2Array *>( tmpGeo->getNumTexCoordArrays());
//	//		}
//		}

	//	osg::ref_ptr<osg::Vec2Array> texcoords = tmp Geo->getTexCoordArray(0);
	//	cout << "Texture size = " << texcoords->size() << endl;
//	}
		if(mOsgRender->getOsgObject()->getVirtualObjectsCount() > 0) {

			DeleteLostObject();
			std::vector <osg::Quat> quat_obj_array;
			std::vector <osg::Vec3d> vect_obj_array;
			for(int i = 0; i < mOsgRender->getOsgObject()->getVirtualObjectsCount(); i++) {
				quat_obj_array.push_back(ObjectsArrayQuat[i]);
				vect_obj_array.push_back(ObjectsArrayPos[i]);
			}
			mOsgRender->osg_UpdateHand(0, hand_coord_x[0], hand_coord_y[0], hand_coord_z[0]);
#if CAR_SIMULATION == 1
			mOsgRender->osg_client_render(arImage, CarsArrayQuat, CarsArrayPos, WheelsArrayQuat, WheelsArrayPos, RegistrationParams, capture->getDistortion(), quat_obj_array, vect_obj_array);
#else
			(arImage, NULL, NULL, NULL, NULL, RegistrationParams, mCapture->getDistortion(), quat_obj_array, vect_obj_array);
#endif
		} else {
			mOsgRender->osg_UpdateHand(0, hand_coord_x[0], hand_coord_y[0], hand_coord_z[0]);
#if CAR_SIMULATION == 1
			mOsgRender->osg_client_render(arImage, CarsArrayQuat, CarsArrayPos, WheelsArrayQuat, WheelsArrayPos, RegistrationParams, capture->getDistortion());
#else
			osg_client_render(arImage, NULL, NULL, NULL, NULL, RegistrationParams, mCapture->getDistortion());
#endif
		}/* Virtual_Objects_Count > 0 */
	}


	void ARMM::DeleteLostObject(void)
	{

		vector<int> obj_ind;//to remove a lost object
		obj_ind.clear();
		for(int i = 0; i < mOsgRender->getOsgObject()->getVirtualObjectsCount(); i++)
		{
			//an object dropped under threshold
			if( ObjectsArrayPos[i].z() < -100 )
			{
				obj_ind.push_back(i);
				//writeback coord
				if( mOsgRender->getOsgObject()->getVirtualObjectsCount() == 1){}
				else{
					if( (i+1) == mOsgRender->getOsgObject()->getVirtualObjectsCount() ){}
					else{
						ObjectsArrayPos[i] = ObjectsArrayPos[i+1];
					}
				}
			}
		}
		if(!obj_ind.empty())
		{
			vector<osg::PositionAttitudeTransform*> pTransArray =
					mOsgRender->getOsgObject()->getObjTransformArray();
			vector<osg::Node*> pNodesArray =
					mOsgRender->getOsgObject()->getObjNodeArray();

			cout << "Lost Number = " << obj_ind.size() << "->";
			for(int i= obj_ind.size()-1; i>=0; i--)
			{
				int index = obj_ind[i];
				cout << index << " ";
				vector<osg::PositionAttitudeTransform*>::iterator it = pTransArray.begin() + index;
				vector<osg::Node*>::iterator it2 = pNodesArray.begin() + index;
				mOsgRender->getShadowedScene()->removeChild(pTransArray.at(index));
				pTransArray.erase(it);
				pNodesArray.erase(it2);
				mOsgRender->getOsgObject()->DecrementObjCount();
				cout << "Virtual objects LOST : Remain " << mOsgRender->getOsgObject()->getVirtualObjectsCount() << endl;

				mOsgRender->getOsgObject()->DecrementObjIndex();
			}
			obj_ind.clear();
		}
	}

	void ARMM::GetCollisionCoordinate(const int & index)
	{
//			osg::Matrix	*modelMatrix = new osg::Matrix;
//
//			modelMatrix->setTrans(trans);
//			modelMatrix->setRotate(rotate);
//			osg::Matrix	modelInverseMatrix = modelMatrix->inverse(*modelMatrix);

//			osg::Vec4d		posCollision;
//			REP(i,3) posCollision[i] = pCollision[i];
//			posCollision[3] = 1;

//			osg::Vec4d		posCollisionLocal = (modelInverseMatrix) * posCollision;

//			osg::Vec3d modified_pos,;
		osg::Vec3d pos;
		pos.set(pCollision[0], pCollision[1], pCollision[2]);
//			modified_pos.set(ObjectsArrayPos[0]/10);
		output << "v " << pos << endl;
//			cout << "PosCollision in World coordinate -> " << pos << endl;
//			cout << "Pos in World coordinate(OSG) -> " << modified_pos << endl;
//			cout << "Modified PosCollision without Rotate and Scaling -> " << pos- modified_pos << endl;
//			cout << "PosCollision in Local coordinate -> " << posCollisionLocal << endl;
	}

	void ARMM::DecideCollisionCondition()
	{
		if( touch ) //touch = true
		{
			if(stroke)
			{
//				cout << " Collision with " << collisionInd << " in "
//	          << pCollision[0] << ","
//	          << pCollision[1] << ","
//	          << pCollision[2] << ","
//	          << endl;
				collisionInd2 = collisionInd; //collisionInd2 indicates the collided object in previous frame
			}
			else
			{
				if( collisionInd == collisionInd2)
				{
					if( touch && !collision)
					{

						collision = true;

						cout << "Collided obj  index = " << collisionInd	 << endl;
						cout << "All of Object index = " << mOsgRender->getOsgObject()->getObjectIndex() << endl;
						int ind = (mOsgRender->getOsgObject()->SizeObjTransformArray()-1) -
								(mOsgRender->getOsgObject()->getObjectIndex()- collisionInd);

						//restoring the name of collided model
						pSrcModel = mOsgRender->getOsgObject()->getObjNodeArray().at(ind)->getName();
						cout << "Source model name = " << pSrcModel.c_str() << endl;

						//make the updater for soft texture ON
						mOsgRender->getOsgObject()->setSoftTexture(true);

						// set a created hand to the graphics tree
						mKC->set_input(101, mOsgRender);

						m_pass = 0;
						collidedNodeInd = ind;
					}
					//					cout << " Collision with " << collisionInd << " with parts" << endl;
				}
				else
				{
//					cout << " Collision with " << collisionInd << " and finished transfer from " << collisionInd2 << endl;
					if(transfer == 0 ) transfer = 1;
				}
	        }
	      return;
	    }
	}

}/*  Endf of Namespace ARMM */

