#include "ShapeHelper.h"

NxActor* CreateBox(NxScene* scene, NxVec3 dimensions, NxVec3 loc, const NxReal den)
{
	// Actor, body and shape descriptors
	NxActorDesc actorDesc;
	NxBodyDesc bodyDesc;
	NxBoxShapeDesc boxDesc;

	// The actor has one shape, a box, 1m on a side
	boxDesc.dimensions.set(dimensions);	//requires half dimension values
	actorDesc.shapes.pushBack(&boxDesc);

	actorDesc.body			= &bodyDesc;
	actorDesc.density		= den; // kg/m^3
	actorDesc.globalPose.t	= loc;	

	return scene->createActor(actorDesc);	
}

NxActor* CreateCylinder(NxPhysicsSDK* physx, NxScene* scene, NxVec3 loc, float rad, float height, const int sides)
{
	NxActorDesc actorDesc;
	NxBodyDesc bodyDesc;

	NxVec3 *verts = new NxVec3[sides*2];

	for (int i=0; i < sides*2; i++)
	{
		float degInRad = NxMath::degToRad((float)i*(360/sides));
		verts[i] = NxVec3(cos(degInRad)*rad,0,sin(degInRad)*rad);

		if (i>sides-1)
			verts[i].y=height;
	}

	// Create descriptor for convex mesh`
	NxConvexMeshDesc* convexDesc	= new NxConvexMeshDesc();
	convexDesc->numVertices			= sides*2;
	convexDesc->pointStrideBytes	= sizeof(NxVec3);
	convexDesc->points				= verts;
	convexDesc->flags				= NX_CF_COMPUTE_CONVEX;

	NxConvexShapeDesc convexShapeDesc;
	convexShapeDesc.localPose.t		= loc;
	convexShapeDesc.userData		= convexDesc;

	NxInitCooking();
	// Cooking from memory
	MemoryWriteBuffer buf;
	bool status = NxCookConvexMesh(*convexDesc, buf);
	//
	// Please note about the created Convex Mesh, user needs to release it when no one uses it to save memory. It can be detected
	// by API "meshData->getReferenceCount() == 0". And, the release API is "gPhysicsSDK->releaseConvexMesh(*convexShapeDesc.meshData);"
	//
	NxConvexMesh *pMesh			= physx->createConvexMesh(MemoryReadBuffer(buf.data));
	convexShapeDesc.meshData	= pMesh;
	NxCloseCooking();

	if (pMesh)
	{
		// Save mesh in userData for drawing.
		pMesh->saveToDesc(*convexDesc);

		NxActorDesc actorDesc;
		actorDesc.shapes.pushBack(&convexShapeDesc);
		actorDesc.body		= &bodyDesc;
		actorDesc.density	= 0.2f;
		return scene->createActor(actorDesc);
	}

	return 0;
}

NxActor* ConvexMesh(NxPhysicsSDK* physx, NxScene* scene, NxVec3* verts, NxVec3 loc, int size)
{
	NxActorDesc actorDesc;
	NxBodyDesc bodyDesc;

	// Create descriptor for convex mesh
	NxConvexMeshDesc* convexDesc	= new NxConvexMeshDesc();
	convexDesc->numVertices			= size;
	convexDesc->pointStrideBytes	= sizeof(NxVec3);
	convexDesc->points				= verts;
	convexDesc->flags				= NX_CF_COMPUTE_CONVEX;
	NxConvexShapeDesc convexShapeDesc;

	convexShapeDesc.localPose.t		= loc;
	convexShapeDesc.userData		= convexDesc;

	NxInitCooking();
	// Cooking from memory
	MemoryWriteBuffer buf;
	bool status = NxCookConvexMesh(*convexDesc, buf);
	//
	// Please note about the created Convex Mesh, user needs to release it when no one uses it to save memory. It can be detected
	// by API "meshData->getReferenceCount() == 0". And, the release API is "gPhysicsSDK->releaseConvexMesh(*convexShapeDesc.meshData);"
	//
	NxConvexMesh *pMesh			= physx->createConvexMesh(MemoryReadBuffer(buf.data));
	convexShapeDesc.meshData	= pMesh;
	NxCloseCooking();

	if (pMesh)
	{
		// Save mesh in userData for drawing.
		pMesh->saveToDesc(*convexDesc);

		NxActorDesc actorDesc;
		
		actorDesc.shapes.pushBack(&convexShapeDesc);
		actorDesc.body		= &bodyDesc;
		actorDesc.density	= 1.0f;
		return scene->createActor(actorDesc);
	}

	return 0;
}

NxActor* CreateSphere(NxScene* scene, const NxReal rad, NxVec3 loc, const NxReal den)
{
	// Actor, body and shape descriptors
	NxActorDesc			ballActDesc;
	NxBodyDesc			ballBodyDesc;
	NxSphereShapeDesc	ballShapeDesc;

	//ballCapsuleShapeDesc.radius = 0.0127f;
	ballShapeDesc.radius = rad;
	
	ballActDesc.shapes.pushBack(&ballShapeDesc);
	ballActDesc.body			= &ballBodyDesc;

	ballActDesc.density			= den;
	//weight: 80g
	//diameter 1in
	//volume: 0.524in^3
	//ballActDesc.globalPose.t	= NxVec3(17, 5, 0);
	ballActDesc.globalPose.t	= loc;

	return scene->createActor(ballActDesc);
}

NxActor* CreateCapsule(NxScene* scene, const NxReal rad, const NxReal height, NxVec3 loc, const NxReal den)
{
	// Actor, body and shape descriptors
	NxActorDesc			ActorDesc;
	NxBodyDesc			BodyDesc;
	NxCapsuleShapeDesc	ShapeDesc;

	ShapeDesc.height = height;
	ShapeDesc.radius = rad;
	ActorDesc.globalPose.M.rotZ(1.57);
	ActorDesc.shapes.pushBack(&ShapeDesc);
	ActorDesc.body			= &BodyDesc;
	ActorDesc.density = 1;
	ActorDesc.globalPose.t = NxVec3(loc);
	return scene->createActor(ActorDesc);
}

NxMaterial* CreateMaterial(NxScene* scene, float sFric, float dFric, float rest)
{
	NxMaterialDesc materialDesc;

	materialDesc.staticFriction = sFric;
	materialDesc.dynamicFriction = dFric;
	materialDesc.restitution = rest;

	return (NxMaterial*)scene->createMaterial(materialDesc);
}