#include "B2DJQueryCallback.h"

B2DJQueryCallBack::B2DJQueryCallBack(void)
{
}

B2DJQueryCallBack::~B2DJQueryCallBack(void)
{
}
B2DJQueryCallBack::B2DJQueryCallBack(const b2Vec2& point)
{
	m_point = point;
	m_fixture = NULL;
}
bool B2DJQueryCallBack::ReportFixture(b2Fixture* fixture)
{
	b2Body* body = fixture->GetBody();
	if (body->GetType() == b2_dynamicBody)
	{
		bool inside = fixture->TestPoint(m_point);
		if (inside)
		{
			m_fixture = fixture;
			return false;  //terminate the query
		}
	}
	return true; //continue the query
}