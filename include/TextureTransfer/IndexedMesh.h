#ifndef _INDEXEDMESH_H_
#define _INDEXEDMESH_H_

//-------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------
#include <vector>

#include "BasicFace.h"
#include <string>

//-------------------------------------------------------------------
// Class definition
//-------------------------------------------------------------------
namespace TextureTransfer
{

	class IndexedMesh
	{
	 public:
	  IndexedMesh();
	  ~IndexedMesh();

	  Vertex* AddVertex();
	  Vertex* AddVertex(const Vector3& p, const Vector2& t);

	  void BeginFacet();
	  void EndFacet();
	  void AddVertex2Facet(unsigned int i);
	  void Clear();

	  void Save(const std::string & file_name);

	  void FindTextureMax();

	 public:
	  std::vector<Vertex> mVertices ;
	  std::vector<Facet>  mFaces ;
	  bool mInFacet ;
	  Vector2 mTexMax, mTexMin;

	  std::vector<Vector2> mTextureCoords;
	  std::vector<double> mTexParts;			//for reserving harmonic field value. This vector size is the same as mVertices vector size
	  std::vector<int> mTextureFaces;

	  std::vector<int> mTextureNumber; 		//to indicate corresponding texture image number. This vector size is the same as mVertices vector size
	  unsigned int mNumIndex;
	};

}
#endif
