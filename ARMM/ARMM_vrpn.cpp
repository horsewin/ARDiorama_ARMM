#ifndef ARMM_VRPN_CPP
#define ARMM_VRPN_CPP

#include "ARMM/ARMM_vrpn.h"

#include <cstring>
#include <iostream>
#include <cstdio>
#include <cmath>

#include "constant.h"

namespace ARMM
{
	using namespace std;
	ARMM_Communicator::ARMM_Communicator( vrpn_Connection *c) : vrpn_Tracker( "ARMM_Comm", c )
	{
		HeightfieldData = new float[19200];
		CarsArrayPos = new osg::Vec3d[ConstParams::NUM_CARS];
		CarsArrayQuat = new osg::Quat[ConstParams::NUM_CARS];
		WheelsArrayPos = new osg::Vec3d*[ConstParams::NUM_CARS];
		WheelsArrayQuat = new osg::Quat*[ConstParams::NUM_CARS];

		for( int i = 0 ; i < ConstParams::NUM_CARS ; i++ ) {
			WheelsArrayPos[i] = new osg::Vec3d[ConstParams::NUM_WHEELS];
			WheelsArrayQuat[i] = new osg::Quat[ConstParams::NUM_WHEELS];
		}
	}

	ARMM_Communicator::~ARMM_Communicator() {

		for( int i = 0 ; i < ConstParams::NUM_CARS ; i++ ) {
			delete [] WheelsArrayPos[i];
			delete [] WheelsArrayQuat[i];
		}
		delete [] WheelsArrayPos;
		delete [] WheelsArrayQuat;
		delete [] CarsArrayPos;
		delete [] CarsArrayQuat;
		delete [] HeightfieldData;
	}

	void ARMM_Communicator::VRPN_Update_Client_Cars(osg::Quat *q,osg::Vec3d  *v, osg::Quat wq[][4], osg::Vec3d wv[][4], float *ground_grid) {
		memcpy(CarsArrayPos, v, sizeof(CarsArrayPos));
		memcpy(CarsArrayQuat, q, sizeof(CarsArrayQuat));
		memcpy(WheelsArrayPos, wv, sizeof(WheelsArrayPos));
		memcpy(WheelsArrayQuat, wq, sizeof(WheelsArrayQuat));
		memcpy(HeightfieldData, ground_grid, sizeof(HeightfieldData));
	}

	void ARMM_Communicator::mainloop() {
		vrpn_gettimeofday(&_timestamp, NULL);

		vrpn_Tracker::timestamp = _timestamp;

		char msgbuf[1000];

		d_sensor = 0;

		int  len = vrpn_Tracker::encode_to(msgbuf);

		if (d_connection->pack_message(len, _timestamp, position_m_id, d_sender_id, msgbuf,
			vrpn_CONNECTION_LOW_LATENCY))
		{
			fprintf(stderr,"can't write message: tossing\n");
		}



		server_mainloop();
	}

}
#endif

