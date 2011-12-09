#include "LinkerAgent.h"

LinkerAgent::LinkerAgent(void)
{
}

LinkerAgent::~LinkerAgent(void)
{
}

LinkerAgent::LinkerAgent(DJ2DSprite* pSprite, cpSpace* pSpace, cpBody* pBody)
{
	m_pSprite = pSprite;
	m_pSpace = pSpace;
	m_pBody = pBody;
}
void LinkerAgent::UpdateFromEngine()
{
	DJVector2 djPosition = m_pSprite->GetPosition();
	float djAngle = m_pSprite->GetRotation();
	cpVect cpPosition;
	cpPosition.x = (float) djPosition.x();
	cpPosition.y = (float) -djPosition.y();
	cpBodySetAngle(m_pBody, djAngle);
	cpBodySetPos(m_pBody, cpPosition);
}
void LinkerAgent::UpdateFromChipmunk()
{
	float angle = (float)cpBodyGetAngle(m_pBody);
	cpVect cpPosition = cpBodyGetPos(m_pBody);
	m_pSprite->SetPosition((float)cpPosition.x, (float)-cpPosition.y);
	m_pSprite->SetRotation(angle);
}