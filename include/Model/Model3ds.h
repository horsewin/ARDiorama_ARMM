// Copyright 2009 Isis Innovation Limited
//
// C++ Interface: Model3ds
//
// Description: A 3ds model class
//
//
// Author: Robert Castle <bob@robots.ox.ac.uk>, (C) 2009
//
#ifndef MODEL3DS_H
#define MODEL3DS_H

#include "Reconstruction/Texture.h"
#include "OpenGL.h"

#include <lib3ds.h>
#include <deque>
#include <TooN/TooN.h>
#include <boost/shared_ptr.hpp>

//for texture unwrapping
#include "TextureTransfer/LSCM.h"

namespace PTAMM {

	using namespace TooN;

	class TiXmlElement;

	typedef float Vector3D[3];  //for use with lib3ds

	//メッシュごとの対応インデックス
	struct MeshIndex
	{
			std::vector<int> faceId;
	};

	//材質データ格納用
	struct Materials
	{
			float   ambient[4];
			float   diffuse[4];
			float   specular[4];
			float   shininess;
			std::vector<MeshIndex> meshIdx;
	};

	/**
	 * A 3DS model class
	 * @author Robert Castle <bob@robots.ox.ac.uk>
	*/
	class Model3DS{

	  public:
		Model3DS();
		~Model3DS();

		bool Load( std::string sModelDir, std::string sFileName, TooN::Vector<3> v3Rotation );
		void Save( TiXmlElement * modelsNode );
		bool Load( TiXmlElement * modelsNode );

		void IntegrationTextures(Lib3dsFile * pModel, std::string msModelDir);
		void Draw();

		//Manipulation function
		void ScaleIncrease();
		void ScaleDecrease();
		void MoveForwards();
		void MoveBack();
		void MoveLeft();
		void MoveRight();
		void MoveUp();
		void MoveDown();
		void PitchIncrease();
		void PitchDecrease();
		void RollIncrease();
		void RollDecrease();
		void YawIncrease();
		void YawDecrease();
		void MoveTo( Vector<3> v3Position );

		const SE3<> & Pose() { return mse3MfromW; }
		const double Scale() { return mdScale; }
		void Reset();

		std::string & Name() { return msName; }
		double Diameter() { return 3*mdScale*mdDiameter; }

	  protected:
		bool _Load();
		void DrawTextureMonitor(int x, int y, int w, int h);
		GLuint _GenerateDisplayList( Lib3dsFile * pModel, bool bWireframe );
		void _DrawAxes();
		void _FindBoundingBox( Lib3dsFile * pModel );
		void _DrawBoundingBox();

		void _DelayedLoad();
		ImageType _LoadTextures( Lib3dsFile * pModel );

		void Load3DSModel(Lib3dsFile * pModel, boost::shared_ptr<TextureTransfer::LSCM> pLSCM, std::vector< boost::shared_ptr<TextureTransfer::IndexedMesh> > & pMesh); //added by Atsushi 2012.9.10
		void ConvertDataStructure(boost::shared_ptr<TextureTransfer::LSCM> pLSCM, std::vector< boost::shared_ptr<TextureTransfer::IndexedMesh> > pMesh);

		// edited by Umakatsu 2011.6.7
		std::deque< Texture * >  _LoadTextures( Lib3dsFile * pModel , std::string dirpath );
		GLuint _GenerateDisplayList( Lib3dsFile * pModel, bool bWireframe , std::deque< Texture * > texturesList);

	  protected:
		GLuint mnDisplayList[2];        // The OpenGL display lists for the model
		bool mbLoaded;                  // Has the model been loaded

		std::string msName;             // Name of the model
		double mdScaleMult;             // the scale multiplier for the model
		double mdScale;                 // the model's scale;
		SE3<> mse3MfromW;               // Model's pose and location in the world.
		SE3<> mse3ModelOffset;          // Initial rotation of model to line up with the axes

		Vector<3> mv3DimensionMin;      // The minimum and maximum values for the model's
		Vector<3> mv3DimensionMax;      // boundary. Used to form the bounding box
		Vector<3> mv3CentreOfMass;      // The model's centre or mass. Model rotates about this
		Vector<3> mv3Offset;            // This displacement make the centre of the model's base the origin.
		double mdDiameter;                // This is used to set the radius of the selected target

		std::string msModelDir;         // The directory containing the model file and its textures
		std::string msModelFile;        // The model file name

		bool mbDelayedLoad;             // Is a delayed load pending?
		bool mIntegrate;
	};
}
#endif
