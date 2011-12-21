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
	m_pEffectNode = NULL;
	m_pBody->SetUserData(this);
}
void LinkerAgent::UpdateFromEngine()
{
	DJVector2 djPosition = m_pSprite->GetPosition();
	float djAngle = m_pSprite->GetRotation();
	m_pBody->SetTransform(b2Vec2(djPosition.x(), -djPosition.y()), -djAngle);
	if (m_pEffectNode) m_pEffectNode->SetPosition(djPosition.x(), djPosition.y());
}
void LinkerAgent::UpdateFromBox2D()
{
	b2Vec2 b2dPosition = m_pBody->GetPosition();
	m_pSprite->SetPosition(b2dPosition.x, -b2dPosition.y);
	m_pSprite->SetRotation(-m_pBody->GetAngle());
	if (m_pEffectNode) m_pEffectNode->SetPosition(b2dPosition.x, -b2dPosition.y);
}
LinkerAgent::LinkerAgent(DJ2DSprite* pSprite, b2Body* pBody, djbool isSeed)
{
	m_pSprite = pSprite;
	m_pBody = pBody;
	m_isSeed = isSeed;
	m_pEffectNode = NULL;
	m_pBody->SetUserData(this);
}
LinkerAgent::LinkerAgent(DJ2DSprite* pSprite, b2Body* pBody, djbool isSeed, DJ2DDynamicEffectNode* pEffectNode)
{
	m_pSprite = pSprite;
	m_pBody = pBody;
	m_isSeed = isSeed;
	m_pEffectNode = pEffectNode;
	m_pBody->SetUserData(this);
}