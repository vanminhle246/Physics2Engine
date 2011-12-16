#include "LinkerAgent.h"

LinkerAgent::LinkerAgent(void)
{
}

LinkerAgent::~LinkerAgent(void)
{
}
LinkerAgent::LinkerAgent(DJ2DSprite* pSprite, b2Body* pBody)
{
	m_pSprite = pSprite;
	m_pBody = pBody;
	m_isSeed = DJFALSE;
	m_pBody->SetUserData(this);
}
void LinkerAgent::UpdateFromEngine()
{
	DJVector2 djPosition = m_pSprite->GetPosition();
	float djAngle = m_pSprite->GetRotation();
	m_pBody->SetTransform(b2Vec2(djPosition.x(), -djPosition.y()), -djAngle);
}
void LinkerAgent::UpdateFromBox2D()
{
	b2Vec2 b2dPosition = m_pBody->GetPosition();
	m_pSprite->SetPosition(b2dPosition.x, -b2dPosition.y);
	m_pSprite->SetRotation(-m_pBody->GetAngle());
}
LinkerAgent::LinkerAgent(DJ2DSprite* pSprite, b2Body* pBody, djbool isSeed)
{
	m_pSprite = pSprite;
	m_pBody = pBody;
	m_isSeed = isSeed;
	m_pBody->SetUserData(this);
}
