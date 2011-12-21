/////////////////////////////////////////////////////////////////
//
// effects.h
// Effects features, functions and classes
//
/////////////////////////////////////////////////////////////////
//
// Copyright (C) 2009 Daniel Jeppsson
// All Rights Reserved. These instructions, statements, computer
// programs, and/or related material (collectively, the "Source")
// contain unpublished information propietary to Daniel Jeppsson
// which is protected by US federal copyright law and by 
// international treaties. This Source may NOT be disclosed to 
// third parties, or be copied or duplicated, in whole or in 
// part, without the written consent of Daniel Jeppsson.
//
/////////////////////////////////////////////////////////////////
// 
// Author: Daniel Jeppsson
// 
/////////////////////////////////////////////////////////////////

#ifndef _EFFECTS_H
#define _EFFECTS_H

#include <djengine.h>
#include <dj2d.h>
#include <djrenderdevice.h>
#include <djresourcemanager.h>
#include <djtagfile.h>


/////////////////////////////////////////////////////////////////
struct DJEffectVertexPS
{
	float				fX;
	float				fY;
	float				fSize;
	float				fCol[4];
	float				fRot;
};
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
struct DJEffectVertex
{
	float				fX;
	float				fY;
	float				fCol[4];
	float				fU;
	float				fV;
};
/////////////////////////////////////////////////////////////////

void InitParticleSystem();
void TermParticleSystem();

/////////////////////////////////////////////////////////////////
struct DJEffectParticle
{
	DJEffectParticle*	pNext;
	DJVector3			vPos;
	DJVector3			vPrevPos;
	DJVector3			vDir;

	djint32				nLife;

	float				fRot;
	float				fDeltaRot;
	
	DJColor				cCol;
	DJColor				cPrevCol;
	DJColor				cDeltaCol;

	float				fSize;
	float				fPrevSize;
	float				fDeltaSize;

	//djint32				nRand;

	//float			m_fInitialLifeTime;
	//float			m_vInitialVel[2];
	//float			m_vInitialPos[2];
	//float			m_cInitialCol[4];
};
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
class DJDynamicEffectDesc : public DJResource, public DJPackable
{
	IMPLEMENT_INTERFACE(DynamicEffectDesc, Resource)

public:
	// Material for effect
	DJMaterial*			m_pMat;
	// Material for effect if pointsprites are supported
	DJMaterial*			m_pMatPS;
	// Texture overload
	DJTexture*			m_pTexOverload;
	// Flags
	djuint32			m_uFlags;
	// Initial force constant
	DJVector3			m_vForce;
	// Initial Yaw / Yaw variance
	float				m_fYaw;
	float				m_fYawVar;
	// Initial Pitch / Pitch variance
	float				m_fPitch;
	float				m_fPitchVar;
	// Initial Speed / Speed variance
	float				m_fSpeed;
	float				m_fSpeedVar;
	// Maximum number of active particles
	djint32				m_nMaxParticles;
	// Number of particles emitted per frame / Variance
	float				m_fEmitsPerFrame;
	float				m_fEmitVar;
	// Life counter (number of frames) / Variance
	djint32				m_nLife;
	djint32				m_nLifeVar;
	// Initial particle color / Variance
	DJColor				m_cStartColor;
	DJColor				m_cStartColorVar;
	// End particle color / Variance
	DJColor				m_cEndColor;
	DJColor				m_cEndColorVar;
	// Starting particle size / Variance
	float				m_fStartSize;
	float				m_fStartSizeVar;
	// End size of particle / Variance
	float				m_fEndSize;
	float				m_fEndSizeVar;
	// Rotation of particle / Variance
	float				m_fRot;
	float				m_fRotVar;
	// Rotation speed of particle / Variance
	float				m_fRotSpeed;
	float				m_fRotSpeedVar;
	// Source blend mode
	DJBLENDMODE			m_uSrcBlendMode;
	// Destination blend mode
	DJBLENDMODE			m_uDstBlendMode;

public:
	// Constructor
	DJDynamicEffectDesc();
	// Destructor
	~DJDynamicEffectDesc();

	/////////////////////////////////////////////////////////////////
	// DJPackable virtual calls

	/// Pre-calculate size of packed data (might be recursive)
	virtual djuint32 GetPackedSize(const DJBinaryBuffer &buffer) const;
	/// Pack object / data (recursively if necessary) into a buffer
	virtual PACKRESULT Pack(DJBinaryBuffer &buffer) const;
	/// Unpack object / data (recursively if necessary) into a buffer
	virtual PACKRESULT Unpack(DJBinaryBuffer &buffer);

	/////////////////////////////////////////////////////////////////
	// ASCII virtual calls

	/// Load from a tagfile
	virtual djint32 LoadAscii(DJTagFile &file, DJTagDir *pDir);
	/// Save to a tagfile
	virtual djint32 SaveAscii(DJTagFile &file, DJTagDir *pDir) const;

	/////////////////////////////////////////////////////////////////
	// DJResource virtual calls

	/// Create function - Each resource need to implement this
	static DJResource* CreateResource(const char *szName, djuint32 uFlags);
	/// Virtual reload function - Each resource need to implement this
	virtual djresult OnResourceReload();

	/// Load and create effect with specified name / path
	static DJDynamicEffectDesc* CreateDynamicEffectDesc(const char *lpszName);
};
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
class DJDynamicEffect
{
protected:

public:
	// Dynamic VBO
	DJVertexBuffer*				m_pVB;
	// Indexbuffer (if not pointsprites)
	DJIndexBuffer*				m_pIB;
	// Number of active particles
	djint32						m_nActiveCount;
	// Active particles
	DJEffectParticle*			m_pActiveParticles;

	// Uses pointsprites
	djbool						m_bUsePointSprites;

	// Emit counter
	float						m_fEmitCounter;

	// Effect descriptor
	const DJDynamicEffectDesc*	m_pEffectDesc;

	// Constructor
	DJDynamicEffect();
	// Destructor
	virtual ~DJDynamicEffect();

	// Initialize effect
	djbool InitEffect(const DJDynamicEffectDesc * pEffectDesc);
	// Terminate effect
	djbool TermEffect();

	// Add new particle
	djbool AddParticle(const DJVector3 & vPos);
	// Update effect
	void UpdateEffect(const DJVector3 & vPos);
	// Update vertex buffers
	void UpdateEffectBuffers();
	// Render effect
	void RenderEffect();
};
/////////////////////////////////////////////////////////////////
extern djuint32				g_nScreenWidth;
extern djuint32				g_nScreenHeight;
/////////////////////////////////////////////////////////////////
class DJ2DDynamicEffectNode : public DJDynamicEffect, public DJ2DNode
{
protected:

public:
	
	DJ2DDynamicEffectNode() : DJDynamicEffect(), DJ2DNode()
	{
	}

	virtual ~DJ2DDynamicEffectNode()
	{
		// Remove sprite if it is active (slow)
		// FIXME
		theSpriteEngine.RemoveNode(this);
	}

	virtual djbool OnUpdate(float fDeltaTime)
	{
		DJMatrix2D tm2D;
		RecalculateTransform(DJTRUE);
		GetWorldTransform(tm2D);
		DJVector2 vPos = tm2D * m_vPos;

		UpdateEffect(DJVector3(vPos[0], vPos[1], 0.0f));
		return DJTRUE;
	}

	virtual void OnPaint(const DJ2DRenderContext &rc)
	{
		theGraphicsBatchHandler.OnPaint();
	
		pTheRenderDevice->SetWorldTransform(DJMatrix::Identity());
		pTheRenderDevice->SetViewTransform(DJMatrix::Identity());
		pTheRenderDevice->SetPerspectiveOrtho(0.0f, 0.0f, (float)g_nScreenWidth, (float)g_nScreenHeight, 0.0f, 1.0f);
		//pTheRenderDevice->SetPerspectiveIdentity();

		DJMatrix tm, tm2;
		DJMatrix2D tm2D;
		GetLocalTransform(tm2D);
		tm2D = rc.m_mTransform * tm2D;
		tm2D.GetMatrix(&tm);
		//rc.m_pLayer->GetLocalTransform();
		rc.m_mLayerTransform.GetMatrix(&tm2);
		//pTheRenderDevice->SetWorldTransform(tm);
		//rc.m_mLayerTransform.GetMatrix(&tm);
		pTheRenderDevice->SetViewTransform(tm2);

	//	rc.m_m
		//pTheRenderDevice->SetPerspectiveOrtho(0.0f, 0.0f, 854.0f, 480.0f, 0.0f, 1.0f);
		//pTheRenderDevice->SetViewTransform(DJMatrix::Identity());
		//pTheRenderDevice->SetWorldTransform(DJMatrix::Identity());
		
		UpdateEffectBuffers();
		RenderEffect();
	}
};
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////

#endif
