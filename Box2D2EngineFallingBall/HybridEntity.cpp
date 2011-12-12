#include "HybridEntity.h"

HybridEntity::HybridEntity(void)
{

}

HybridEntity::~HybridEntity(void)
{
}

void HybridEntity::SetPositionFromEngine(float x, float y)
{
	m_pSprite->SetPosition(x, y);
	UpdateFromEngine();
}

void HybridEntity::UpdateFromEngine()
{
	DJVector2 djPosition = m_pSprite->GetPosition();
	float djAngle = m_pSprite->GetRotation();
	//m_pBody->SetTransform(b2Vec2(djPosition.x(), -djPosition.y()), djAngle);
	//m_pBody->SetTransform(b2Vec2(1, 1), 20);
	//m_pBody->SetLinearVelocity(b2Vec2(1, 1));
}
void HybridEntity::UpdateFromBox2D()
{
	b2Vec2 b2dPosition = m_pBody->GetPosition();
	m_pSprite->SetPosition(b2dPosition.x, -b2dPosition.y);
	m_pSprite->SetRotation(m_pBody->GetAngle());
}
HybridEntity::HybridEntity(DJ2DSprite* pSprite, b2World* pWord, b2Body* pBody)
{
	m_pSprite = pSprite;
	m_pWorld = pWord;
	m_pBody = pBody;
}