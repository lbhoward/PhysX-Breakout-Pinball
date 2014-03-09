#include "Block.h"

Block::Block()
{
}

Block::Block(NxScene* scene, NxVec3 dimensions, NxVec3 loc)
{
	actor = CreateBox(scene,dimensions,loc,5);
	actor->raiseBodyFlag(NX_BF_KINEMATIC);
	actor->userData = &ud; 
}

void Block::SetColour(float setR, float setG, float setB)
{
	ud.colour.set(setR, setG, setB);
	actor->userData = &ud;
}

NxActor* Block::GetActor()
{
	return actor;
}