/*
 * osg_Client.h
 *
 *  Created on: 2011/05/xx
 *  	 Author: Adrian Clark and Thammathip Piumsomboon from HIT Lab NZ
 *
 *  This header file is edited by Atsushi Umakatsu, Osaka university master student in 2012.
 *
 */
#ifndef OSG_CLIENT_H
#define OSG_CLIENT_H

//-------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osg/Texture2D>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/PolygonMode>
#include <osgDB/ReadFile>
#include <osg/ShapeDrawable>
#include <osgShadow/ShadowedScene>
#include <osg/MatrixTransform>
#include <osg/PositionAttitudeTransform>
#include <osg/io_utils>
#include <osg/Depth>
#include <osgUtil/Optimizer>

//Image processing library
#include <opencv/highgui.h>

//Standard API
#include <cstring>

#include <boost/shared_ptr.hpp>

//Original objects
#include "ARMM/Rendering/osg_Object.h"

namespace
{
	inline void LoadCheck(void * ptr, const char * filename)
	{
		if( ptr == NULL){
			std::cerr << "Error : No such file in the directory or incorrect path.  " << filename << std::endl;
		}
	}
}

namespace ARMM
{
	//---------------------------------------------------------------------------
	// Constant/Define
	//---------------------------------------------------------------------------

	class osg_Object;
	class osg_Client
	{
		public:
			osg_Client(){};
			~osg_Client(){};

			void osg_init(double *projMatrix);
			void osg_inittracker(std::string markerName, int maxLengthSize, int maxLengthScale);
			void osg_client_render(IplImage *newFrame, osg::Quat *q,osg::Vec3d  *v, osg::Quat wq[][4], osg::Vec3d wv[][4], CvMat *cParams, CvMat *cDistort);
			void osg_client_render(IplImage *newFrame, osg::Quat *q,osg::Vec3d  *v, osg::Quat wq[][4], osg::Vec3d wv[][4], CvMat *cParams, CvMat *cDistort, std::vector <osg::Quat> q_array,std::vector <osg::Vec3d>  v_array);
			void osg_uninit();
			void osg_UpdateHeightfieldTrimesh(float *ground_grid);
			void osgAddObjectNode(osg::Node* n);
			void osgAddObjectNode(osg::Node* n, const double & scale);
			void osgAddObjectNode(osg::Node* n, const double & scale, const osg::Quat & quat);
			//void osg_setHFNode(osg::Node* n);
			void osg_createHand(int index, float world_scale, float ratio);
			void osg_UpdateHand(int index, float *x, float *y, float *grid);
			void osg_UpdateSoftTexture(void);

			boost::shared_ptr<osg_Object> getOsgObject() const {
				return mOsgObject;
			}

			void setOsgObject(boost::shared_ptr<osg_Object> osgObject) {
				mOsgObject = osgObject;
			}

			osg::ref_ptr<osgShadow::ShadowedScene> getShadowedScene() const {
				return shadowedScene;
			}

		public:
			enum{resX=12, resY=9};

		private:
			//Shadowing Stuff
			osg::ref_ptr<osgShadow::ShadowedScene> shadowedScene;
			uint rcvShadowMask;
			uint castShadowMask;

			boost::shared_ptr<osg_Object>mOsgObject;
	};
}
#endif
