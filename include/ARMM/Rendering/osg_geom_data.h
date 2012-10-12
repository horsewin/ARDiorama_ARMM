#ifndef OSGGEOM_H
#define OSGGEOM_H

//OSG include
#include <osg/ShapeDrawable>
#include <osg/Geometry>
#include <osg/Material>
#include <osg/StateSet>
#include <osg/LightModel>
#include <osg/PositionAttitudeTransform>
#include <osgUtil/SmoothingVisitor>
#include <osg/TextureRectangle>
#include <osg/TexMat>

//osgWorks include
#include <osgwTools/Shapes.h>

//Standard API
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>

#include "Model/Model3ds.h"
#include "constant.h"

namespace ARMM
{
	extern osg::Vec3d softTexCoord[ConstParams::resX*ConstParams::resY];

	struct MeshUpdater : public osg::Drawable::UpdateCallback
	{
		MeshUpdater( const unsigned int size )
		  : _size( size )
		{}

		virtual ~MeshUpdater()
		{}

		virtual void update( osg::NodeVisitor*, osg::Drawable* draw )
		{
			osg::Geometry* geom( draw->asGeometry() );
			osg::Vec3Array* verts( dynamic_cast< osg::Vec3Array* >( geom->getVertexArray() ) );

			// Update the vertex array from the soft body node array.
			osg::Vec3Array::iterator it( verts->begin() );
			unsigned int idx = 0;

			for( idx=0; idx<_size; idx++)
			{
				*it++ = softTexCoord[idx];
			}

			verts->dirty();
			draw->dirtyBound();

			// Generate new normals.
			osgUtil::SmoothingVisitor smooth;
			smooth.smooth( *geom );
			geom->getNormalArray()->dirty();
		}

		const unsigned int _size;
	};

	osg::Node* osgSphereNode(float sphere_size)
	{
		float scale = 10;

		osg::ref_ptr<osg::Sphere> sphere = new osg::Sphere(osg::Vec3d(0,0,0), sphere_size*scale);
		osg::ref_ptr<osg::ShapeDrawable > shape = new osg::ShapeDrawable( sphere );
		shape->setColor(osg::Vec4f(1.0, .0, .0, 1.0));
		osg::ref_ptr< osg::Geode > geode = new osg::Geode();
		geode->addDrawable( shape.get() );
		return geode.release();
	}

	osg::Node* osgBoxNode(float box_size)
	{
		float scale = 10;

		osg::ref_ptr<osg::Box> box= new osg::Box(osg::Vec3d(0,0,0), box_size*scale);
		osg::ref_ptr<osg::ShapeDrawable> shape = new osg::ShapeDrawable( box.get() );
		shape->setColor(osg::Vec4f(1.0, 1.0, 1.0, 1.0));
		osg::ref_ptr<osg::Geode> geode = new osg::Geode;
		geode->addDrawable( shape.get() );
		return geode.release();
	}

	osg::Node* osgTexture(const char * file)
	{
		//open 3ds file
		Lib3dsFile * model = lib3ds_file_open(file);
		if (model == NULL)
		{
			std::cerr << "Failed to load model " << file << std::endl;
			return NULL;
		}
		else
		{
			std::cout << "Load file : " << file << std::endl;
		}

		std::vector<PTAMM::Materials> materials;//材質データ構造
		PTAMM::Materials MatDefault;//デフォルト用
		std::vector<int>::iterator Iter;
		std::vector<int>::iterator EndIter;
		Lib3dsMaterial *material;
		Lib3dsMesh *mesh;

		//材質データ構造のメモリ確保
		materials.resize(model->nmaterials);
		for (int loop = 0; loop < model->nmaterials; ++loop) {
			material = model->materials[loop];//loop番目のマテリアル
			//各材質データ内のメンバでメモリ確保
			materials[loop].meshIdx.resize(model->nmeshes);
			for (int n = 0; n < model->nmeshes; ++n) {
				mesh = model->meshes[n];//n番目のメッシュ
				materials[loop].meshIdx[n].faceId.reserve(mesh->nfaces
						/ model->nmaterials);//適当にメモリ確保
			}
		}

		//材質データ構造に対応するメッシュデータIDを格納する
		static int MatId = 0;
		for (int k = 0; k < model->nmeshes; ++k) {
			mesh = model->meshes[k];//k番目のメッシュ
			for (int i = 0; i < mesh->nfaces; ++i) {
				MatId = mesh->faces[i].material;
				if (MatId == -1) {
	//				MatDefault.meshIdx[k].faceId.push_back(i);
				} else {
					materials[MatId].meshIdx[k].faceId.push_back(i);
				}
			}
		}

		//shrink - to - fitで確保メモリの調整
		for (int loop = 0; loop < model->nmaterials; ++loop) {
			for (int j = 0; j < model->nmeshes; ++j) {
				std::vector<int>(materials[loop].meshIdx[j].faceId).swap(
						materials[loop].meshIdx[j].faceId);
			}

		}

		// Loop through every mesh
		long int id;

		//declare array variable corresponding to models
		osg::ref_ptr<osg::Vec2Array> vertices	= new osg::Vec2Array;
		osg::ref_ptr<osg::DrawElementsUInt> faceArray = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
		osg::ref_ptr<osg::Vec2Array> texcoords	= new osg::Vec2Array;
		id = 0;

		// Acquire a texture name
		std::string tex_file = "ARMM/Data/Model1/LSCM_NewTorus1.bmp";
		osg::ref_ptr<osg::Texture2D> texture(new osg::Texture2D);

		// Activate texture
		if (!tex_file.empty()) {
			//loading texture image object
			osg::ref_ptr<osg::Image> image (osgDB::readImageFile(tex_file.c_str()));
			if(!image->valid()){
				std::cerr << "Error in osgDB::readImageFile -> " << tex_file.c_str() << std::endl;
			}else{
				texture->setImage(image.get());
			}
			texture->setUnRefImageDataAfterApply(false);
		}

		for(int mm = 0; mm<model->nmeshes; mm++)
		{
			// get mesh information from the Lib3dsFile object
			Lib3dsMesh * mesh = model->meshes[mm];

			// Begin drawing with our selected mode
			EndIter = materials[0].meshIdx[mm].faceId.end();
			for (Iter = materials[0].meshIdx[mm].faceId.begin(); Iter!= EndIter; Iter++)
			{
				// Go through each corner of the triangle and draw it.
				for(int vv=0; vv<3; vv++)
				{
					// Get the index for each point of the face
					int index = mesh->faces[(*Iter)].index[vv];
					vertices->push_back( osg::Vec2(mesh->texcos[index][0], 1 - mesh->texcos[index][1]));
					faceArray->push_back(id++);
					texcoords->push_back( osg::Vec2(mesh->texcos[index][0], 1 - mesh->texcos[index][1])); // caution to texture coordinate
				}
			}
		}

		//set each attribute
		osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;
		geometry->setVertexArray(vertices.get());
		geometry->addPrimitiveSet(faceArray.get());
		osgUtil::SmoothingVisitor::smooth (*geometry);
		geometry->setTexCoordArray(0, texcoords.get());

		osg::ref_ptr<osg::Geode> geode= new osg::Geode;

		// Assign texture unit 0 of our new StateSet to the texture
		// we just created and enable the texture.
		osg::ref_ptr<osg::StateSet> state_geode = new osg::StateSet;
		texture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::NEAREST);
		state_geode->setTextureAttributeAndModes(0, texture.get(), osg::StateAttribute::ON);
		state_geode->setMode(GL_LIGHTING, GL_FALSE);
		state_geode->setMode(GL_DEPTH_TEST, GL_FALSE); // if do not use this, there is something wrong for texture mapping
		geode->setStateSet(state_geode.get());

		// add it to a parent node
		geode->addDrawable(geometry.get());

		return geode.release();
	}

	osg::Node* osgCreateSoft(std::string texturename)
	{
		const short resX(12), resY(9);
		osg::ref_ptr< osg::Geode > geode( new osg::Geode );
		const osg::Vec3 llCorner( 0.0, .0, 0.0 );
		const osg::Vec3 uVec( 100.0, 0.0, 0.0 );
		const osg::Vec3 vVec( 0.0, 100.0, 0.0 ); // Must be at a slight angle for wind to catch it.
		osg::Geometry* geom = osgwTools::makePlane( llCorner,
			uVec, vVec, osg::Vec2s( resX-1, resY-1 ) );
		geode->addDrawable( geom );

		// Set up for dynamic data buffer objects
		geom->setDataVariance( osg::Object::DYNAMIC );
		geom->setUseDisplayList( false );
		geom->setUseVertexBufferObjects( true );
		geom->getOrCreateVertexBufferObject()->setUsage( GL_DYNAMIC_DRAW );

		// Flag state: 2-sided lighting and a texture map.
		{
			osg::StateSet* stateSet( geom->getOrCreateStateSet() );

			osg::LightModel* lm( new osg::LightModel() );
			lm->setTwoSided( true );
			stateSet->setAttributeAndModes( lm );

			const std::string texName( texturename.c_str() );
			osg::Texture2D* tex( new osg::Texture2D(
				osgDB::readImageFile( texName ) ) );
			if( ( tex == NULL ) || ( tex->getImage() == NULL ) )
				osg::notify( osg::WARN ) << "Unable to read texture: \"" << texName << "\"." << std::endl;
			else
			{
				tex->setResizeNonPowerOfTwoHint( false );
				stateSet->setTextureAttributeAndModes( 0, tex );
			}
		}

		geom->setUpdateCallback( new MeshUpdater( ConstParams::resX * ConstParams::resY));

		return (geode.release());
	}

	osg::Node* osg3DSFileFromDiorama( const char* file, const char * dir = NULL)
	{
		std::ostringstream modelfile;
		if(dir)
		{
			modelfile << dir << file;
		}
		else
		{
			std::cerr << "Error: No directory is specified" << std::endl;
			return NULL;
		}

		//open 3ds file
		Lib3dsFile * model = lib3ds_file_open(modelfile.str().c_str());
		if (model == NULL)
		{
			std::cerr << "Failed to load model " << file << std::endl;
			return NULL;
		}
		else
		{
			std::cout << "Load file : " << file << std::endl;
		}

		std::vector<PTAMM::Materials> materials;//材質データ構造
		PTAMM::Materials MatDefault;//デフォルト用
		std::vector<int>::iterator Iter;
		std::vector<int>::iterator EndIter;
		Lib3dsMaterial *material;
		Lib3dsMesh *mesh;

		//材質データ構造のメモリ確保
		materials.resize(model->nmaterials);
		for (int loop = 0; loop < model->nmaterials; ++loop) {
			material = model->materials[loop];//loop番目のマテリアル
			//各材質データ内のメンバでメモリ確保
			materials[loop].meshIdx.resize(model->nmeshes);
			for (int n = 0; n < model->nmeshes; ++n) {
				mesh = model->meshes[n];//n番目のメッシュ
				materials[loop].meshIdx[n].faceId.reserve(mesh->nfaces
						/ model->nmaterials);//適当にメモリ確保
			}
		}

		//材質データ構造に対応するメッシュデータIDを格納する
		static int MatId = 0;
		for (int k = 0; k < model->nmeshes; ++k) {
			mesh = model->meshes[k];//k番目のメッシュ
			for (int i = 0; i < mesh->nfaces; ++i) {
				MatId = mesh->faces[i].material;
				if (MatId == -1) {
	//				MatDefault.meshIdx[k].faceId.push_back(i);
				} else {
					materials[MatId].meshIdx[k].faceId.push_back(i);
				}
			}
	//		std::vector<int>(MatDefault.meshIdx[k].faceId).swap(MatDefault.meshIdx[k].faceId);
		}

		//shrink - to - fitで確保メモリの調整
		for (int loop = 0; loop < model->nmaterials; ++loop) {
			for (int j = 0; j < model->nmeshes; ++j) {
				std::vector<int>(materials[loop].meshIdx[j].faceId).swap(
						materials[loop].meshIdx[j].faceId);
			}

		}

	//	//some defaults
	//	static GLfloat ambiant[4] = { 0.2f, 0.2f, 0.2f, 1.0f };
	//	static GLfloat diffuse[4] = { 0.8f, 0.8f, 0.8f, 1.0f };
	//	static GLfloat spectal[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

		// Loop through every mesh
		osg::ref_ptr<osg::Group> body = new osg::Group;
		long int id;
		std::cout << model->nmaterials << std::endl;
		for(int nm=0; nm < model->nmaterials; nm++)
		{
			//declare array variable corresponding to models
			osg::ref_ptr<osg::Vec3Array> vertices	= new osg::Vec3Array;
			osg::ref_ptr<osg::DrawElementsUInt> faceArray = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
	//		osg::ref_ptr<osg::Vec3Array> normals	= new osg::Vec3Array;
			osg::ref_ptr<osg::Vec2Array> texcoords	= new osg::Vec2Array;
			id = 0;

			// Acquire a texture name
			std::string tex_file;
			if(dir)
			{
				tex_file = dir;
			}
			else
			{
				tex_file = ConstParams::DATABASEDIR;
			}
			tex_file += model->materials[nm]->texture1_map.name;
	//		osg::ref_ptr<osg::TextureRectangle> texture = 0;
			osg::ref_ptr<osg::Texture2D> texture(new osg::Texture2D);
			std::cout << "Texture - " << tex_file.c_str() << std::endl;
			// Activate texture
			if (!tex_file.empty())
			{
				//loading texture image object

				osg::ref_ptr<osg::Image> image = 			osgDB::readImageFile(tex_file.c_str());
				if(image){}
				else
				{
					std::cerr << " NULL pointer found" << std::endl;
					continue;
				}

				if(!image->valid())
				{
					std::cerr << "Error in osgDB::readImageFile -> " << tex_file.c_str() << std::endl;
				}
				else
				{
					texture->setImage(image.get());
				}
				texture->setUnRefImageDataAfterApply(false);
			}
			else
			{
				std::cout << "Default " << nm << std::endl;
			}

			for(int mm = 0; mm<model->nmeshes; mm++)
			{
				// get mesh information from the Lib3dsFile object
				Lib3dsMesh * mesh = model->meshes[mm];

				// Begin drawing with our selected mode
				EndIter = materials[nm].meshIdx[mm].faceId.end();
				for (Iter = materials[nm].meshIdx[mm].faceId.begin(); Iter!= EndIter; Iter++)
				{
					// Go through each corner of the triangle and draw it.
					for(int vv=0; vv<3; vv++)
					{
						// Get the index for each point of the face
						int index = mesh->faces[(*Iter)].index[vv];
	//					cout << mesh->vertices[index][0] << "," << mesh->vertices[index][1] << "," << mesh->vertices[index][2] << endl;

						//Regular version
						vertices->push_back( osg::Vec3(mesh->vertices[index][0], mesh->vertices[index][1], mesh->vertices[index][2]));

						//for Yasuhara's cube
	//					double scale = 0.1;
	//					vertices->push_back(osg::Vec3(mesh->vertices[index][0]/scale-0.193, mesh->vertices[index][1]/scale+0.815, mesh->vertices[index][2]/scale-0.113));

						//this one for Umakatsu's reconstruction
	//					texcoords->push_back( osg::Vec2(mesh->texcos[index][0], 1-mesh->texcos[index][1])); // caution to texture coordinate


	//					//this one for Yasuhara's reconstruction
						texcoords->push_back( osg::Vec2(mesh->texcos[index][0]/640, 1-mesh->texcos[index][1]/480)); // caution to texture coordinate
					}
					std::vector<int > tmpFace;
					for(int i=0; i<3; i++) tmpFace.push_back(id++);
					faceArray->push_back(tmpFace[0]);
					faceArray->push_back(tmpFace[2]);
					faceArray->push_back(tmpFace[1]);
				}
			}

			//set each attribute
			osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;
			geometry->setVertexArray(vertices.get());
			geometry->addPrimitiveSet(faceArray.get());
			geometry->setTexCoordArray(0, texcoords.get());

			osg::ref_ptr<osg::Geode> geode= new osg::Geode;

			// Assign texture unit 0 of our new StateSet to the texture
			// we just created and enable the texture.
			osg::ref_ptr<osg::StateSet> state_geode = new osg::StateSet;
			state_geode->setTextureAttributeAndModes(0, texture.get(), osg::StateAttribute::ON);
			state_geode->setMode(GL_LIGHTING, GL_FALSE);
			state_geode->setMode(GL_DEPTH_TEST, GL_FALSE); // if do not use this, there is something wrong for texture mapping
			geode->setStateSet(state_geode.get());

			// add it to a parent node
			geode->addDrawable(geometry.get());

			body->addChild(geode.get());
		}
		return body.release();
	}

	//TODO 論文執筆したら除去。モデルの重心を計算してサイズ調整をする関数をosg3DSFIle...に追加すること
	osg::Node* osg3DSFileFromDiorama2( const char* file, const char * dir = NULL)
	{
		std::ostringstream modelfile;
		if(dir)
		{
			modelfile << dir << file;
		}
		else
		{
			std::cerr << "Error: No directory is specified" << std::endl;
			return NULL;
		}

		//open 3ds file
		Lib3dsFile * model = lib3ds_file_open(modelfile.str().c_str());
		if (model == NULL)
		{
			std::cerr << "Failed to load model " << file << std::endl;
			return NULL;
		}
		else
		{
			std::cout << "Load file : " << file << std::endl;
		}

		std::vector<PTAMM::Materials> materials;//材質データ構造
		PTAMM::Materials MatDefault;//デフォルト用
		std::vector<int>::iterator Iter;
		std::vector<int>::iterator EndIter;
		Lib3dsMaterial *material;
		Lib3dsMesh *mesh;

		//材質データ構造のメモリ確保
		materials.resize(model->nmaterials);
		for (int loop = 0; loop < model->nmaterials; ++loop) {
			material = model->materials[loop];//loop番目のマテリアル
			//各材質データ内のメンバでメモリ確保
			materials[loop].meshIdx.resize(model->nmeshes);
			for (int n = 0; n < model->nmeshes; ++n) {
				mesh = model->meshes[n];//n番目のメッシュ
				materials[loop].meshIdx[n].faceId.reserve(mesh->nfaces
						/ model->nmaterials);//適当にメモリ確保
			}
		}

	//	//材質デフォルトのメモリ確保
	//	MatDefault.meshIdx.resize(model->nmeshes);
	//	for (int loop1 = 0; loop1 < model->nmeshes; ++loop1) {
	//		mesh = model->meshes[loop1];
	//		if (model->nmaterials != 0){
	//			MatDefault.meshIdx[loop1].faceId.reserve(mesh->nfaces / model->nmaterials);
	//		}
	//	}

		//材質データ構造に対応するメッシュデータIDを格納する
		static int MatId = 0;
		for (int k = 0; k < model->nmeshes; ++k) {
			mesh = model->meshes[k];//k番目のメッシュ
			for (int i = 0; i < mesh->nfaces; ++i) {
				MatId = mesh->faces[i].material;
				if (MatId == -1) {
	//				MatDefault.meshIdx[k].faceId.push_back(i);
				} else {
					materials[MatId].meshIdx[k].faceId.push_back(i);
				}
			}
	//		std::vector<int>(MatDefault.meshIdx[k].faceId).swap(MatDefault.meshIdx[k].faceId);
		}

		//shrink - to - fitで確保メモリの調整
		for (int loop = 0; loop < model->nmaterials; ++loop) {
			for (int j = 0; j < model->nmeshes; ++j) {
				std::vector<int>(materials[loop].meshIdx[j].faceId).swap(
						materials[loop].meshIdx[j].faceId);
			}

		}

	//	//some defaults
	//	static GLfloat ambiant[4] = { 0.2f, 0.2f, 0.2f, 1.0f };
	//	static GLfloat diffuse[4] = { 0.8f, 0.8f, 0.8f, 1.0f };
	//	static GLfloat spectal[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

		// Loop through every mesh
		osg::ref_ptr<osg::Group> body = new osg::Group;
		long int id;
		std::cout << model->nmaterials << std::endl;
		for(int nm=0; nm < model->nmaterials; nm++)
		{
			//declare array variable corresponding to models
			osg::ref_ptr<osg::Vec3Array> vertices	= new osg::Vec3Array;
			osg::ref_ptr<osg::DrawElementsUInt> faceArray = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
	//		osg::ref_ptr<osg::Vec3Array> normals	= new osg::Vec3Array;
			osg::ref_ptr<osg::Vec2Array> texcoords	= new osg::Vec2Array;
			id = 0;

			// Acquire a texture name
			std::string tex_file;
			if(dir)
			{
				tex_file = dir;
			}
			else
			{
				tex_file = ConstParams::DATABASEDIR;
			}
			tex_file += model->materials[nm]->texture1_map.name;
	//		osg::ref_ptr<osg::TextureRectangle> texture = 0;
			osg::ref_ptr<osg::Texture2D> texture(new osg::Texture2D);
			std::cout << "Texture - " << tex_file.c_str() << std::endl;
			// Activate texture
			if (!tex_file.empty())
			{
				//loading texture image object

				osg::ref_ptr<osg::Image> image = 			osgDB::readImageFile(tex_file.c_str());
				if(image){}
				else
				{
					std::cerr << " NULL pointer found" << std::endl;
					continue;
				}

				if(!image->valid())
				{
					std::cerr << "Error in osgDB::readImageFile -> " << tex_file.c_str() << std::endl;
				}
				else
				{
					texture->setImage(image.get());
				}
				texture->setUnRefImageDataAfterApply(false);
			}
			else
			{
				std::cout << "Default " << nm << std::endl;
			}

			for(int mm = 0; mm<model->nmeshes; mm++)
			{
				// get mesh information from the Lib3dsFile object
				Lib3dsMesh * mesh = model->meshes[mm];

				// Begin drawing with our selected mode
				EndIter = materials[nm].meshIdx[mm].faceId.end();
				for (Iter = materials[nm].meshIdx[mm].faceId.begin(); Iter!= EndIter; Iter++)
				{
					// Go through each corner of the triangle and draw it.
					for(int vv=0; vv<3; vv++)
					{
						// Get the index for each point of the face
						int index = mesh->faces[(*Iter)].index[vv];
	//					cout << mesh->vertices[index][0] << "," << mesh->vertices[index][1] << "," << mesh->vertices[index][2] << endl;

						//Regular version
	//					vertices->push_back( osg::Vec3(mesh->vertices[index][0], mesh->vertices[index][1], mesh->vertices[index][2]));

						//for Yasuhara's cube
						double scale = 0.1;
						vertices->push_back(osg::Vec3(mesh->vertices[index][0]/scale-0.193, mesh->vertices[index][1]/scale+8.15, mesh->vertices[index][2]/scale-0.113));

						//this one for Umakatsu's reconstruction
	//					texcoords->push_back( osg::Vec2(mesh->texcos[index][0], 1-mesh->texcos[index][1])); // caution to texture coordinate


	//					//this one for Yasuhara's reconstruction
						texcoords->push_back( osg::Vec2(mesh->texcos[index][0]/640, 1-mesh->texcos[index][1]/480)); // caution to texture coordinate
					}
					std::vector<int > tmpFace;
					for(int i=0; i<3; i++) tmpFace.push_back(id++);
					faceArray->push_back(tmpFace[0]);
					faceArray->push_back(tmpFace[2]);
					faceArray->push_back(tmpFace[1]);
				}
			}

			//set each attribute
			osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;
			geometry->setVertexArray(vertices.get());
			geometry->addPrimitiveSet(faceArray.get());
			geometry->setTexCoordArray(0, texcoords.get());

			osg::ref_ptr<osg::Geode> geode= new osg::Geode;

			// Assign texture unit 0 of our new StateSet to the texture
			// we just created and enable the texture.
			osg::ref_ptr<osg::StateSet> state_geode = new osg::StateSet;
			state_geode->setTextureAttributeAndModes(0, texture.get(), osg::StateAttribute::ON);
			state_geode->setMode(GL_LIGHTING, GL_FALSE);
			state_geode->setMode(GL_DEPTH_TEST, GL_FALSE); // if do not use this, there is something wrong for texture mapping
			geode->setStateSet(state_geode.get());

			// add it to aosg::Node* osg3DSFileFromDiorama( const char* file, const char * dir = NULL)
			geode->addDrawable(geometry.get());

			body->addChild(geode.get());
		}
		return body.release();
	}
}
#endif
