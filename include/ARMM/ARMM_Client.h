#ifndef ARMM_CLIENT_H
#define ARMM_CLIENT_H

#define POINTGREYCAMERA 1

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "constant.h"

#include <stdio.h>
//#include <tchar.h>

#include <math.h>

#include "vrpn_Tracker.h"
#include "vrpn_Connection.h"
#include "CaptureLibrary.h"

#include "ImageType.h"
//---------------------------------------------------------------------------
// Constant/Define
//---------------------------------------------------------------------------
//#define USE_CLIENT_SENDER

#if CAR_SIMULATION == 1
	const int NUM_CARS = 2;
	const int NUM_WHEELS = 4;
#else
	const int NUM_CARS = 0;
	const int NUM_WHEELS = 0;
#endif

const int NUM_OBJECTS = 10;
const int CAR_PARAM 			= NUM_CARS*NUM_WHEELS+NUM_CARS;
const int COLLISION_PARAM	= CAR_PARAM + 1;
const short resX(12);
const short resY(9);
extern bool running;

#ifndef VRPN_ARCH
typedef  char            vrpn_int8;
typedef  unsigned char   vrpn_uint8;
typedef  short           vrpn_int16;
typedef  unsigned short  vrpn_uint16;
typedef  int             vrpn_int32;
typedef  unsigned int	 vrpn_uint32;
typedef  float           vrpn_float32;
typedef  double          vrpn_float64;
#endif

//---------------------------------------------------------------------------
// Struct
//---------------------------------------------------------------------------
typedef	struct _vrpn_TRACKERHANDCB {
	struct timeval	msg_time;	// Time of the report
	vrpn_int32	sensor;		// Which sensor is reporting
	vrpn_float32 hand[UDP_LIMITATION][3];
	//vrpn_float32 hand;
} vrpn_TRACKERHANDCB;
typedef void (VRPN_CALLBACK *vrpn_TRACKERHANDCHANGEHANDLER)(void *userdata,
					     const vrpn_TRACKERHANDCB info);

typedef	struct _vrpn_TRACKERSOFTTEXTURECB {
	struct timeval	msg_time;	// Time of the report
	vrpn_int32	sensor;		// Which sensor is reporting
	vrpn_float32 softT[resX*resY][3];
} vrpn_TRACKERSOFTTEXTURECB;
typedef void (VRPN_CALLBACK *vrpn_TRACKERSOFTTEXTURECHANGEHANDLER)(void *userdata,
					     const vrpn_TRACKERSOFTTEXTURECB info);

class KeyboardController_client;

namespace ARMM
{
	enum datatype{REGULAR, SOFTBODY};
	class PointgreyCamera;

	//---------------------------------------------------------------------------
	// Class definition
	//---------------------------------------------------------------------------
	class ARMMClient : public vrpn_Tracker_Remote
	{
	public:
		ARMMClient(const char * name, vrpn_Connection *cn = NULL);
		virtual ~ARMMClient(void);
		virtual void mainloop();

		virtual int register_change_handler(void *userdata,
			  vrpn_TRACKERHANDCHANGEHANDLER handler, vrpn_int32 whichSensor  = vrpn_ALL_SENSORS);

		virtual int unregister_change_handler(void *userdata,
			  vrpn_TRACKERHANDCHANGEHANDLER handler, vrpn_int32 whichSensor  = vrpn_ALL_SENSORS);

		virtual int register_change_handler(void *userdata,
			  vrpn_TRACKERSOFTTEXTURECHANGEHANDLER handler, vrpn_int32 whichSensor  = vrpn_ALL_SENSORS);

		virtual int unregister_change_handler(void *userdata,
			  vrpn_TRACKERSOFTTEXTURECHANGEHANDLER handler, vrpn_int32 whichSensor  = vrpn_ALL_SENSORS);

	protected:
		vrpn_Callback_List<vrpn_TRACKERHANDCB> hand_callback_list;
		vrpn_Callback_List<vrpn_TRACKERSOFTTEXTURECB> st_callback_list;
	  //ARMMSensorCallbacks   all_sensor_callbacks;
	  //ARMMSensorCallbacks   *sensor_callbacks;

		//Atsushi
		vrpn_int32 hand_m_id;	// ID of tracker hand message
		vrpn_int32 softtexture_m_id;	// ID of tracker hand message

	protected:
		virtual int register_types(void);
		static int VRPN_CALLBACK handle_hand_change_message(void *userdata,
			vrpn_HANDLERPARAM p);
		static int VRPN_CALLBACK handle_softtexture_change_message(void *userdata,
			vrpn_HANDLERPARAM p);

	private:
		datatype mDataType;

	};

	class ARMM{
	public:
		ARMM();
		ARMM(::Capture * camera);
		~ARMM();
		void Run();
		void Run( ImageType imRGB );

	private:
		void Init( void );
		void RenderScene(IplImage *arImage, ::Capture *capture);
		void RenderScene(IplImage *arImage, CvMat * distortion);
		void DeleteLostObject( void );
		void GetCollisionCoordinate(const int & index);
		void DecideCollisionCondition();

	private:
		ARMMClient * armm_client;
		::KeyboardController_client *kc;
		#ifdef POINTGREYCAMERA
		PointgreyCamera *capture;
		#else
		Capture *capture;
		#endif

	};

	#ifdef USE_CLIENT_SENDER
	/////////////////////// ARMM_COMMUNICATOR /////////////////////////////

	class ARMMClientSender : public vrpn_Tracker
	{
	public:
		ARMMClientSender( vrpn_Connection_IP *c = 0);
		virtual ~ARMMClientSender();
		virtual void mainloop();
		void SetPasskey(const int & pass);
		int  GetPasskey( void )const;
	protected:
		struct timeval _timestamp;
		int m_pass;
	};
	#endif // ** USE_CLIENT_SENDER **
}
#endif
