/////////////////////////////////////////////////////////////////
//
// tutorial1app.cpp
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

/////////////////////////////////////////////////////////////////
// Engine Includes
#include <djmodulemanager.h>
#include <djostime.h>
#include <dj2d.h>
#include <dj2dutil.h>
#include <djgamesounds.h>
#include <djfont.h>

/////////////////////////////////////////////////////////////////
// Game Includes
#include "tutorial1app.h"
#include "..\LinkerAgent.h"

/////////////////////////////////////////////////////////////////
// Name of the game module
const char *GetGameModuleName()
{
	return "Tutorial1";
}
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
// Name of the game application
const char *GetGameObjectName()
{
	return "Tutorial1Application";
}
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
IMPORT_STATIC_MODULE(Tutorial1)
IMPORT_STATIC_MODULE(OpenGLRender)
#if defined (_DJAIRPLAY)
IMPORT_STATIC_MODULE(AirSound)
#else
IMPORT_STATIC_MODULE(OpenALSound)
#endif
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
void DJSystem_AddStaticModules()
{
	ADD_STATIC_MODULE(OpenGLRender);
#if defined (_DJAIRPLAY)
	ADD_STATIC_MODULE(AirSound);
#else
	ADD_STATIC_MODULE(OpenALSound);
#endif
	ADD_STATIC_MODULE(Tutorial1);
}
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
BEGIN_ENUMERATE_INTERFACE()
ENUMERATE_INTERFACE(Tutorial1Application)
END_ENUMERATE_INTERFACE()
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
BEGIN_CREATE_INTERFACE()
CREATE_INTERFACE(Tutorial1Application)
END_CREATE_INTERFACE()
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
BEGIN_REGISTER_SYMBOL(Tutorial1)
REGISTER_INTERFACE(Tutorial1Application)
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
REGISTER_ENUMERATE_INTERFACE()
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
END_REGISTER_SYMBOL()
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
// Globals
// Screen width (set in OnInit function)
djuint32				g_nScreenWidth			= 0;
// Screen height (set in OnInit function)
djuint32				g_nScreenHeight			= 0;
// Sheet of graphics for the flare sprite
DJ2DGraphicsSheet*		g_pBallSheet			= NULL;
// Sheet of graphics for the background
DJ2DGraphicsSheet*		g_pBackgroundSheet		= NULL;
// Our test sprite
DJ2DSprite*				g_pBallSprite				= NULL;
// A font to use for drawing text
DJFont*					g_pFont					= NULL;

/*coded by Minh*/
DJ2DGraphicsSheet*		g_pHorizontalBarSheet	= NULL;
DJ2DSprite*				g_pHorizontalBarSprite	= NULL;
cpSpace*				g_pSpace				= NULL;
cpBody*					g_pBallBody				= NULL;
LinkerAgent*			g_pBallLinkerAgent		= NULL;
/**/

/////////////////////////////////////////////////////////////////
// List of sounds to load
static const char *l_szSoundFiles[SOUND_COUNT] =
{
	"sound/zap1.wav",
	"sound/tick.wav",
};
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
// Application class
DJTutorial1Application::DJTutorial1Application()
{
	DJTrace("%s()", __FUNCTION__);
}

///

DJTutorial1Application::~DJTutorial1Application()
{
	DJTrace("%s()", __FUNCTION__);
}

///

djresult DJTutorial1Application::OnInit()
{
	DJTrace("%s()", __FUNCTION__);

	// Set screen orientation depending on device
//#if defined(_DJIPHONE)
//	theSpriteEngine.SetScreenOrientation(1);
//#else
	theSpriteEngine.SetScreenOrientation(0);
//#endif
	
	// Get size of screen and put in global variables
	pTheRenderDevice->GetScreenSize(g_nScreenWidth, g_nScreenHeight);

	// Randomize random seed by using system time
	djRandomSetSeed( (djuint32)(djGetSystemTimeFloat() * 65536.0f) );
	
	// Initialize sprite engine with the layers we want
	if (!theSpriteEngine.Init(LAYER_COUNT))
	{
		DJError("Failed to initialize the sprite engine!");
		return DJ_FAILURE;
	}

	// Load font
	g_pFont = (DJFont*)theResourceManager.GetResource( "font", DJResource::TYPE_FONT );
	if (g_pFont == NULL)
	{
		DJError("Failed to load the font!");
		return DJ_FAILURE;
	}

	// Create a flare sprite sheet
	g_pBallSheet = DJ_NEW(DJ2DGraphicsSheet);
	if (!g_pBallSheet->Init("sprites/ball.tga"))
	{
		DJError("Failed to load flare sheet!");
		return DJ_FAILURE;
	}
	DJVector2 vHotspot = DJVector2(40/2, 40/2); 
	g_pBallSheet->AutoGenFrames(0, 0, 40, 40, 1, 1, &vHotspot);
	theSpriteEngine.AddGraphicsSheet(g_pBallSheet);

	// Create background image sheet
	g_pBackgroundSheet = DJ_NEW(DJ2DGraphicsSheet);
	if (!g_pBackgroundSheet->Init("backgrounds/space001.tga"))
	{
		DJError("Failed to load background sheet!");
		return DJ_FAILURE;
	}
	g_pBackgroundSheet->AutoGenFrames(0, 0, 1024, 512, 1, 1, NULL);
	theSpriteEngine.AddGraphicsSheet(g_pBackgroundSheet);
		
	// Set the background layer to draw the background image
	DJ2DLayer *pLayer = theSpriteEngine.GetLayer(LAYER_BACKGROUND);
	DJ2DGraphicsFrame *pFrame = g_pBackgroundSheet->GetFrame(0);
	//pLayer->SetBackground(pFrame);
	//pLayer->SetPosition(0,0);

	// Create a sprite we can later draw
	g_pBallSprite = DJ_NEW(DJ2DSprite);
	// Make the sprite magnify with a blur instead of pixelated
	g_pBallSprite->SetFilter(DJ2DSprite::filterLinear);
	// Make the sprite blend with the background by adding the color values
	g_pBallSprite->SetBlendOp(DJ2DSprite::blendAdd);
	// Set the sprite to the center of the screen
	g_pBallSprite->SetPosition((float)g_nScreenWidth/2, (float)g_nScreenHeight/2);
	// Set the graphics of the sprite to be the flare graphics we loaded above
	g_pBallSprite->SetCurrentFrame(g_pBallSheet->GetFrame(0));

	// Add sprite to our sprite layer
	pLayer = theSpriteEngine.GetLayer(LAYER_SPRITES);
	pLayer->AddNode(g_pBallSprite);

	/* Tạo cp space*/
	g_pSpace = cpSpaceNew();
	g_pSpace->gravity = cpv(0, -10);
	/* not found cpSpaceSetGravity()*/

	/* Tạo ball trong cp space*/
	cpFloat radius = 20;
	cpFloat mass = 1;
	cpFloat moment = cpMomentForCircle(mass, 0, radius, cpvzero);
	g_pBallBody = cpSpaceAddBody(g_pSpace, cpBodyNew(mass, moment));
	//cpBodySetPos()
	g_pBallLinkerAgent = new LinkerAgent(g_pBallSprite, g_pSpace, g_pBallBody);
	cpShape* ballShape = cpSpaceAddShape(g_pSpace, cpCircleShapeNew(g_pBallBody, radius, cpvzero));
	ballShape->e = 1;
	ballShape->u = 1;
	ballShape->body = g_pBallBody;
	g_pBallLinkerAgent->UpdateFromEngine();
	/**/

	//Tạo thêm một thanh ngang
	g_pHorizontalBarSheet = DJ_NEW(DJ2DGraphicsSheet);
	if (!g_pHorizontalBarSheet->Init("sprites/horizontalBar.tga"))
	{
		DJError("Failed to load horizontal bar sheet!");
		return DJ_FAILURE;
	}
	vHotspot = DJVector2(200/2, 20/2);
	g_pHorizontalBarSheet->AutoGenFrames(0, 0, 200, 20, 1, 1, &vHotspot);
	theSpriteEngine.AddGraphicsSheet(g_pHorizontalBarSheet);
	g_pHorizontalBarSprite = DJ_NEW(DJ2DSprite);
	g_pHorizontalBarSprite->SetFilter(DJ2DSprite::filterLinear);
	g_pHorizontalBarSprite->SetBlendOp(DJ2DSprite::blendAdd);
	g_pHorizontalBarSprite->SetPosition((float)g_nScreenWidth/2, (float)g_nScreenHeight/2 + 200);
	g_pHorizontalBarSprite->SetCurrentFrame(g_pHorizontalBarSheet->GetFrame(0));
	pLayer->AddNode(g_pHorizontalBarSprite);

	g_pSpace->staticBody;
	
	// Load sound effects using the DJEngine gamesound helper functions (internally uses DJSoundDevice)
	if (!djGameSoundsInit(l_szSoundFiles, SOUND_COUNT))
	{
		DJError("Failed to load sound effects!");
		return DJ_FAILURE;
	}

	return DJ_SUCCESS;
}

///

void DJTutorial1Application::OnTerm()
{
	DJTrace("%s()", __FUNCTION__);

	// Delete the sprite we allocated in the OnInit
	DJ_SAFE_DELETE(g_pBallSprite);

	DJ_SAFE_RELEASE(g_pFont);

	g_pBallSheet = NULL;		// No need to delete this since the sprite engine takes care of it (we added it above)
	g_pBackgroundSheet = NULL;	// No need to delete this since the sprite engine takes care of it (we added it above)

	// Terminate sprite engine
	theSpriteEngine.Term();

	// Terminate DJEngine gamesound helper function
	djGameSoundsTerm();

}

///

void DJTutorial1Application::OnUpdate()
{
	//DJTrace(__FUNCTION__"()");
	
	// Update sprite engine
	theSpriteEngine.OnUpdate(GetDeltaAppTime());
}

///

void DJTutorial1Application::OnPaint()
{
	//DJTrace(__FUNCTION__"()");

	// Set the clear color
	//pTheRenderDevice->SetClearColor(DJColor(1,0,0,0));
	pTheRenderDevice->SetClearColor(DJColor(0,0,0,0));

	// Clear the screen
	pTheRenderDevice->Clear(DJRenderDevice::flagClearAll);
	// Disable the depth buffer (no need for it in 2D games usually)
	pTheRenderDevice->EnableZBuffer(0);
	
	// Set render context
	DJ2DRenderContext rc;
	rc.m_uFlags = 0;
	rc.m_cModColor = DJColor(1,1,1,1);
	rc.m_cAddColor = DJColor(0,0,0,0);
	rc.m_mLayerTransform = DJMatrix2D::Identity();
	rc.m_pLayer = NULL;
	rc.m_mTransform = DJMatrix2D::Identity();

	// Render sprites and all layers
	theSpriteEngine.OnPaint(rc);

	// Setup screen for rendering text
	pTheRenderDevice->SetViewTransform(DJMatrix::Identity());
	pTheRenderDevice->SetPerspectiveOrtho(0,0,(float)g_nScreenWidth,(float)g_nScreenHeight,0.0f,100.0f);
	g_pFont->DrawString("Tutorial1\nTouch screen to move sprite", DJVector3(10,10,0), DJVector2(16,16), 0xFFFFFFFF);
}

///
 
djint32 DJTutorial1Application::OnTouchBegin(djint32 nID, float x, float y)
{
	DJInfo("Touch Begin: %d %.2f %.2f", nID, x, y);

	if (DJApplication::OnTouchBegin(nID, x,y))
		return 1;

	djGameSoundPlay(SOUND_ZAP);

	if (g_pBallSprite)
		g_pBallSprite->SetPosition(x,y);

	return 0;
}

///

djint32 DJTutorial1Application::OnTouchMove(djint32 nID, float x, float y)
{
	DJInfo("Touch Move: %d %.2f %.2f", nID, x, y);

	if (DJApplication::OnTouchMove(nID, x,y))
		return 1;

	if (g_pBallSprite)
		g_pBallSprite->SetPosition(x,y);

	return 0;
}

///

djint32 DJTutorial1Application::OnTouchEnd(djint32 nID, float x, float y)
{
	DJInfo("Touch End: %d %.2f %.2f", nID, x, y);

	if (DJApplication::OnTouchEnd(nID, x,y))
		return 1;

	if (g_pBallSprite)
		g_pBallSprite->SetPosition(x,y);

	return 0;
}

///

djint32 DJTutorial1Application::OnAccelerate(float x, float y, float z)
{
	if (DJApplication::OnAccelerate(x,y,z))
		return 1;

	//DJInfo("%s %.2f %.2f %.2f", __FUNCTION__, x, y, z);
	return 0;
}

///

djint32 DJTutorial1Application::OnButtonDown(djint32 nKey)
{
	DJInfo("Button Down: %d", nKey);

	if (DJApplication::OnButtonDown(nKey))
		return 1;

	return 0;
}

///

djint32 DJTutorial1Application::OnButtonUp(djint32 nKey)
{
	DJInfo("Button Up: %d", nKey);

	if (DJApplication::OnButtonUp(nKey))
		return 1;

	return 0;
}

///

void DJTutorial1Application::OnMessage(djuint32 nMessage, djuint32 nParam1, djuint32 nParam2)
{
	//DJTrace(__FUNCTION__"()");
}
// Application class
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
DJ_FILE_END();
/////////////////////////////////////////////////////////////////

