#include "Motor.h"

Motor::Motor()
{
}

Motor::Motor(NxScene* scene, NxVec3 loc, const NxReal dirVel)
{
	actor = CreateCapsule(scene,0.5,4,loc,10);

	NxRevoluteJointDesc jointDesc;
	jointDesc.actor[0] = actor;
	jointDesc.setGlobalAnchor(loc);
	jointDesc.setGlobalAxis(NxVec3(0,1,0));
	jointDesc.motor.maxForce = 10000;
	jointDesc.motor.velTarget = dirVel;
	jointDesc.flags |= NX_RJF_MOTOR_ENABLED;
	joint = (NxRevoluteJoint*)scene->createJoint(jointDesc);

	ud;
	actor->userData = &ud; 
}

void Motor::SetColour(float setR, float setG, float setB)
{
	ud.colour.set(setR, setG, setB);
	actor->userData = &ud;
}

NxActor* Motor::GetActor()
{
	return actor;
}