#pragma once
#include <Box2D/BOX2D.h>

class B2DJQueryCallBack :
	public b2QueryCallback
{
public:
	b2Vec2 m_point;
	b2Fixture* m_fixture;
	B2DJQueryCallBack(void);
	~B2DJQueryCallBack(void);
	B2DJQueryCallBack(const b2Vec2& point);
	bool ReportFixture(b2Fixture* fixture);
};
