#include "Paddle.h"

Paddle::Paddle()
{
}

Paddle::Paddle(NxScene* scene, NxVec3 loc)
{
	InitActors(scene);
}

void Paddle::InitActors(NxScene* scene)
{
	actors[0] = CreateBox(scene, NxVec3(0.5f,0.5f,0.5f), NxVec3(2.5f,5,-42.0f), 10000);
	actors[3] = CreateBox(scene, NxVec3(0.5f,0.5f,0.5f), NxVec3(-2.5f,5,-42.0f), 10000);

	//actors[0]->userData = &blue;
	//actors[3]->userData = &blue;

	actors[0]->raiseBodyFlag(NX_BF_FROZEN_POS_Y);
	actors[0]->raiseBodyFlag(NX_BF_FROZEN_POS_Z);
	actors[0]->raiseBodyFlag(NX_BF_FROZEN_ROT);
	actors[3]->raiseBodyFlag(NX_BF_FROZEN_POS_Y);
	actors[3]->raiseBodyFlag(NX_BF_FROZEN_POS_Z);
	actors[3]->raiseBodyFlag(NX_BF_FROZEN_ROT);

	NxDistanceJointDesc distanceDesc;
    distanceDesc.actor[0] = actors[0];
    distanceDesc.actor[1] = actors[3];
    distanceDesc.localAnchor[0] = NxVec3(-0.5f,0,0);
    distanceDesc.localAnchor[1] = NxVec3(0.5f,0,0);
    distanceDesc.setGlobalAxis(NxVec3(1,0,0));
    
    distanceDesc.maxDistance = 4.0f; // maximum rest length of the spring
    distanceDesc.minDistance = 4.0f; // minimum rest length of the spring

	distanceDesc.flags = (NX_DJF_MIN_DISTANCE_ENABLED | NX_DJF_MAX_DISTANCE_ENABLED);  // combination of the bits defined by ::NxDistanceJointFlag

	(NxDistanceJoint*)scene->createJoint(distanceDesc);

	actors[1] = CreateCapsule(scene, 0.5, 1.0, NxVec3(1.0,5,-42.0f), 10);
	NxRevoluteJointDesc leftRevDesc;
	leftRevDesc.actor[0] = actors[1];
	leftRevDesc.actor[1] = actors[0];
	leftRevDesc.setGlobalAxis(NxVec3(0,1,0));      //The direction of the axis we revolve around

	joints[0] = (NxRevoluteJoint*)scene->createJoint(leftRevDesc);
	NxJointLimitPairDesc leftLimit;
	leftLimit.low.value = -NxPi*0.15;
	leftLimit.high.value = NxPi*0.25;
	joints[0]->setLimits(leftLimit);

	actors[2] = CreateCapsule(scene, 0.5, 1.0, NxVec3(-1.1,5,-42.0f), 10);
	NxRevoluteJointDesc rightRevDesc;
	rightRevDesc.actor[0] = actors[2];
	rightRevDesc.actor[1] = actors[3];
	rightRevDesc.setGlobalAxis(NxVec3(0,1,0));      //The direction of the axis we revolve around

	joints[1] = (NxRevoluteJoint*)scene->createJoint(rightRevDesc);
	NxJointLimitPairDesc rightLimit;
	rightLimit.low.value = -NxPi*0.35;
	rightLimit.high.value = NxPi*0.15;
	joints[1]->setLimits(rightLimit);
}

void Paddle::SetColour(float setR, float setG, float setB)
{
	ud.colour.set(setR, setG, setB);
	//actor->userData = &ud;
}

NxActor* Paddle::GetActor(int index)
{
	return actors[index];
}

void Paddle::MovePaddle(NxVec3 motion)
{
	for (int i = 0; i < 4; i++)
	{
		actors[i]->setLinearVelocity(motion);
	}
}

void Paddle::StopPaddle()
{
	for (int i = 0; i < 4; i++)
	{
		actors[i]->setLinearVelocity(NxVec3(0,0,0));
	}
}

void Paddle::FirePaddle()
{
	actors[1]->addForce(NxVec3(0,0,6000));
	actors[2]->addForce(NxVec3(0,0,6000));
}

void Paddle::EndFirePaddle()
{
	actors[1]->addForce(NxVec3(0,0,-3000));
	actors[2]->addForce(NxVec3(0,0,-3000));
}

void Paddle::UpdatePaddle()
{
	//Keep the anchor-point updated so that the Flippers remain on the Paddle
	NxVec3 leftFlipAnchor = actors[0]->getGlobalPosition();
	joints[0]->setGlobalAnchor(leftFlipAnchor);
	NxVec3 rightFlipAnchor = actors[3]->getGlobalPosition();
	joints[1]->setGlobalAnchor(rightFlipAnchor);
}