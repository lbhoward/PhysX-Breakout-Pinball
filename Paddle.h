#pragma once

#include "NxPhysics.h";
#include "ShapeHelper.h";
#include "Extras\UserData.h";

class Paddle
{
private:
	NxActor* actors[4];
	NxRevoluteJoint* joints[2];
	ActorUserData ud;

	void InitActors(NxScene* scene);

public:
	Paddle();
	Paddle(NxScene* scene, NxVec3 loc);
	
	void SetColour(float setR, float setG, float setB);
	NxActor* GetActor(int index);

	void MovePaddle(NxVec3 motion);
	void StopPaddle();
	void FirePaddle();
	void EndFirePaddle();

	void UpdatePaddle();
};