#include "VisualDebugger.h"
#include "Simulation.h"
#include "Extras/DrawObjects.h"
#include "Extras/Timing.h"
#include "Extras/UserData.h"
#include "Extras/HUD.h"
#include "Extras/MyCloth.h"
#include <GL/glut.h>
//extern variables, defined in Simulation.cpp
extern NxScene* scene;
extern NxReal delta_time;
extern MyCloth* mClothA;

HUD hud;

//global variables
bool bHardwareScene = false;
bool bPause = false;
bool bShadows = true;
RenderingMode rendering_mode = RENDER_SOLID;
DebugRenderer gDebugRenderer;
const NxDebugRenderable* debugRenderable = 0;

//Game Variables
int score = 0;
int lastHit = 0;
char charScore[20];
char charBalls[2];
int ballsLeft = 3;

// Force globals
NxVec3	gForceVec(0,0,0);
NxReal	gForceStrength	= 10000;

// Keyboard globals
#define MAX_KEYS 256
bool gKeys[MAX_KEYS];

// Camera globals
float	gCameraAspectRatio = 1.0f;
NxVec3	gCameraPos(5,25,-58);
NxVec3	gCameraForward(0,-1,1);
NxVec3	gCameraRight(-1,0,0);
const NxReal gCameraSpeed = 10;

///
/// Assign callback functions, set-up lighting
///
void Init(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitWindowSize(1024, 768);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

	//callbacks
	glutSetWindow(glutCreateWindow("Breakout Pinball"));
	glutDisplayFunc(RenderCallback);
	glutReshapeFunc(ReshapeCallback);
	glutIdleFunc(IdleCallback);
	atexit(ExitCallback);

	//default render states
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_CULL_FACE);

	//lighting
	glEnable(GL_LIGHTING);
	float AmbientColor[]    = { 0.1f, 0.1f, 0.1f, 0.0f };
	float AmbientColor2[]    = { 0.2f, 0.1f, 0.0f, 0.0f };			
	float DiffuseColor[]    = { 0.2f, 0.2f, 0.2f, 0.0f };			
	float SpecularColor[]   = { 0.5f, 0.5f, 0.5f, 0.0f };			
	float Position[]        = { 100.0f, 100.0f, -400.0f, 1.0f };

	//default lighting
	glLightfv(GL_LIGHT0, GL_AMBIENT, AmbientColor);	
	glLightfv(GL_LIGHT0, GL_DIFFUSE, DiffuseColor);	
	glLightfv(GL_LIGHT0, GL_SPECULAR, SpecularColor);	
	glLightfv(GL_LIGHT0, GL_POSITION, Position);
	glEnable(GL_LIGHT0);

	// Initialise the PhysX SDK.
	if (!InitPhysX())
	{
		printf("Could not initialise PhysX.\n");
		ExitCallback();
	}

	// Initialise the HUD
	hud.Clear();
	//Score Display
	hud.AddDisplayString("Score: ", 0,0.95);
	itoa(score,charScore,10);
	hud.AddDisplayString(charScore, 0.15,0.95);
	//Balls Left Display
	hud.AddDisplayString("Balls Left: ", 0.8,0.95);
	itoa(ballsLeft,charBalls,10);
	hud.AddDisplayString(charBalls, 0.95,0.95);

	// Initialise the simulation scene.
	InitScene();

	PrintControls();
}

///
/// Start the first step of simulation and enter the GLUT's main loop.
///
void StartMainLoop() 
{
	glutMainLoop(); 
}

void Display()
{
	//clear display buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//set the camera view
	SetupCamera();

	//display scene
	if (rendering_mode != RENDER_WIREFRAME)
		RenderActors(bShadows);
	if (debugRenderable)
		gDebugRenderer.renderData(*debugRenderable);

	//Update HUD
	itoa(score,charScore,10);
	hud.SetDisplayString(1,charScore,0.15,0.95);
	itoa(ballsLeft,charBalls,10);
	hud.SetDisplayString(3,charBalls,0.95,0.95);
	hud.Render();

	glFlush();
	glutSwapBuffers();
}

///
/// Perform one step of simulation, process input keys and render the scene.
///
void RenderCallback()
{
	if (scene && !bPause)
	{
		//get new results
		GetPhysicsResults();

		debugRenderable = 0;
		if (rendering_mode != RENDER_SOLID)
			debugRenderable = scene->getDebugRenderable();

		//user defined process function
		UpdateScene();

		//start new simulation step
		SimulationStep();
	}

	Display();
}

///
/// Draw a force arrow.
///
void DrawForce(NxActor* actor, NxVec3& forceVec, const NxVec3& color)
{
	if (actor)
	{
		// Draw only if the force is large enough
		NxReal force = forceVec.magnitude();
		if (force < 0.1)  return;

		forceVec = 2*forceVec/force;

		NxVec3 pos = actor->getCMassGlobalPosition();
		DrawArrow(pos, pos + forceVec, color);
	}
}

///
/// Function to handle the Rendering of all Actors
///
void RenderActors(bool shadows)
{
	//iterate through all actors
	NxU32 nbActors = scene->getNbActors();
	NxActor** actors = scene->getActors();
	while (nbActors--)
	{
		NxActor* actor = *actors++;

		DrawActor(actor, 0, false);

		if (shadows)
			DrawActorShadow(actor, false);
	}

	if (mClothA)
	{
		glColor3f(1.0,0.0,0.0);
		mClothA->draw(shadows);
	}
}

///
/// Setup the camera view veriables.
///
void SetupCamera()
{
	gCameraAspectRatio = (float)glutGet(GLUT_WINDOW_WIDTH) / (float)glutGet(GLUT_WINDOW_HEIGHT);

	// Setup camera
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0f, gCameraAspectRatio, 1.0f, 10000.0f);
	gluLookAt(gCameraPos.x, gCameraPos.y, gCameraPos.z,
		gCameraPos.x+gCameraForward.x, gCameraPos.y+gCameraForward.y, gCameraPos.z+gCameraForward.z,
		0.0f, 1.0f, 0.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

///
/// Setup the camera view variables.
///
void ResetCamera()
{
	gCameraAspectRatio = 1.0f;
	gCameraPos = NxVec3(0,5,-15);
	gCameraForward = NxVec3(0,0,1);
	gCameraRight = NxVec3(-1,0,0);
}

///
/// Change the view point and aspect ratio.
///
void ReshapeCallback(int width, int height)
{
	glViewport(0, 0, width, height);
	gCameraAspectRatio = float(width)/float(height);
}

void IdleCallback() { glutPostRedisplay(); }

///
/// Release PhysX SDK on exit.
///
void ExitCallback() { ReleasePhysX(); }

///
/// Simple info printed out in the command line.
///
void PrintControls()
{
	hud.AddDisplayString("Controls", 0, 0.9);
	hud.AddDisplayString("Move paddle left: Left Arrow",0,0.85);
	hud.AddDisplayString("Move paddle right: Right Arrow",0,0.8);
	hud.AddDisplayString("Activate paddle flippers: Up Arrow",0,0.75);
	hud.AddDisplayString("Pull pinspring down: Down Arrow",0,0.7);
	hud.AddDisplayString("Exit game: F4",0,0.65);
}

///
/// Increment the score based on last hit block
///
void IncScore(int thisHit)
{
	//Check to see if we have scored a streak
	if (lastHit == thisHit)
		score += 200;	//If so, more points are awarded
	else
		score += 100;	//Otherwise, award the standard point value

	lastHit = thisHit;	//Store which colour the ball just hit
}


///
/// Check how many balls left, or decrement them
///
int DecBalls(bool justCheck)
{
	if (!justCheck)
		ballsLeft--;

	return ballsLeft;
}

///
/// Game Over Message
///
void DisplayGameOver()
{
	hud.AddDisplayString("GAME OVER",0.5,0.5);
}