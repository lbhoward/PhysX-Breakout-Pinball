#include <stdio.h>		// C header for debug output
#define NOMINMAX
#include <windows.h>	// delay, keyboard input
#include <NxPhysics.h>	// PhysX SDK

//function declarations
bool InitPhysX();
void ReleasePhysX();
void InitScene();
void Display();
void UpdateScene();
void SimulationStep();
void GetPhysicsResults();

NxActor* CreateGroundPlane();
NxActor* CreateBox();

//global variables
NxPhysicsSDK* physx = 0;
NxScene* scene = 0;
NxReal delta_time = 1.0f/60.0f; // 1/60th of a second
NxActor* ground_plane = 0;
NxActor* box = 0;

int main()
{
	//initialise PhysX
	if (!InitPhysX())
	{
		printf("Could not initialise PhysX.\n");
		ReleasePhysX();
		exit(1);
	}

	//populate the scene with actors
	InitScene();

	//loop until the 'Esc' key is pressed
	while (!GetAsyncKeyState(VK_ESCAPE))
	{
		//perform one step of the simulation
		SimulationStep();

		//while calculating render the results (from the previous step)
		Display();

		//a short pause, just to slow down the display output, adjust according to your preferences
		Sleep(200);

		//update the simulation results
		GetPhysicsResults();

		//custom processing procedure - apply forces, react to collisions etc.
		UpdateScene();
	}

	//clean up memory
	ReleasePhysX();

	return 0;
}

///
/// Initialise the SDK, debugging parameters, default gravity etc.
///
bool InitPhysX()
{
	//initialise the SDK
	physx = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION);
	if (!physx) return false;

	//create the scene
	NxSceneDesc sceneDesc;
	//set default gravity
	sceneDesc.gravity = NxVec3(0, -9.8f, 0);
	
	scene = physx->createScene(sceneDesc);  
	if(!scene) return false;

	//attach a remote debugger: PhysX Visual Debugger
	if (physx->getFoundationSDK().getRemoteDebugger())
		physx->getFoundationSDK().getRemoteDebugger()->connect("localhost", 5425, NX_DBG_EVENTMASK_EVERYTHING);

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
/// Initialise all actors and their properties here.
///
void InitScene()
{
	ground_plane = CreateGroundPlane();
	box = CreateBox();
}

///
/// Start the processing of simulation using the fixed time variable.
///
void SimulationStep()
{
	scene->simulate(delta_time);
	scene->flushStream();
}

///
/// Collect the simulation results. Complementary to SimulationStep function.
///
void GetPhysicsResults()
{
	//wait until the results are ready
	scene->fetchResults(NX_RIGID_BODY_FINISHED, true);
}


///
/// Implement any manipulation on actors here.
///
void UpdateScene()
{
}

void Display()
{
	//display object properties - global position and velocity
	NxVec3 pos = box->getGlobalPosition();
	NxVec3 vel = box->getLinearVelocity();

	printf("pos %.2f %.2f %.2f :: ", pos.x, pos.y, pos.z);
	printf("vel %.2f %.2f %.2f\n", vel.x, vel.y, vel.z);
}

NxActor* CreateGroundPlane()
{
	// Create a static plane with a default descriptor
	NxActorDesc actorDesc;
	NxPlaneShapeDesc planeDesc;
	actorDesc.shapes.pushBack(&planeDesc);
	return scene->createActor(actorDesc);
}

NxActor* CreateBox()
{
	// Set the initial height of the box to 3.5m so that it starts off falling onto the ground
	NxReal boxStartHeight = 3.5; 

	// Actor, body and shape descriptors
	NxActorDesc actorDesc;
	NxBodyDesc bodyDesc;
	NxBoxShapeDesc boxDesc;

	// The actor has one shape, a box, 1m on a side
	boxDesc.dimensions.set(0.5, 0.5, 0.5);	//requires half dimension values
	actorDesc.shapes.pushBack(&boxDesc);

	actorDesc.body			= &bodyDesc;
	actorDesc.density		= 10.0f; // kg/m^3
	actorDesc.globalPose.t	= NxVec3(0, boxStartHeight, 0);	

	return scene->createActor(actorDesc);	
}
