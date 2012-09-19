/*
 * osg_Object.h
 *
 *  Created on: 2012/09/16
 *      Author: umakatsu
 */

#ifndef OSG_OBJECT_H_
#define OSG_OBJECT_H_

#include <osg/Geode>
#include <osg/Geometry>
#include <osg/PolygonMode>
#include <osg/MatrixTransform>
#include <osg/PositionAttitudeTransform>

#include <vector>

namespace ARMM
{
	class osg_Object
	{
	public:
		osg_Object();
		~osg_Object();

		void SetObjTransformArrayIndex( const int & idx, osg::Quat quat, osg::Vec3d vec);
		void PushObjNodeArray(osg::Node* n){ obj_node_array.push_back(n); }
		osg::ref_ptr<osg::Node> GetObjNodeIndex(const int & idx) const;
		std::vector<osg::Node*> getObjNodeArray() const {
			return obj_node_array;
		}

		int SizeObjNodeArray(void) const ;
		int SizeObjTransformArray(void) const;

	int getVirtualObjectsCount() const {
		return Virtual_Objects_Count;
	}

	int getObjectIndex() const {
		return objectIndex;
	}
		void setObjNodeArray(std::vector<osg::Node*> objNodeArray) {
			obj_node_array = objNodeArray;
		}

		std::vector<osg::PositionAttitudeTransform*> getObjTransformArray() const {
			return obj_transform_array;
		}

		void setObjTransformArray(
				std::vector<osg::PositionAttitudeTransform*> objTransformArray) {
			obj_transform_array = objTransformArray;
		}

		osg::ref_ptr<osg::PositionAttitudeTransform> getObjTexturePosAtt() const {
			return mObjTexturePosAtt;
		}

		void setObjTexturePosAtt(
				osg::ref_ptr<osg::PositionAttitudeTransform> objTexture) {
			mObjTexturePosAtt = objTexture;
		}

		osg::ref_ptr<osg::Node> getObjTexture() const {
			return objTexture;
		}

		void setObjTexture(osg::ref_ptr<osg::Node> objTexture) {
			this->objTexture = objTexture;
		}

		bool isSoftTexture() const {
			return softTexture;
		}

		void setSoftTexture(bool softTexture) {
			this->softTexture = softTexture;
		}

		void IncrementObjCount(void){ Virtual_Objects_Count++; }
		void DecrementObjCount(void){ Virtual_Objects_Count--; }
		void IncrementObjIndex(void){ objectIndex++; }
		void IncrementObjIndex(const int & val){ objectIndex+=val; }
		void DecrementObjIndex(void){ objectIndex--; }

	private:
		std::vector<osg::Node*> obj_node_array;
		std::vector<osg::PositionAttitudeTransform*> obj_transform_array;

		//for virtual objects
		int Virtual_Objects_Count;

		int objectIndex;

		//for soft texture
		osg::ref_ptr<osg::Node> objTexture;
		osg::ref_ptr<osg::PositionAttitudeTransform> mObjTexturePosAtt;
		bool softTexture;
	};
}

#endif /* OSG_OBJECT_H_ */
