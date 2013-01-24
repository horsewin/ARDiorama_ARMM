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
	const char * ConstParams::MENUDATADIR = "/home/umakatsu/Dropbox/Lab/MenuDatabase/";
	//for file loading
	const char * ConstParams::MARKER_FILENAME = "ARMM/Data/Celica.bmp";
	//const char *  MARKER_FILENAME "Data/thai_art.jpg"
	const char *  ConstParams::CAMERA_PGR_PARAMS_FILENAME = "/home/umakatsu/Dropbox/Lab/M2/proc/Cameras/camera_pointgrey.yml";
	const char *  ConstParams::CAMERA_PARAMS_FILENAME = "/home/umakatsu/Dropbox/Lab/M2/proc/Cameras/camera.yml";
	const char *  ConstParams::KINECT_PARAMS_FILENAME = "/home/umakatsu/Dropbox/Lab/M2/proc/Cameras/kinect.yml";
	const char *  ConstParams::KINECT_TRANSFORM_FILENAME = "/home/umakatsu/Dropbox/Lab/M2/proc/Cameras/KinectTransform.yml";
	const char *  ConstParams::KINECT_CONFIG_FILENAME = "/home/umakatsu/Dropbox/Lab/M2/proc/Cameras/KinectConfig.xml";

	const int ConstParams::COLLISION_PARAM	= 1;

	//window size
	const int WINDOW_WIDTH  = 1280;
	const int WINDOW_HEIGHT = 960;

	//for virtual objects
	const int MAX_NUM_VIR_OBJ = 5;

	//for AR menu
	const int ConstParams::ADDMODELBUTTON		= 201;
	const int ConstParams::RESETBUTTON			= 202;
	const int ConstParams::STARTTRANSBUTTON	= 203;
	const int ConstParams::CANCELMODELBUTTON	= 301;
}
