#include "Simulation.h"

//global variables
NxPhysicsSDK* physx = 0;
NxScene* scene = 0;
NxReal delta_time;
bool gameOver = false;

//Actors
NxActor* groundPlane = 0;

//Interactive Table Additions
NxActor* leftDrum = 0; NxActor* centerDrum = 0; NxActor* rightDrum = 0;

//Spring Pulley Actors
NxActor* springPullBase = 0; NxActor* springPullTop = 0;
NxDistanceJoint* springPull = 0;

//Pinball Actors
NxActor* ball = 0;

//Bricks to break
Block* blocks[10][10];
//Pinball Table
PinballTable* pbTable;
//Paddle
Paddle* paddle;
//Motors
Motor* motorA; Motor* motorB;
//Flag
NxActor* mPoleA = 0;
MyCloth* mClothA;
//Triggers
NxActor* startTrigger = 0; NxActor* endTrigger = 0;
NxActor* trackGate = 0;

//ActorUseData to hold colour for drawing.
ActorUserData red;
ActorUserData green;
ActorUserData blue;
ActorUserData black;


NxReal SpringPullForce = 0;
bool isPulling = false;

NxMaterial* steelMat = 0;

MyTriggerReport triggerReport;
MyContactReport contactReport;


/*					FUNCTIONS
	For a descriptive overview of each function,
	see above their implementation.
											*/
//user function declarations
NxActor* CreateGroundPlane();

//Actor Creation Functions
//void PinballTable();
NxDistanceJoint* CreateSpring(NxActor* a0, NxActor* a1, const NxVec3& anchor0, const NxVec3& anchor1, const NxVec3& globalAxis);
void PinSpring();
//void Paddle();
void BlockField();
void Drums();
void setGroupContacts();

//Cloth Creation
MyCloth* CreateCloth();

//Collision Functions
void onTrigger(NxShape& a, NxShape& b, NxTriggerFlag status);

//Input Functions
void SpecialKeyboard(int key, int x, int y);
void SpecialKeyboardUp(int key, int x, int y);
void MovePaddle(NxVec3 motion);
void StopPaddle();


///
/// Initialise the SDK, hardware support, debugging parameters, default gravity etc.
///
bool InitPhysX()
{
	//initialise the SDK
	physx = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION);
	if (!physx) return false;

	//visual debugging 
	physx->setParameter(NX_VISUALIZE_JOINT_LIMITS, 1);
	physx->setParameter(NX_VISUALIZATION_SCALE, 1);
	physx->setParameter(NX_VISUALIZE_COLLISION_SHAPES, 1);
	physx->setParameter(NX_VISUALIZE_ACTOR_AXES, 1);
	physx->setParameter(NX_CONTINUOUS_CD, 1);

	//attach a remote debugger: PhysX Visual Debugger
	if (physx->getFoundationSDK().getRemoteDebugger())
		physx->getFoundationSDK().getRemoteDebugger()->connect("localhost", 5425, NX_DBG_EVENTMASK_EVERYTHING);

    //scene descriptor
    NxSceneDesc sceneDesc;

	//Tilted Table Gravity Simulator
	sceneDesc.gravity = NxVec3(0,-9.8f,-3.2f);

	//check the hardware option first
	sceneDesc.simType = NX_SIMULATION_HW;
    scene = physx->createScene(sceneDesc);

	//if not available run in software
	if(!scene)
	{ 
		sceneDesc.simType = NX_SIMULATION_SW; 
		scene = physx->createScene(sceneDesc);  
		if(!scene) return false;
	}

	scene->setUserTriggerReport(&triggerReport);
	scene->setUserContactReport(&contactReport);

	//update the current time
	getElapsedTime();

	//Register glut Special Keyboard function
	glutSpecialUpFunc(SpecialKeyboardUp);	//Release a special key (CTRL, Shift, Etc.)
	glutSpecialFunc(SpecialKeyboard);		//Press a special key (CTRL, Shift, Etc.)

	return true;
}

///
/// Release all resources.
///
void ReleasePhysX()
{
	if (scene) physx->releaseScene(*scene);
	if (physx) physx->release();
}

///
/// Restart the SDK and the scene.
///
void ResetPhysX()
{
	ReleasePhysX();
	InitPhysX();
	InitScene();
}

///
/// Start the processing of simulation using the elapsed time variable.
///
void SimulationStep()
{
	// Update the time step
	delta_time = getElapsedTime();

	// perform a simulation step for delta time since the last frame
	scene->simulate(delta_time);
	scene->flushStream();
}

///
/// Collect the simulation results. Complementary to SimulationStep function.
///
void GetPhysicsResults()
{
	scene->fetchResults(NX_RIGID_BODY_FINISHED, true);
}

///
/// Initialise all actors and their properties here.
///
void InitScene()
{
	//Ground Plane
	groundPlane = CreateGroundPlane();

	//Pinball Table
	pbTable = new PinballTable(physx, scene, NxVec3(5,2.5,-13));
	//Paddle
	paddle = new Paddle(scene, NxVec3());

	//Ball Launcher
	PinSpring();

	//Motors
	motorA = new Motor(scene,NxVec3(10,5,-30),10);
	motorB = new Motor(scene,NxVec3(-2,5,-30),-10);

	//Flag
	mPoleA = CreateBox(scene, NxVec3(0.2,4,0.2),NxVec3(18.5,10,-20),10);
	mPoleA->raiseBodyFlag(NX_BF_KINEMATIC);
	mClothA = CreateCloth();
	mClothA->getNxCloth()->attachToShape(mPoleA->getShapes()[0], NX_CLOTH_ATTACHMENT_TWOWAY);

	//Bouncy Drums
	Drums();

	//Triggers
	startTrigger = CreateBox(scene, NxVec3(1.5,1,3), NxVec3(14,5,8), 10);
	startTrigger->raiseBodyFlag(NX_BF_KINEMATIC);
	startTrigger->getShapes()[0]->setFlag(NX_TRIGGER_ENABLE, true);
	startTrigger->setGroup(4);

	endTrigger = CreateBox(scene, NxVec3(2,1,3), NxVec3(18.5,5,-48), 10);
	endTrigger->raiseBodyFlag(NX_BF_KINEMATIC);
	endTrigger->getShapes()[0]->setFlag(NX_TRIGGER_ENABLE, true);
	endTrigger->setGroup(5);

	//Pinball
	ball = CreateSphere(scene,0.5f,NxVec3(17, 5, -44),1.52f);;
	ball->raiseBodyFlag(NX_BF_FROZEN_POS_Y);
	steelMat = CreateMaterial(scene,0.4,0.4,1);
	ball->getShapes()[0]->setMaterial(steelMat->getMaterialIndex());
	ball->setGroup(3);

	BlockField();

	setGroupContacts();
}

///
/// Implement any manipulation on actors here.
///
void UpdateScene()
{
	paddle->UpdatePaddle();

	//Pull Logic
	if (isPulling)	//If the Special Key (Down) has been pressed, setting isPulling to true...
	{
		//if (SpringPullForce > -7000)	//And providing we have not exceeded the preset maximum force...
			//SpringPullForce -= 7000;	//Add more force...
		SpringPullForce -= 17000 * delta_time;
		springPullTop->addForce(NxVec3(0,0,SpringPullForce)); //To push our pulleys Top component down a little further...
	}
	else	//Otherwise, when we let go...
		SpringPullForce = 0;	//The spring launches the Top component back, propelling any Actors on top of it (the Pinball!)

	mClothA->getNxCloth()->setWindAcceleration(NxVec3(-100,0,0));
}

NxActor* CreateGroundPlane()
{
	// Create a static plane with a default descriptor
	NxActorDesc actorDesc;
	NxPlaneShapeDesc planeDesc;
	actorDesc.shapes.pushBack(&planeDesc);
	return scene->createActor(actorDesc);
}

NxDistanceJoint* CreateSpring(NxActor* a0, NxActor* a1, const NxVec3& anchor0, const NxVec3& anchor1, const NxVec3& globalAxis)
{
	//created as a distance joint, with spring property enabled
    NxDistanceJointDesc distanceDesc;
    distanceDesc.actor[0] = a0;
    distanceDesc.actor[1] = a1;
    distanceDesc.localAnchor[0] = anchor0;
    distanceDesc.localAnchor[1] = anchor1;
    distanceDesc.setGlobalAxis(globalAxis);
    
    distanceDesc.maxDistance = 0.15f; // maximum rest length of the spring
    distanceDesc.minDistance = 0.05f; // minimum rest length of the spring
    NxSpringDesc spring;
    spring.spring = 10500;
    spring.damper = 5;
    distanceDesc.spring = spring;  
    distanceDesc.flags = (NX_DJF_MIN_DISTANCE_ENABLED | NX_DJF_MAX_DISTANCE_ENABLED);  // combination of the bits defined by ::NxDistanceJointFlag
    distanceDesc.flags |= NX_DJF_SPRING_ENABLED; //enable spring

    distanceDesc.jointFlags |= NX_JF_VISUALIZATION;
	distanceDesc.jointFlags |= NX_JF_COLLISION_ENABLED;

    return (NxDistanceJoint*)scene->createJoint(distanceDesc);
}

void PinSpring()
{
	springPullBase = CreateBox(scene, NxVec3(0.75f,1,0.5f), NxVec3(0,8,0), 5);
	springPullTop = CreateBox(scene, NxVec3(0.75f,1,0.2f), NxVec3(0,8,-5), 5);

	springPullBase->userData = &blue;
	springPullTop->userData = &blue;

	springPullBase->setGlobalPosition(NxVec3(17.25,5.5,-51.5));
	springPullTop->setGlobalPosition(NxVec3(17.25,5.5,-48));
	springPullBase->raiseBodyFlag(NX_BF_KINEMATIC);
	springPullTop->raiseBodyFlag(NX_BF_FROZEN_POS_Y); //Simulate sturdy rod-type spring
	springPullTop->raiseBodyFlag(NX_BF_FROZEN_POS_X); //Simulate sturdy rod-type spring
	springPullTop->raiseBodyFlag(NX_BF_FROZEN_ROT);

	CreateSpring(springPullBase, springPullTop, NxVec3(0.75f,0.75f,0.1f), NxVec3(0.75f,0.75f,-2.4f), NxVec3(0,0,1));	
	CreateSpring(springPullBase, springPullTop, NxVec3(0.75f,-0.75f,0.1f), NxVec3(0.75f,-0.75f,-2.4f), NxVec3(0,0,1));	
	CreateSpring(springPullBase, springPullTop, NxVec3(-0.75f,0.75f,0.1f), NxVec3(-0.75f,0.75f,-2.4f), NxVec3(0,0,1));	
	CreateSpring(springPullBase, springPullTop, NxVec3(-0.75f,-0.75f,0.1f), NxVec3(-0.75f,-0.75f,-2.4f), NxVec3(0,0,1));	
}

void BlockField()
{
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 6; j++)
		{
			blocks[i][j] = new Block(scene,NxVec3(0.5,0.5,0.25), NxVec3((i*1.5)-1,5,(j*1.5)-15));

			if (j%2==0) //Check even
				blocks[i][j]->SetColour(0,1,0); //Set Green
			if (j%2==1) //Check odd
				blocks[i][j]->SetColour(0,0,1); //Set Blue


			blocks[i][j]->GetActor()->setGroup(2);
		}
	}
}

void Drums()
{
	leftDrum = CreateCylinder(physx, scene, NxVec3(0,4,2), 1, 2, 32);
	leftDrum->raiseBodyFlag(NX_BF_KINEMATIC);
	leftDrum->setGroup(1);

	rightDrum = CreateCylinder(physx, scene, NxVec3(7.75,4,2), 1, 2, 32);
	rightDrum->raiseBodyFlag(NX_BF_KINEMATIC);
	rightDrum->setGroup(1);

	centerDrum = CreateCylinder(physx, scene, NxVec3(3.8,4,5), 1, 2, 32);
	centerDrum->raiseBodyFlag(NX_BF_KINEMATIC);
	centerDrum->setGroup(1);
}

void setGroupContacts()
{
	scene->setActorGroupPairFlags(3,1,NX_NOTIFY_ON_START_TOUCH); //Ball<>Drums
	scene->setActorGroupPairFlags(3,2,NX_NOTIFY_ON_START_TOUCH); //Ball<>Blocks

	//scene->setActorGroupPairFlags(3,4, NX_TRIGGER_ON_ENTER);
}

void SpecialKeyboardUp(int key, int x, int y)
{
	switch (key)
	{
		case GLUT_KEY_DOWN:
			isPulling = false;
			break;
		case GLUT_KEY_UP:
			paddle->EndFirePaddle();
			break;
		case GLUT_KEY_LEFT:
			StopPaddle();
			break;
		case GLUT_KEY_RIGHT:
			StopPaddle();
			break;
	}
}

void SpecialKeyboard(int key, int x, int y)
{
	if(!gameOver)
	{
		switch (key)
		{
		case GLUT_KEY_DOWN:
			isPulling = true;
			break;
		case GLUT_KEY_UP:
			paddle->FirePaddle();
			break;
		case GLUT_KEY_LEFT:
			MovePaddle(NxVec3(10,0,0));
			break;
		case GLUT_KEY_RIGHT:
			MovePaddle(NxVec3(-10,0,0));
			break;
		case GLUT_KEY_F4:
			exit(0);
			break;
		}
	}
	else
	{
		switch (key)
		{
			case GLUT_KEY_F4:
				exit(0);
				break;
		}
	}
}

void MovePaddle(NxVec3 motion)
{
	paddle->MovePaddle(motion);
}

void StopPaddle()
{
	paddle->StopPaddle();
}

MyCloth* CreateCloth()
{
	NxClothDesc clothDesc;
	clothDesc.globalPose.t = NxVec3(18.5,14.5,-20);
	clothDesc.globalPose.M.rotX(-1.57);
	clothDesc.thickness = 0.2;
	clothDesc.flags |= NX_CLF_COLLISION_TWOWAY | NX_CLF_VISUALIZATION;

	return (new MyCloth(scene, clothDesc, 5, 5, 0.5, NULL, false));
}

void MyTriggerReport::onTrigger(NxShape& triggerShape, NxShape& otherShape, NxTriggerFlag status)
{
	if(status & NX_TRIGGER_ON_ENTER)
	{
		//Check to see if the ball has entered the Launch Area
		if (triggerShape.getActor().getGroup() == 5 && otherShape.getActor().getGroup() == 3)
		{
			//Check if that was the last ball
			if (DecBalls(true) == 0)
			{
				gameOver = true;
				DisplayGameOver();
			}
			else
				pbTable->OpenGate(); //Open the safety gate
		}
	}
	if(status & NX_TRIGGER_ON_LEAVE)
	{
		//Check to see if the ball has left the Launch Area
		if (triggerShape.getActor().getGroup() == 4 && otherShape.getActor().getGroup() == 3)
		{
			//If the safety gate is open, and the ball has left the area
			if (pbTable->gateOpen)
				DecBalls(false);			//Decrement the amount of balls left

			pbTable->CloseGate(); //Close the safety gate
		}
	}
}

void MyContactReport::onContactNotify(NxContactPair& pair, NxU32 events)
{
	//you can read the contact information out of the contact pair data here.

	//check if ON_START_TOUCH event
	if (events & NX_NOTIFY_ON_START_TOUCH)
	{
		//Ball<>Blocks
		if (pair.actors[0]->getGroup() == 2 && pair.actors[1]->getGroup() == 3)
		{
			ActorUserData* ud = (ActorUserData*)pair.actors[0]->userData;

			//Check which colour block we just hit, call the increment score function from VisualDebugger.cpp
			if (ud->colour == NxVec3(0,1,0))
				IncScore(1);
			if (ud->colour == NxVec3(0,0,1))
				IncScore(2);

			//Delete that actor
			scene->releaseActor(*pair.actors[0]);
		}

		//Ball<>Drums
		else if (pair.actors[0]->getGroup() == 3 && pair.actors[1]->getGroup() == 1)
		{
			//Calculate the position of the ball, as a normal vector, from the drum it just hit
			NxVec3 bounceForce = pair.actors[0]->getGlobalPosition() - pair.actors[1]->getGlobalPosition();
			bounceForce.normalize();
			//Rebound the ball in the opposite direction
			pair.actors[0]->addForce(bounceForce*1000);
		}
	}
}