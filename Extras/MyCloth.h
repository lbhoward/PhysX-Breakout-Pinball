// ===============================================================================
//						  NVIDIA PHYSX SDK TRAINING PROGRAMS
//					                CLOTH OBJECT
//
//                     Written by Matthias Müller-Fischer, 3-8-05
//						     Edited by Bob Schade, 5-1-06
//						 Edited by Lawrence Howard, 27-1-2013
// ===============================================================================

#ifndef MYCLOTH_H
#define MYCLOTH_H

#include "NxPhysics.h"
#include "GL/glut.h"

class MyCloth {
public:
	MyCloth();
	/* create regular mesh */
	MyCloth(NxScene *scene, NxClothDesc &desc, NxReal w, NxReal h, NxReal d, char *texFilename = NULL, bool tearLines = false);

	~MyCloth();

	void draw(bool shadows);
	NxCloth *getNxCloth() { return mCloth; }

private:
	void init(NxScene *scene, NxClothDesc &desc, NxClothMeshDesc &meshDesc);
	void generateRegularMeshDesc(NxClothMeshDesc &desc, NxReal w, NxReal h, NxReal d, bool texCoords, bool tearLines = false);
	void generateTearLines(NxClothMeshDesc& desc, NxU32 w, NxU32 h);
	void releaseMeshDescBuffers(const NxClothMeshDesc& desc);

	bool cookMesh(NxClothMeshDesc& desc);

	void allocateReceiveBuffers(int numVertices, int numTriangles);
	void releaseReceiveBuffers();

	bool mInitDone;

	NxMeshData mReceiveBuffers;
	NxScene *mScene;
	NxCloth *mCloth;
	NxClothMesh *mClothMesh;

	GLfloat *mTexCoords;
	NxU32   mNumTexCoords;
	GLuint  mTexId;
};

#endif  // CLOTH_H
