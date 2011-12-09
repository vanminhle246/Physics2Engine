#pragma once
#include <djapp.h>
#include <dj2d.h>
#include <chipmunk.h>

class LinkerAgent
{
public:
	DJ2DSprite*	m_pSprite;
	cpSpace* m_pSpace;
	cpBody* m_pBody;
	LinkerAgent(void);
	~LinkerAgent(void);
	LinkerAgent(DJ2DSprite* pSprite, cpSpace* pSpace, cpBody* pBody);
	void UpdateFromEngine();
	void UpdateFromChipmunk();
};
