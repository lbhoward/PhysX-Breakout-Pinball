#pragma once

#include "NxPhysics.h"
#include "ShapeHelper.h"
#include "Extras\UserData.h";

class PinballTable
{
private:
	NxActor* gate;
	NxActor* actors[12];
	ActorUserData ud;
	NxVec3* verts[12];

	void InitVerts();

public:
	bool gateOpen;

	PinballTable();
	PinballTable(NxPhysicsSDK* PhysX, NxScene* scene, NxVec3 loc);

	void SetColour(int index, float setR, float setG, float setB);
	NxActor* GetActor(int index);

	void OpenGate();
	void CloseGate();
};