#pragma once
#define __PLACEMENT_VEC_NEW_INLINE
#define __PLACEMENT_NEW_INLINE
#include <Box2D/Box2D.h>

class MyContactListener :
	public b2ContactListener
{
public:
	MyContactListener(void);
	~MyContactListener(void);
	void BeginContact(b2Contact* contact);
	void EndContact(b2Contact* contact);
};
