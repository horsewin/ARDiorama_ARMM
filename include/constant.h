#ifndef CONSTANT_H
#define CONSTANT_H

#define CAR_SIMULATION 1
#define SIM_MICROMACHINE 1
#define USE_OSGMENU 1

#define REP(i,n) for(int i=0;i<(int)n;++i)

namespace ARMM
{
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

	class ConstParams
	{
	public:
		//for image processing
		#define CAPTURE_SIZE cvSize(640,480)
		#define REGISTRATION_SIZE cvSize(320,240)
		#define MESH_SIZE cvSize(160,120)
		#define OPFLOW_SIZE cvSize(SKIN_X,SKIN_X)
		#define SKIN_SEGM_SIZE cvSize(SKIN_X,SKIN_Y)

		static const int		SKIN_X;
		static const int		SKIN_Y;
		; // 11 pixels
		enum{MIN_HAND_PIX = 21};
		enum{HAND_GRID_SIZE = MIN_HAND_PIX*MIN_HAND_PIX};
		enum{HAND_SIZE = HAND_GRID_SIZE};

		static const int HAND_MAX_TRANSMIT_SIZE;

		//for VRPN connection
		enum{UDP_LIMITATION = 100};

		//const float MIN_HAND_PIX = 15; // 11 pixels
		//const int HAND_GRID_SIZE = 225;// 15x15

		//for file loading
		static const char *	DATABASEDIR;
		static const char *	MENUDATADIR;
		static const char *  MARKER_FILENAME;
		static const char *  CAMERA_PGR_PARAMS_FILENAME;
		static const char *  CAMERA_PARAMS_FILENAME;
		static const char *  KINECT_PARAMS_FILENAME;
		static const char *  KINECT_TRANSFORM_FILENAME;
		static const char *  KINECT_CONFIG_FILENAME;
		static const char *  CAR1_BODY_FILENAME;
		static const char *  CAR1_WHEEL_FILENAME;
		static const char *  CAR2_BODY_FILENAME;
		static const char *  CAR2_WHEEL_FILENAME;

#if CAR_SIMULATION == 1
		enum{NUM_CARS = 2};
		enum{NUM_WHEELS = 4};
#else
		enum{NUM_CARS = 0};
		enum{NUM_WHEELS = 0};
#endif

		enum{NUM_OBJECTS = 10};
		static const int CAR_PARAM;
		static const int COLLISION_PARAM;

		//window size
		static const int WINDOW_WIDTH  = 1280;
		static const int WINDOW_HEIGHT = 960;

		//for virtual hands
		enum{MAX_NUM_HANDS=1};

		//for virtual objects
		static const int MAX_NUM_VIR_OBJ;

		//for soft texture
		enum{resX=12, resY=9};
	};
}
#endif
