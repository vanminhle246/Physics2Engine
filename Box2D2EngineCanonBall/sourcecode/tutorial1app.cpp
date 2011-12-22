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
#include "LinkerAgent.h"
#include "B2DjSimulator.h"
#include "MyContactListener.h"
#include "effects.h"
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
DJ2DGraphicsSheet*		g_pSmileBallSheet		= NULL;
// Sheet of graphics for the background
DJ2DGraphicsSheet*		g_pBackgroundSheet		= NULL;
// Our test sprite
DJ2DSprite*				g_pBallSprite			= NULL;
// A font to use for drawing text
DJFont*					g_pFont					= NULL;

/*coded by Minh*/
DJ2DGraphicsSheet*		g_pHorizontalBarSheet	= NULL;
DJ2DSprite*				g_pBottomBarSprite		= NULL;
B2DjSimulator*			g_pSimulator			= NULL;
b2Body*					g_pBottomBarBody		= NULL;
LinkerAgent*			g_pBottomBarLinker		= NULL;
b2Body*					g_pBallBody				= NULL;
LinkerAgent*			g_pBallLinker			= NULL;
DJ2DSprite*				g_pTopBarSprite			= NULL;
b2Body*					g_pTopBarBody			= NULL;
LinkerAgent*			g_pTopBarLinker			= NULL;
DJ2DSprite*				g_pLeftBarSprite		= NULL;
b2Body*					g_pLeftBarBody			= NULL;
LinkerAgent*			g_pLeftBarLinker		= NULL;
DJ2DSprite*				g_pRightBarSprite		= NULL;
b2Body*					g_pRightBarBody			= NULL;
LinkerAgent*			g_pRightBarLinker		= NULL;
DJ2DGraphicsSheet*		g_pVerticalBarSheet		= NULL;
b2Body*					g_pVirtualGroundBody	= NULL;
b2MouseJoint*			g_pMouseJoint			= NULL;
DJLinkedList<LinkerAgent> g_bulletList;
DJ2DGraphicsSheet*		g_pCryBallSheet			= NULL;
MyContactListener		g_contactListenerInstance;
DJ2DSprite*				g_pCanonBaseSprite		= NULL;
DJ2DSprite*				g_pCanonSprite			= NULL;
DJDynamicEffectDesc*	g_pEffectDescription	= NULL;
DJ2DDynamicEffectNode*	g_pEffectNode			= NULL;
/**/

/////////////////////////////////////////////////////////////////
// List of sounds to load
static const char *l_szSoundFiles[SOUND_COUNT] =
{
	"sound/zap1.wav",
	"sound/tick.wav",
};
/////////////////////////////////////////////////////////////////

float CalculateDistanceToBase(const DJVector2& A, const DJVector2& B, const DJVector2& C, const float screenWidth)
{
	if (B.y() == A.y())
	{
		return ((C.x() - A.x())/(screenWidth - A.x()));
	}
	/*for explanation*
	float AH = B.y() - A.y();
	float AD = C.y() - A.y();
	float BH = A.x() - B.x();
	float ED = (AD)*(BH)/(AH);
	float DC = C.x() - A.x();
	float EC = ED + DC;
	return EC/screenWidth;
	/**/
	float EC = ((C.y() - A.y())*(A.x() - B.x())/(B.y() - A.y())) + (C.x() - A.x());
	return (EC/(screenWidth - C.x() + EC));
}

DJLinkedList<DJVector2>* GetLessAsteroid(DJLinkedList<DJVector2>* enemyPositions, const djint32 asteroidRadius, DJVector2 screenSize)
{
	DJLinkedList<DJVector2>* asteroidPositions = new DJLinkedList<DJVector2>();
	if (asteroidRadius <= 0) return asteroidPositions;
	djint32 asteroidDiameter = 2*asteroidRadius;
	int rowNumber = (int)djCeiling(screenSize.y() / asteroidDiameter);
	int columnNumber = (int)djCeiling(screenSize.x() / asteroidDiameter);
	DJDebug("row: %d  column: %d", rowNumber, columnNumber);
	int arraySize = rowNumber*columnNumber;
	int* markedArray = new int[arraySize];
	for (int i =0; i<arraySize; i++) markedArray[i] = 0;
	DJLinkedListIter<DJVector2> iter(enemyPositions);
	DJVector2* currentPosition;
	int currentColumn;
	int currentRow;
	while (currentPosition = iter.Get())
	{
		currentColumn = (int)djFloor(currentPosition->x() / asteroidDiameter);
		currentRow = (int)djFloor(currentPosition->y() / asteroidDiameter);
		markedArray[currentRow*columnNumber + currentColumn] = 1;
		iter.Next();
	}
	int asteroidCount = 0;
	for (int i =0; i<arraySize; i++)
	{
		if (markedArray[i])
		{
			asteroidCount++;
			currentColumn = i % columnNumber;
			currentRow = i / columnNumber;
			DJDebug("%d: [%d, %d]", asteroidCount, currentRow, currentColumn);
			DJVector2* currentAsteroid = new DJVector2(currentColumn*asteroidDiameter + asteroidRadius, currentRow*asteroidDiameter + asteroidRadius);
			DJDebug("%d: [%f, %f]", asteroidCount, currentAsteroid->x(), currentAsteroid->y());
			asteroidPositions->AddLast(currentAsteroid);
		}
	}
	/**
	DJLinkedListIter<DJVector2> iter1(asteroidPositions);
	asteroidCount = 0;
	while (currentPosition = iter1.Get())
	{
		asteroidCount++;
		DJDebug("asteroid%d [%.3f %.3f]", asteroidCount, (float)currentPosition->x(), (float)currentPosition->y());
		iter1.Next();
	}
	/**/
	DJ_SAFE_DELETE_ARRAY(markedArray);
	//delete []markedArray;
	return asteroidPositions;
}
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
	g_pSmileBallSheet = DJ_NEW(DJ2DGraphicsSheet);
	if (!g_pSmileBallSheet->Init("sprites/ball1.tga"))
	{
		DJError("Failed to load ball sheet!");
		return DJ_FAILURE;
	}
	DJVector2 vHotspot = DJVector2(40/2, 40/2); //Điểm neo trong hình. vị trí ở giữa.
	g_pSmileBallSheet->AutoGenFrames(0, 0, 40, 40, 1, 1, &vHotspot);
	theSpriteEngine.AddGraphicsSheet(g_pSmileBallSheet);

	g_pCryBallSheet = DJ_NEW(DJ2DGraphicsSheet);
	if (!g_pCryBallSheet->Init("sprites/ball2.tga"))
	{
		DJError("Failed to load ball sheet!");
		return DJ_FAILURE;
	}
	vHotspot = DJVector2(40/2, 40/2); //Điểm neo trong hình. vị trí ở giữa.
	g_pCryBallSheet->AutoGenFrames(0, 0, 40, 40, 1, 1, &vHotspot);
	theSpriteEngine.AddGraphicsSheet(g_pCryBallSheet);

	/**
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
	DJ2DGraphicsFrame *pFrame = g_pBackgroundSheet->GetFrame(0);
	//pLayer->SetBackground(pFrame);
	//pLayer->SetPosition(0,0);
	/**/
	DJ2DLayer *pLayer = theSpriteEngine.GetLayer(LAYER_BACKGROUND);	

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
	g_pBottomBarSprite = DJ_NEW(DJ2DSprite);
	g_pBottomBarSprite->SetFilter(DJ2DSprite::filterLinear);
	g_pBottomBarSprite->SetBlendOp(DJ2DSprite::blendAdd);
	g_pBottomBarSprite->SetPosition((float)g_nScreenWidth/2, (float)g_nScreenHeight/2 + 200);
	g_pBottomBarSprite->SetCurrentFrame(g_pHorizontalBarSheet->GetFrame(0));
	pLayer->AddNode(g_pBottomBarSprite);
	
	b2Vec2	vectorGravity(0.0f, -100.0f);
	g_pSimulator = new B2DjSimulator();
	g_pSimulator->m_world->SetGravity(vectorGravity);
	g_pSimulator->m_world->SetContactListener(&g_contactListenerInstance);
	b2BodyDef staticBodyDefinition;
	staticBodyDefinition.position.Set(0, 0);
	g_pBottomBarBody = g_pSimulator->m_world->CreateBody(&staticBodyDefinition);
	g_pBottomBarSprite->SetScale(4, 1);
	b2PolygonShape horizontalBarShape;
	horizontalBarShape.SetAsBox(400.0f, 10.0f);
	g_pBottomBarBody->CreateFixture(&horizontalBarShape, 0.0f);
	g_pBottomBarLinker = new LinkerAgent(g_pBottomBarSprite, g_pBottomBarBody);
	g_pBottomBarLinker->UpdateFromEngine();	

	/**
	g_pCanonBaseSprite = DJ_NEW(DJ2DSprite);
	g_pCanonBaseSprite->SetFilter(DJ2DSprite::filterLinear);
	g_pCanonBaseSprite->SetBlendOp(DJ2DSprite::blendAdd);
	g_pCanonBaseSprite->SetPosition(100, g_pBottomBarSprite->GetPosition().y()-20);
	g_pCanonBaseSprite->SetCurrentFrame(g_pCryBallSheet->GetFrame(0));
	pLayer = theSpriteEngine.GetLayer(LAYER_OVERLAY);
	pLayer->AddNode(g_pCanonBaseSprite);

	g_pCanonSprite = DJ_NEW(DJ2DSprite);
	g_pCanonSprite->SetFilter(DJ2DSprite::filterLinear);
	g_pCanonSprite->SetBlendOp(DJ2DSprite::blendAdd);
	g_pCanonSprite->SetPosition(100, 100);
	g_pCanonSprite->SetCurrentFrame(g_pHorizontalBarSheet->GetFrame(0));
	pLayer = theSpriteEngine.GetLayer(LAYER_OVERLAY);
	pLayer->AddNode(g_pCanonSprite);
	/**/
	
	/**/
	for (djint i = 0; i<1; i++)	
	{
		// Create a sprite we can later draw
		g_pBallSprite = DJ_NEW(DJ2DSprite);
		// Make the sprite magnify with a blur instead of pixelated
		g_pBallSprite->SetFilter(DJ2DSprite::filterLinear);
		// Make the sprite blend with the background by adding the color values
		g_pBallSprite->SetBlendOp(DJ2DSprite::blendAdd);
		// Set the sprite to the center of the screen
		float x = djRandomGetFloatInRange(0, (float)g_nScreenWidth);
		float y = djRandomGetFloatInRange(0, (float)g_nScreenHeight);
		g_pBallSprite->SetPosition((float)g_nScreenWidth/2, (float)g_nScreenHeight/2);
		//g_pBallSprite->SetPosition(x, y);
		// Set the graphics of the sprite to be the flare graphics we loaded above
		g_pBallSprite->SetCurrentFrame(g_pCryBallSheet->GetFrame(0));
		// Add sprite to our sprite layer
		pLayer = theSpriteEngine.GetLayer(LAYER_SPRITES);
		pLayer->AddNode(g_pBallSprite);
		g_pBallSprite->SetScale(0.5, 0.5);

		b2BodyDef ballDefinition;
		ballDefinition.type = b2_dynamicBody;
		ballDefinition.position.Set(0.0f, 0.0f);
		g_pBallBody = g_pSimulator->m_world->CreateBody(&ballDefinition);
		b2CircleShape dynamicBall;
		dynamicBall.m_p.Set(0, 0);
		dynamicBall.m_radius = 10;
		b2FixtureDef ballFixtureDefinition;
		ballFixtureDefinition.shape = &dynamicBall;
		ballFixtureDefinition.density = 0.5;
		ballFixtureDefinition.restitution = 0.75;
		g_pBallBody->CreateFixture(&ballFixtureDefinition);
		g_pBallLinker = new LinkerAgent(g_pBallSprite, g_pBallBody, DJFALSE, g_pEffectNode);
		g_pBallLinker->UpdateFromEngine();
		g_bulletList.AddLast(g_pBallLinker);
	}
	/**/

	/**/
	g_pEffectDescription = (DJDynamicEffectDesc*)DJDynamicEffectDesc::CreateResource("effects/smoketrail.effect", 0);
	g_pEffectNode = DJ_NEW(DJ2DDynamicEffectNode);
	g_pEffectNode->InitEffect(g_pEffectDescription);
	g_pEffectNode->SetPosition(100, 100);
	theSpriteEngine.AddNode(g_pEffectNode, LAYER_OVERLAY);
	
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
	DJ_SAFE_DELETE(g_pBottomBarSprite);
	DJ_SAFE_DELETE(g_pTopBarSprite);
	DJ_SAFE_DELETE(g_pLeftBarSprite);
	DJLinkedListIter<LinkerAgent> iter(g_bulletList);
	LinkerAgent* linkerAgent;
	while (linkerAgent = iter.Get())
	{
		DJ_SAFE_DELETE(linkerAgent->m_pSprite);
		g_pSimulator->m_world->DestroyBody(linkerAgent->m_pBody);
		iter.Next();
	}
	delete g_pSimulator->m_world;
	DJ_SAFE_RELEASE(g_pFont);

	g_pSmileBallSheet = NULL;		// No need to delete this since the sprite engine takes care of it (we added it above)
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
	float32 timeStep = 1.0f/m_fFPS;
	g_pSimulator->Step(timeStep);
	//g_pBallLinker->UpdateFromBox2D();
	DJLinkedListIter<LinkerAgent> iter(g_bulletList);
	LinkerAgent* linkerAgent;
	while (linkerAgent = iter.Get())
	{
		//if (linkerAgent->m_pEffectNode) linkerAgent->m_pEffectNode->OnUpdate(GetDeltaAppTime());
		linkerAgent->UpdateFromBox2D();
		iter.Next();
	}
	g_pEffectNode->OnUpdate(GetDeltaAppTime());
	theSpriteEngine.OnUpdate(GetDeltaAppTime());
}

///

void DJTutorial1Application::OnPaint()
{
	//DJTrace(__FUNCTION__"()");

	// Set the clear color
	//Đặt màu nền
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
	g_pFont->DrawString("Canon balls\nClick your mouse to shoot", DJVector3(10,10,0), DJVector2(16,16), 0xFFFFFFFF);
}

///
 
djint32 DJTutorial1Application::OnTouchBegin(djint32 nID, float x, float y)
{
	DJInfo("Touch Begin: %d %.2f %.2f", nID, x, y);

	if (DJApplication::OnTouchBegin(nID, x,y))
		return 1;

	//djGameSoundPlay(SOUND_ZAP); turn off sound

	//if (g_pBallSprite) g_pBallSprite->SetPosition(x,y);
	b2Vec2 touchingPoint = b2Vec2(x, -y);
	g_pSimulator->MouseDown(touchingPoint);
	g_pEffectNode->SetPosition(x, y);
	return 0;
}

///

djint32 DJTutorial1Application::OnTouchMove(djint32 nID, float x, float y)
{
	DJInfo("Touch Move: %d %.2f %.2f", nID, x, y);

	if (DJApplication::OnTouchMove(nID, x,y))
		return 1;
	b2Vec2 touchingPoint = b2Vec2(x, -y);
	//if (g_pBallSprite) g_pBallSprite->SetPosition(x,y);
	g_pSimulator->MouseMove(touchingPoint);
	return 0;
}

///

djint32 DJTutorial1Application::OnTouchEnd(djint32 nID, float x, float y)
{
	DJInfo("Touch End: %d %.2f %.2f", nID, x, y);

	if (DJApplication::OnTouchEnd(nID, x,y))
		return 1;

	//if (g_pBallSprite) g_pBallSprite->SetPosition(x,y);
	b2Vec2 touchingPoint = b2Vec2(x, -y);
	g_pSimulator->MouseUp(touchingPoint);
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
	/**
	DJVector2 A(5, 0);
	DJVector2 B(0, 5);
	DJVector2 C(3, 4);
	float scrWidth = 5;
	float d = CalculateDistanceToBase(A, B, C, scrWidth);
	DJInfo("Button Down: %f : ", d);
	/**/
	FILE *fpt = fopen("AsteroidTest.txt", "r");
	float readingValue = 0;
	DJLinkedList<DJVector2>* enemyPositions = new DJLinkedList<DJVector2>();
	djint32 asteroidRadius;
	int enemyNumber;
	DJVector2 screenSize;
	float x, y;
	if (fpt)
	{
		fscanf(fpt,"%f", &readingValue);
		asteroidRadius = (int) readingValue;
		fscanf(fpt,"%f", &readingValue);
		enemyNumber = (int) readingValue;
		fscanf(fpt,"%f %f", &x, &y);
		screenSize = DJVector2(x, y);
		for (int i=0; i<enemyNumber; i++)
		{
			fscanf(fpt,"%f %f", &x, &y);
			DJVector2* enemyPosition = new DJVector2(x, y);
			enemyPositions->AddLast(enemyPosition);
			//DJDebug("enemy position %f %f", x, y);	
		}
		fclose(fpt);
	}
	else
	{
		DJDebug("Cannot read file AsteroidTest.txt");
	}
	DJLinkedList<DJVector2>* asteroidPositions;
	asteroidPositions = GetLessAsteroid(enemyPositions, asteroidRadius, screenSize);
	/**/
	DJLinkedListIter<DJVector2> iter(asteroidPositions);
	DJVector2* currentPosition;
	int asteroidCount = 0;
	while (currentPosition = iter.Get())
	{
		asteroidCount++;
		DJDebug("asteroid%d [%.3f %.3f]", asteroidCount, (float)currentPosition->x(), (float)currentPosition->y());
		iter.Next();
	}
	/**/
	DJInfo("Button Down: %d test : ", nKey);
	//DJInfo("my FPS: %f", m_fFPS);
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

