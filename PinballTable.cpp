#include "PinballTable.h"
#include <iostream>;

PinballTable::PinballTable()
{
}

PinballTable::PinballTable(NxPhysicsSDK* PhysX, NxScene* scene, NxVec3 loc)
{
	gateOpen = true;
	gate = CreateBox(scene, NxVec3(0.3,1,2), NxVec3(16.3,5.5,0), 10);
	gate->raiseBodyFlag(NX_BF_KINEMATIC);

	ud.colour.set(0.6,0,1);
	gate->userData = &ud;

	InitVerts();

	for (int i = 0; i < 12; i++)
	{
		bool indexCaught = false;
		int j = 0;

		while (!indexCaught)
		{
			if (verts[i][j].x == NULL)
				indexCaught = true;
			else
				j++;
		}

		actors[i] = ConvexMesh(PhysX, scene, verts[i], loc, j-1);
		actors[i]->raiseBodyFlag(NX_BF_KINEMATIC);
		actors[i]->userData = &ud;
	}
}

void PinballTable::InitVerts()
{
	//Top Left Corner
	verts[0] = new NxVec3[6];
	verts[0][0] = NxVec3(13,2,19); verts[0][1] = NxVec3(13,2,24); verts[0][2] = NxVec3(8,2,24);
	verts[0][3] = NxVec3(13,4,19); verts[0][4] = NxVec3(13,4,24); verts[0][5] = NxVec3(8,4,24);

	//Top Right Corner
	verts[1] = new NxVec3[6];
	verts[1][0] = NxVec3(-13,2,19); verts[1][1] = NxVec3(-13,2,24); verts[1][2] = NxVec3(-8,2,24);
	verts[1][3] = NxVec3(-13,4,19); verts[1][4] = NxVec3(-13,4,24); verts[1][5] = NxVec3(-8,4,24);

	//Left Barrier
	verts[2] = new NxVec3[6];
	verts[2][0] = NxVec3(11,2,-22); verts[2][1] = NxVec3(11,2,-27.5); verts[2][2] = NxVec3(7,2,-27.5);
	verts[2][3] = NxVec3(11,4,-22); verts[2][4] = NxVec3(11,4,-27.5); verts[2][5] = NxVec3(7,4,-27.5);

	//Right Barrier
	verts[3] = new NxVec3[6];
	verts[3][0] = NxVec3(-13,2,-22); verts[3][1] = NxVec3(-13,2,-27.5); verts[3][2] = NxVec3(-9,2,-27.5);
	verts[3][3] = NxVec3(-13,4,-22); verts[3][4] = NxVec3(-13,4,-27.5); verts[3][5] = NxVec3(-9,4,-27.5);

	//Left Block Barrier
	verts[4] = new NxVec3[8];
	verts[4][0] = NxVec3(5,2,10); verts[4][1] = NxVec3(5,2,11); verts[4][2] = NxVec3(1,2,12); verts[4][3] = NxVec3(1,2,13);
	verts[4][4] = NxVec3(5,3,10); verts[4][5] = NxVec3(5,3,11); verts[4][6] = NxVec3(1,3,12); verts[4][7] = NxVec3(1,3,13);

	//Right Block Barrier
	verts[5] = new NxVec3[8];
	verts[5][0] = NxVec3(-7,2,10); verts[5][1] = NxVec3(-7,2,11); verts[5][2] = NxVec3(-3,2,12); verts[5][3] = NxVec3(-3,2,13);
	verts[5][4] = NxVec3(-7,3,10); verts[5][5] = NxVec3(-7,3,11); verts[5][6] = NxVec3(-3,3,12); verts[5][7] = NxVec3(-3,3,13);

	//Base
	verts[6] = new NxVec3[8];
	verts[6][0] = NxVec3(14,0,-38); verts[6][1] = NxVec3(-14,0,-38); verts[6][2] = NxVec3(-14,0,25); verts[6][3] = NxVec3(14,0,25); 
	verts[6][4] = NxVec3(14,2,-38); verts[6][5] = NxVec3(-14,2,-38); verts[6][6] = NxVec3(-14,2,25); verts[6][7] = NxVec3(14,2,25); 

	//Top-Beam
	verts[7] = new NxVec3[8];
	verts[7][0] = NxVec3(14,2,24); verts[7][1] = NxVec3(-14,2,24); verts[7][2] = NxVec3(-14,2,25); verts[7][3] = NxVec3(14,2,25); 
	verts[7][4] = NxVec3(14,4,24); verts[7][5] = NxVec3(-14,4,24); verts[7][6] = NxVec3(-14,4,25); verts[7][7] = NxVec3(14,4,25); 
	
	//Left-Beam
	verts[8] = new NxVec3[8];
	verts[8][0] = NxVec3(14,2,24); verts[8][1] = NxVec3(13,2,24); verts[8][2] = NxVec3(13,2,-38); verts[8][3] = NxVec3(14,2,-38); 
	verts[8][4] = NxVec3(14,4,24); verts[8][5] = NxVec3(13,4,24); verts[8][6] = NxVec3(13,4,-38); verts[8][7] = NxVec3(14,4,-38); 

	//Right-Beam
	verts[9] = new NxVec3[8];
	verts[9][0] = NxVec3(-14,2,24); verts[9][1] = NxVec3(-13,2,24); verts[9][2] = NxVec3(-13,2,-38); verts[9][3] = NxVec3(-14,2,-38); 
	verts[9][4] = NxVec3(-14,4,24); verts[9][5] = NxVec3(-13,4,24); verts[9][6] = NxVec3(-13,4,-38); verts[9][7] = NxVec3(-14,4,-38); 

	//Slope-Beam
	verts[10] = new NxVec3[8];
	verts[10][0] = NxVec3(11.5,2,-38); verts[10][1] = NxVec3(-14,2,-38); verts[10][2] = NxVec3(-14,2,-30); verts[10][3] = NxVec3(11.5,2,-35); 
	verts[10][4] = NxVec3(11.5,4,-38); verts[10][5] = NxVec3(-14,4,-38); verts[10][6] = NxVec3(-14,4,-30); verts[10][7] = NxVec3(11.5,4,-35); 

	//Track-Beam
	verts[11] = new NxVec3[8];
	verts[11][0] = NxVec3(11.5,2,17); verts[11][1] = NxVec3(11,2,17); verts[11][2] = NxVec3(11,2,-33.5); verts[11][3] = NxVec3(11.5,2,-33.5); 
	verts[11][4] = NxVec3(11.5,4,17); verts[11][5] = NxVec3(11,4,17); verts[11][6] = NxVec3(11,4,-33.5); verts[11][7] = NxVec3(11.5,4,-33.5); 
}

void PinballTable::SetColour(int index, float setR, float setG, float setB)
{
	ud.colour.set(setR, setG, setB);
	actors[index]->userData = &ud;
}

NxActor* PinballTable::GetActor(int index)
{
	return actors[index];
}

void PinballTable::OpenGate()
{
	gate->setGlobalPosition(NxVec3(16.3,5.5,0));
	gateOpen = true;
}

void PinballTable::CloseGate()
{
	gate->setGlobalPosition(NxVec3(16.3,5.5,5));
	gateOpen = false;
}