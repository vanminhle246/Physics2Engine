
#include <djapp.h>
#include <dj2d.h>
#define __PLACEMENT_VEC_NEW_INLINE
#define __PLACEMENT_NEW_INLINE
#include <Box2D/Box2D.h>
class HybridEntity
{
public:
	DJ2DSprite*	m_pSprite;
	b2World* m_pWorld;
	b2Body* m_pBody;
	HybridEntity(void);
	~HybridEntity(void);
	void SetPositionFromEngine(float x, float y);
	void UpdateFromEngine();
	void UpdateFromBox2D();
	HybridEntity(DJ2DSprite* pSprite, b2World* pWord, b2Body* pBody);
};
