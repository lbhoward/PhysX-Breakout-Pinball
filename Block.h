#pragma once

#include "NxPhysics.h";
#include "ShapeHelper.h";
#include "Extras\UserData.h";

class Block
{
private:
	NxActor* actor;
	ActorUserData ud;

public:
	Block();
	Block(NxScene* scene, NxVec3 dimensions, NxVec3 loc);
	
	void SetColour(float setR, float setG, float setB);
	NxActor* GetActor();
};