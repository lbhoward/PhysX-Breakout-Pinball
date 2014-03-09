#pragma once

#include "NxPhysics.h"
#include "NxCooking.h"
#include "Extras/Stream.h"

NxActor* CreateBox(NxScene* scene, NxVec3 dimensions, NxVec3 loc, const NxReal den);
NxActor* CreateCylinder(NxPhysicsSDK* physx, NxScene* scene, NxVec3 loc, float rad, float height, const int sides);
NxActor* ConvexMesh(NxPhysicsSDK* physx, NxScene* scene, NxVec3* verts, NxVec3 loc, int size);
NxActor* CreateSphere(NxScene* scene, const NxReal rad, NxVec3 loc, const NxReal den);
NxActor* CreateCapsule(NxScene* scene, const NxReal rad, const NxReal height, NxVec3 loc, const NxReal den);

NxMaterial* CreateMaterial(NxScene* scene, float sFric, float dFric, float rest);