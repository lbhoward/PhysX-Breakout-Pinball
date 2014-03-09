/// \file Simulation.h
///
/// \brief PhysX routines and user defined functions.
/// \date September, 2011
/// \author Grzegorz Cielniak
/// \edited by Lawrence Howard for Assignment 1: CGP3001M

#pragma once

#include "NxPhysics.h"
#include "NxCooking.h"
#include "VisualDebugger.h"
#include "Extras/Timing.h"
#include "Extras/Stream.h"

#include "PinballTable.h"
#include "Block.h"
#include "Paddle.h"
#include "Motor.h"
#include "Extras/MyCloth.h"

#include "ShapeHelper.h"

#include "Extras/UserData.h"
#include <stdio.h>
#include <GL/glut.h>

/// Initialise the scene.
void InitScene();

/// User defined routine.
void UpdateScene();

/// Initialise the PhysX SDK.
bool InitPhysX();

/// Release the PhysX SDK.
void ReleasePhysX();

/// Reset the PhysX SDK.
void ResetPhysX();

/// Start a single step of simulation.
void SimulationStep();

/// Collect the simulation results.
void GetPhysicsResults();

/// User defined trigger report.
class MyTriggerReport : public NxUserTriggerReport
{
	void onTrigger(NxShape& triggerShape, NxShape& otherShape, NxTriggerFlag status);
};

/// User defined contact report.
class MyContactReport : public NxUserContactReport
{        
    void onContactNotify(NxContactPair& pair, NxU32 events);
};