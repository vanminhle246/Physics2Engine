/////////////////////////////////////////////////////////////////
//
// tutorial1app.h
// Application class
//
/////////////////////////////////////////////////////////////////
//
// Copyright (C) 2009 Daniel Jeppsson
// All Rights Reserved. These instructions, statements, computer
// programs, and/or related material (collectively, the "Source")
// contain unpublished information propietary to Daniel Jeppsson
// which is protected by US federal copyright law and by 
// international treaties. This Source may NOT be disclosed to 
// third parties, or be copied or duplicated, in whole or in 
// part, without the written consent of Daniel Jeppsson.
//
/////////////////////////////////////////////////////////////////
// 
// Author: Daniel Jeppsson
// 
/////////////////////////////////////////////////////////////////

#include <djapp.h>
#include <dj2d.h>
#include <chipmunk.h>

#ifndef _TUTORIAL1APP_H_
#define _TUTORIAL1APP_H_

/////////////////////////////////////////////////////////////////
DJ_FILE_START();
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
// Layer enum
enum
{
	// Layer for background image
	LAYER_BACKGROUND	= 0,
	// Layer for our sprites
	LAYER_SPRITES		= 1,
	// Layer for overlay and UI
	LAYER_OVERLAY		= 2,

	// Number of layers
	LAYER_COUNT			= 3,
};
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
// Sound enum
enum
{
	// ZAP sound effect
	SOUND_ZAP = 0,
	// Tick sound effect
	SOUND_TICK,

	// Number of sound effects
	SOUND_COUNT
};


/////////////////////////////////////////////////////////////////
class DJTutorial1Application : public DJApplication
{
	IMPLEMENT_INTERFACE(Tutorial1Application, Application)

public:

	/////////////////////////////////////////////////////////////////
	// CONSTRUCTION/DESTRUCTION

	// Constructor
	DJTutorial1Application( );
	// Destructor
	virtual ~DJTutorial1Application( );

	// Application initialization
	virtual djresult OnInit( );
	// Application termination
	virtual void OnTerm( );

	/////////////////////////////////////////////////////////////////
	// OPERATIONS

	// Called once per frame from main loop
	virtual void OnUpdate( );

	// Called once per frame to render the application
	virtual void OnPaint( );

	/////////////////////////////////////////////////////////////////
	// EVENT HANDLERS

	// Touch begin callback
	virtual djint32 OnTouchBegin( djint32 nID, float x, float y );
	// Touch move callback
	virtual djint32 OnTouchMove( djint32 nID, float x, float y );
	// Touch end callback
	virtual djint32 OnTouchEnd( djint32 nID, float x, float y );
	// Accelerator event callback
	virtual djint32 OnAccelerate( float x, float y, float z );
	// Button Down
	virtual djint32 OnButtonDown(djint32 nKey);
	// Button Up
	virtual djint32 OnButtonUp(djint32 nKey);

	// Handle incoming system event
	virtual void OnMessage( djuint32 nMessage, djuint32 nParam1 = 0, djuint32 nParam2 = 0 );
};
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
DJ_FILE_END();
/////////////////////////////////////////////////////////////////

#endif // _TUTORIAL1_H_
