#ifndef CONSTANT_H
#define CONSTANT_H

#define CAR_SIMULATION 1
#define SIM_MICROMACHINE 1

#define REP(i,n) for(int i=0;i<(int)n;++i)

const int		SKIN_X = 320;
const int		SKIN_Y = 240;
const float MIN_HAND_PIX = 21; // 11 pixels
const int		HAND_GRID_SIZE = 441;// 15x15
const int		HAND_SIZE = HAND_GRID_SIZE;

const int HAND_MAX_TRANSMIT_SIZE = 120;

//for image processing
#define CAPTURE_SIZE cvSize(640,480)
#define REGISTRATION_SIZE cvSize(320,240)
#define MESH_SIZE cvSize(160,120)
#define OPFLOW_SIZE cvSize(SKIN_X,SKIN_X)
#define SKIN_SEGM_SIZE cvSize(SKIN_X,SKIN_Y)

//for VRPN connection
const int UDP_LIMITATION = 100;

//const float MIN_HAND_PIX = 15; // 11 pixels
//const int HAND_GRID_SIZE = 225;// 15x15

#endif
