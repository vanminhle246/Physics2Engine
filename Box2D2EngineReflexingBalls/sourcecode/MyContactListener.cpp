#include "MyContactListener.h"
#include "LinkerAgent.h"
void HandleContact(LinkerAgent* la1, LinkerAgent* la2)
{
	/**/
	djbool isSeed = la1->m_isSeed;
	la1->m_isSeed = la2->m_isSeed;
	la2->m_isSeed = isSeed;
	/**/
	/**
	if (la1->m_isSeed) la2->m_isSeed = la1->m_isSeed;
	if (la2->m_isSeed) la1->m_isSeed = la2->m_isSeed;
	/**/
}

MyContactListener::MyContactListener(void)
{
}

MyContactListener::~MyContactListener(void)
{
}

void MyContactListener::BeginContact(b2Contact* contact)
{
	b2Body* body1 = contact->GetFixtureA()->GetBody();
	b2Body* body2 = contact->GetFixtureB()->GetBody();
	if ((body1->GetType() == b2_dynamicBody) && (body2->GetType() == b2_dynamicBody))
	{
		void* linkerAgent1 = body1->GetUserData();
		void* linkerAgent2 = body2->GetUserData();
		if (linkerAgent1 && linkerAgent2)
			HandleContact(static_cast<LinkerAgent*>(linkerAgent1), static_cast<LinkerAgent*>(linkerAgent2));
	}
}

void MyContactListener::EndContact(b2Contact* contact)
{
}