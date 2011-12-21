#pragma once
#include <djapp.h>
#include <dj2d.h>
#include "effects.h"
#define __PLACEMENT_VEC_NEW_INLINE
#define __PLACEMENT_NEW_INLINE
#include <Box2D/Box2D.h>

class LinkerAgent
{
public:
	DJ2DSprite*	m_pSprite;
	b2Body* m_pBody;
	djbool	m_isSeed;
	DJ2DDynamicEffectNode* m_pEffectNode;
	LinkerAgent(void);
	~LinkerAgent(void);
	LinkerAgent(DJ2DSprite* pSprite, b2Body* pBody);
	void UpdateFromEngine();
	void UpdateFromBox2D();
	LinkerAgent(DJ2DSprite* pSprite, b2Body* pBody, djbool isSeed);
	LinkerAgent(DJ2DSprite* pSprite, b2Body* pBody, djbool isSeed, DJ2DDynamicEffectNode* pEffectNode);
};
