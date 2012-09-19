/*
 * constant.cpp
 *
 *  Created on: 2012/09/19
 *      Author: umakatsu
 */

#include "constant.h"

namespace ARMM
{
	const int		ConstParams::SKIN_X = 320;
	const int		ConstParams::SKIN_Y = 240;

	const int ConstParams::HAND_MAX_TRANSMIT_SIZE = 120;

	//const float MIN_HAND_PIX = 15; // 11 pixels
	//const int HAND_GRID_SIZE = 225;// 15x15

	const char * ConstParams::DATABASEDIR = "/home/umakatsu/Dropbox/Lab/ModelDatabase/";
	//for file loading
	const char * ConstParams::MARKER_FILENAME = "ARMM/Data/Celica.bmp";
	//const char *  MARKER_FILENAME "Data/thai_art.jpg"
	const char *  ConstParams::CAMERA_PGR_PARAMS_FILENAME = "ARMM/Data/Cameras/camera_pointgrey.yml";
	const char *  ConstParams::CAMERA_PARAMS_FILENAME = "ARMM/Data/Cameras/camera.yml";
	const char *  ConstParams::KINECT_PARAMS_FILENAME = "ARMM/Data/Cameras/kinect.yml";
	const char *  ConstParams::KINECT_TRANSFORM_FILENAME = "ARMM/Data/Cameras/KinectTransform.yml";
	const char *  ConstParams::KINECT_CONFIG_FILENAME = "ARMM/Data/Cameras/KinectConfig.xml";

	const char *  ConstParams::CAR1_BODY_FILENAME = "ARMM/Data/Cars/GT4_body.ive";
	const char *  ConstParams::CAR1_WHEEL_FILENAME = "ARMM/Data/Cars/GT4_tire.ive";
	const char *  ConstParams::CAR2_BODY_FILENAME = "ARMM/Data/Cars/Murcielago_body.ive";
	const char *  ConstParams::CAR2_WHEEL_FILENAME = "ARMM/Data/Cars/Murcielago_tire.ive";

	const int ConstParams::CAR_PARAM 			= ConstParams::NUM_CARS*ConstParams::NUM_WHEELS+ConstParams::NUM_CARS;
	const int ConstParams::COLLISION_PARAM	= ConstParams::CAR_PARAM + 1;

	//window size
	const int WINDOW_WIDTH  = 1280;
	const int WINDOW_HEIGHT = 960;

	//for virtual objects
	const int MAX_NUM_VIR_OBJ = 5;

}
