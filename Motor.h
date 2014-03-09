#pragma once

#include "NxPhysics.h";
#include "ShapeHelper.h";
#include "Extras\UserData.h";

class Motor
{
private:
	NxActor* actor;
	NxRevoluteJoint* joint;
	ActorUserData ud;

public:
	Motor();
	Motor(NxScene* scene, NxVec3 loc, const NxReal dirVel);
	
	void SetColour(float setR, float setG, float setB);
	NxActor* GetActor();
};