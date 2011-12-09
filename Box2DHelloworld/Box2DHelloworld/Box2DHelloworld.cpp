// Box2DHelloworld.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Box2D/Box2D.h>
#include <cstdio>
using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	B2_NOT_USED(argc);
	B2_NOT_USED(argv);
	b2Vec2 gravity(0.0f, -10.0f);
	bool doSleep = true;
	//construct a world object, which will hold and simulate the rigid bodies
	b2World world(gravity);

	//Define the ground body
	b2BodyDef groundBodyDef;
	groundBodyDef.position.Set(0.0f, -10.0f);

	//Call the body factory which allocates memory for the ground body
	//from a pool and creates the ground box shape (also from a pool).
	//The body is also added to the world
	b2Body* groundBody = world.CreateBody(&groundBodyDef);

	//define the ground box shape
	b2PolygonShape groundBox;

	//the extents are the half-widths of the box.
	groundBox.SetAsBox(50.0f, 10.0f);

	//add the ground fixture to the ground body.
	groundBody->CreateFixture(&groundBox, 0.0f);

	//define the dynamic body. We set its position and call the body factory.
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set(0.0f, 4.0f);
	b2Body* body = world.CreateBody(&bodyDef);

	//define another box shape for our dynamic body.
	b2PolygonShape dynamicBox;
	dynamicBox.SetAsBox(1.0f, 1.0f);

	//define the dynamic body fixture
	b2FixtureDef fixtureDef;
	fixtureDef.shape = &dynamicBox;

	//Set the box density to be non-zero, so it will be dynamic.
	fixtureDef.density = 1.0f;

	//override the default friction.
	fixtureDef.friction = 0.3f;

	//add the shape to the body
	body->CreateFixture(&fixtureDef);

	//Prepare for simulation. Typically we use a time step of 1/60 of a second (60Hz)
	// and 10 iterations. This provides a high quality simulation is the most game scenarios
	float32 timeStep = 1.0f/60.0f;
	int32 velocityIterations = 6;
	int32 positionIterations = 2;

	//this is our little game loop.
	for (int32 i=0; i<60; ++i){
		//instruct the world to perform a single step of the simulation.
		//It is generally best to keep the time step and iterations fixed.
		world.Step(timeStep, velocityIterations, positionIterations);

		b2Vec2 position = body->GetPosition();
		float32 angle = body->GetAngle();

		printf("%4.2f %4.2f\n", position.x, position.y, angle);
	}
	//When the world destructor is called, all bodies and joints are freed.
	//This can create orphaned pointers, so be careful about your world management.
	getchar();
	return 0;
}

