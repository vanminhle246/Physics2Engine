/////////////////////////////////////////////////////////////////
//
// effects.cpp
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

#include "effects.h"


/////////////////////////////////////////////////////////////////
DJEffectParticle*	g_pEffectPool = NULL;
DJ2DGraphicsSheet*	g_pFlareSheet;
/////////////////////////////////////////////////////////////////

extern DJBLENDMODE djBlendModeFromName(const char * lpszBlendMode);

static DJVertexBuffer::DJVERTEX_DESC DJEffectVertexPS_Descs[] =
{
	DJVertexBuffer::DJVERTEX_DESC(0, DJVertexBuffer::typeFloat2, "pos_attr", INDEX_POS_ATTR, 0),
	DJVertexBuffer::DJVERTEX_DESC(8, DJVertexBuffer::typeFloat, "psize_attr", INDEX_PSIZE_ATTR, 0),
	DJVertexBuffer::DJVERTEX_DESC(12, DJVertexBuffer::typeFloat4, "col_attr", INDEX_COL_ATTR, DJVertexBuffer::vflagNormalize),
	DJVertexBuffer::DJVERTEX_DESC(28, DJVertexBuffer::typeFloat, "uv_attr", INDEX_UV_ATTR, 0),
};
#define DJEffectVertexPS_DescCount 4
static DJVertexBuffer::DJVERTEX_DESC DJEffectVertex_Descs[] =
{
	DJVertexBuffer::DJVERTEX_DESC(0, DJVertexBuffer::typeFloat2, "pos_attr", INDEX_POS_ATTR, 0),
	DJVertexBuffer::DJVERTEX_DESC(8, DJVertexBuffer::typeFloat4, "col_attr", INDEX_COL_ATTR, DJVertexBuffer::vflagNormalize),
	DJVertexBuffer::DJVERTEX_DESC(24, DJVertexBuffer::typeFloat2, "uv_attr", INDEX_UV_ATTR, 0),
};
#define DJEffectVertex_DescCount 3

/////////////////////////////////////////////////////////////////
void InitParticleSystem(djint32 nCount)
{
	for (djint32 q=0; q<nCount; q++)
	{
		DJEffectParticle* pPart = DJ_NEW(DJEffectParticle);
		pPart->pNext = g_pEffectPool;
		g_pEffectPool = pPart;
	}
}
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
void TermParticleSystem()
{
	while (g_pEffectPool)
	{
		DJEffectParticle *pPart = g_pEffectPool;
		g_pEffectPool = g_pEffectPool->pNext;
		delete pPart;
	}
}
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
static DJEffectParticle* GetParticle()
{
	DJEffectParticle *pPart = g_pEffectPool;
	if (g_pEffectPool)
	{
		g_pEffectPool = g_pEffectPool->pNext;
	}
	else
	{
		InitParticleSystem(100);
		pPart = g_pEffectPool;
		g_pEffectPool = g_pEffectPool->pNext;
	}
	DJAssert(pPart != NULL);
	return pPart;
}
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
static void ReturnParticle(DJEffectParticle *pPart)
{
	DJAssert(pPart != NULL);
	pPart->pNext = g_pEffectPool;
	g_pEffectPool = pPart;
}
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
DJDynamicEffectDesc::DJDynamicEffectDesc() : DJResource(DJResource::TYPE_EFFECT_DESC)
{
	m_nMaxParticles		= 50;
	m_uFlags			= 0;
	m_vForce			= DJVector3(0.000f, 0.01f, 0.0f);
	m_fYaw				= 0.0f;
	m_fYawVar			= DJRADIANS(0);
	m_fPitch			= 0.0f;
	m_fPitchVar			= DJRADIANS(0);
	m_fSpeed			= 1.1f;
	m_fSpeedVar			= 0.5f;
	m_fEmitsPerFrame	= 3.0f;
	m_fEmitVar			= 1.0f;
	m_nLife				= 40;
	m_nLifeVar			= 30;
	m_cStartColor		= DJColor(1.0f,1.0f,1.0f,0.5f);
	m_cStartColorVar	= DJColor(0.1f,0.1f,0.1f,0.0f);
	m_cEndColor			= DJColor(1,1,1,0);
	m_cEndColorVar		= DJColor(0.1f,0.1f,0.1f,0);
	m_fStartSize		= 10.0f;
	m_fEndSize			= 40.0f;
	m_fStartSizeVar		= 10.0f;
	m_fEndSizeVar		= 30.0f;
	m_pMat				= NULL;
	m_pMatPS			= NULL;
	m_pTexOverload		= NULL;
	m_fRot				= 0.0f;
	m_fRotVar			= DJRADIANS(180);
	m_fRotSpeed			= DJRADIANS(0);
	m_fRotSpeedVar		= DJRADIANS(1);
	m_uSrcBlendMode		= DJBLEND_SRCALPHA;
	m_uDstBlendMode		= DJBLEND_INVSRCALPHA;
}

///

DJDynamicEffectDesc::~DJDynamicEffectDesc()
{
	DJ_SAFE_RELEASE(m_pTexOverload);
	DJ_SAFE_RELEASE(m_pMat);
	DJ_SAFE_RELEASE(m_pMatPS);
}

///

struct EffectDescSave
{
	char				m_szMaterialName[64];
	char				m_szMaterialPSName[64];
	char				m_szTexName[64];
	djuint32			m_uFlags;
	DJVector3			m_vForce;
	float				m_fYaw;
	float				m_fYawVar;
	float				m_fPitch;
	float				m_fPitchVar;
	float				m_fSpeed;
	float				m_fSpeedVar;
	djint32					m_nMaxParticles;
	float				m_fEmitsPerFrame;
	float				m_fEmitVar;
	djint32					m_nLife;
	djint32					m_nLifeVar;
	DJColor				m_cStartColor;
	DJColor				m_cStartColorVar;
	DJColor				m_cEndColor;
	DJColor				m_cEndColorVar;
	float				m_fStartSize;
	float				m_fStartSizeVar;
	float				m_fEndSize;
	float				m_fEndSizeVar;
	float				m_fRot;
	float				m_fRotVar;
	float				m_fRotSpeed;
	float				m_fRotSpeedVar;
	DJBLENDMODE			m_uSrcBlendMode;
	DJBLENDMODE			m_uDstBlendMode;
};

djuint32 DJDynamicEffectDesc::GetPackedSize(const DJBinaryBuffer &buffer) const
{
	return sizeof(EffectDescSave);
}

///

PACKRESULT DJDynamicEffectDesc::Pack(DJBinaryBuffer &buffer) const
{
	EffectDescSave save;

	if (m_pMat)
		djStringCopyMax(save.m_szMaterialName, m_pMat->GetName(), 64, 64);
	else
		save.m_szMaterialName[0] = 0;
	if (m_pMatPS)
		djStringCopyMax(save.m_szMaterialPSName, m_pMatPS->GetName(), 64, 64);
	else
		save.m_szMaterialPSName[0] = 0;
	if (m_pTexOverload)
		djStringCopyMax(save.m_szTexName, m_pTexOverload->GetName(), 64, 64);
	else
		save.m_szTexName[0] = 0;

	save.m_szTexName[63] = 0;
	save.m_szMaterialName[63] = 0;
	save.m_szMaterialPSName[63] = 0;
	save.m_uFlags			= m_uFlags;
	save.m_vForce			= m_vForce;
	save.m_fYaw				= m_fYaw;
	save.m_fYawVar			= m_fYawVar;
	save.m_fPitch			= m_fPitch;
	save.m_fPitchVar		= m_fPitchVar;
	save.m_fSpeed			= m_fSpeed;
	save.m_fSpeedVar		= m_fSpeedVar;
	save.m_nMaxParticles	= m_nMaxParticles;
	save.m_fEmitsPerFrame	= m_fEmitsPerFrame;
	save.m_fEmitVar			= m_fEmitVar;
	save.m_nLife			= m_nLife;
	save.m_nLifeVar			= m_nLifeVar;
	save.m_cStartColor		= m_cStartColor;
	save.m_cStartColorVar	= m_cStartColorVar;
	save.m_cEndColor		= m_cEndColor;
	save.m_cEndColorVar		= m_cEndColorVar;
	save.m_fStartSize		= m_fStartSize;
	save.m_fStartSizeVar	= m_fStartSizeVar;
	save.m_fEndSize			= m_fEndSize;
	save.m_fEndSizeVar		= m_fEndSizeVar;
	save.m_fRot				= m_fRot;
	save.m_fRotVar			= m_fRotVar;
	save.m_fRotSpeed		= m_fRotSpeed;
	save.m_fRotSpeedVar		= m_fRotSpeedVar;
	save.m_uSrcBlendMode	= m_uSrcBlendMode;
	save.m_uDstBlendMode	= m_uDstBlendMode;

	if (buffer.Write(&save, sizeof(save)) != sizeof(save))
		return packBufferTooSmall;

	return packSuccess;
}

///

PACKRESULT DJDynamicEffectDesc::Unpack(DJBinaryBuffer &buffer)
{
	EffectDescSave save;

	if (buffer.Read(&save, sizeof(save)) != sizeof(save))
		return packInvalidData;

	save.m_szMaterialName[63] = 0; // Just in case of corruption
	m_pMat = (DJMaterial*)theResourceManager.GetResource(save.m_szMaterialName, DJResource::TYPE_MATERIAL);
	save.m_szMaterialPSName[63] = 0; // Just in case of corruption
	m_pMatPS = (DJMaterial*)theResourceManager.GetResource(save.m_szMaterialPSName, DJResource::TYPE_MATERIAL);
	save.m_szTexName[63] = 0;
	m_pTexOverload = (DJTexture*)theResourceManager.GetResource(save.m_szTexName, DJResource::TYPE_TEXTURE);
	m_uFlags			= save.m_uFlags;
	m_vForce			= save.m_vForce;
	m_fYaw				= save.m_fYaw;
	m_fYawVar			= save.m_fYawVar;
	m_fPitch			= save.m_fPitch;
	m_fPitchVar			= save.m_fPitchVar;
	m_fSpeed			= save.m_fSpeed;
	m_fSpeedVar			= save.m_fSpeedVar;
	m_nMaxParticles		= save.m_nMaxParticles;
	m_fEmitsPerFrame	= save.m_fEmitsPerFrame;
	m_fEmitVar			= save.m_fEmitVar;
	m_nLife				= save.m_nLife;
	m_nLifeVar			= save.m_nLifeVar;
	m_cStartColor		= save.m_cStartColor;
	m_cStartColorVar	= save.m_cStartColorVar;
	m_cEndColor			= save.m_cEndColor;
	m_cEndColorVar		= save.m_cEndColorVar;
	m_fStartSize		= save.m_fStartSize;
	m_fStartSizeVar		= save.m_fStartSizeVar;
	m_fEndSize			= save.m_fEndSize;
	m_fEndSizeVar		= save.m_fEndSizeVar;
	m_fRot				= save.m_fRot;
	m_fRotVar			= save.m_fRotVar;
	m_fRotSpeed			= save.m_fRotSpeed;
	m_fRotSpeedVar		= save.m_fRotSpeedVar;
	m_uSrcBlendMode		= save.m_uSrcBlendMode;
	m_uDstBlendMode		= save.m_uDstBlendMode;

	return packSuccess;
}

///

/*
EFFECT
{
	FLAGS			POINT_SPRITE
	BLEND			SRC_ALPHA INV_SRC_ALPHA
	MAX_PARTICLES	100
	MATERIAL		"effect"
	TEXTURE			"textures/effect.tga"
	YAW				0	10
	PITCH			0	0
	SPEED			1.1	0.5
	EMITS			3	1
	LIFE			40	30
	ROTATION		0	360
	ROTATION_SPEED	0.0	1.0
	START_COLOR		[1 1 1 1] [0.1 0.1 0.1 0.2]
	END_COLOR		[1 1 1 0] [0.1 0.1 0.1 0]
	START_SIZE		10 10
	END_SIZE		40 30
}
*/

djint32 DJDynamicEffectDesc::LoadAscii(DJTagFile &file, DJTagDir *pDir)
{
	if (pDir == NULL)
		pDir = &file.m_root;

	if (pDir->m_attribute.GetLength())
	{
		SetName(pDir->m_attribute);
	}

	DJString str, str2;


	DJTagLine *pLine = pDir->GetTag("FLAGS");
	if (pLine)
	{
		// Parse flags
	}
	else
	{
		// Default flags
	}

	pLine = pDir->GetTag("BLEND");
	if (pLine && pLine->GetArgString(0, str) && pLine->GetArgString(1, str2))
	{
		m_uSrcBlendMode = djBlendModeFromName(str);
		m_uDstBlendMode = djBlendModeFromName(str2);
	}
	else
	{
		m_uSrcBlendMode = DJBLEND_SRCALPHA;
		m_uDstBlendMode = DJBLEND_INVSRCALPHA;
	}

	pLine = pDir->GetTag("MAX_PARTICLES");
	if (!pLine || !pLine->GetArgInt(0, m_nMaxParticles) || m_nMaxParticles > 0)
	{
		m_nMaxParticles = 50;
	}

	pLine = pDir->GetTag("MATERIAL");
	if (!pLine || !pLine->GetArgString(0, str))
	{
		m_pMat = NULL;
	}
	else
	{
		m_pMat = (DJMaterial*)theResourceManager.GetResource(str, DJResource::TYPE_MATERIAL);
	}

	pLine = pDir->GetTag("MATERIALPS");
	if (!pLine || !pLine->GetArgString(0, str))
	{
		m_pMatPS = NULL;
	}
	else
	{
		m_pMatPS = (DJMaterial*)theResourceManager.GetResource(str, DJResource::TYPE_MATERIAL);
	}

	pLine = pDir->GetTag("TEXTURE");
	if (pLine && pLine->GetArgString(0, str))
	{
		m_pTexOverload = (DJTexture*)theResourceManager.GetResource(str, DJResource::TYPE_TEXTURE);
	}
	else
	{
		m_pTexOverload = NULL;
	}

	pLine = pDir->GetTag("YAW");
	if (pLine && pLine->GetArgFloat(0, m_fYaw))
	{
		m_fYaw = DJRADIANS(m_fYaw);
		if (!pLine->GetArgFloat(1, m_fYawVar))
		{
			m_fYawVar = 0.0f;
		}
		else
		{
			m_fYawVar = DJRADIANS(m_fYawVar);
		}
	}
	else
	{
		m_fYaw = 0.0f;
		m_fYawVar = 0.0f;
	}

	pLine = pDir->GetTag("PITCH");
	if (pLine && pLine->GetArgFloat(0, m_fPitch))
	{
		m_fPitch = DJRADIANS(m_fPitch);
		if (!pLine->GetArgFloat(1, m_fPitchVar))
		{
			m_fPitchVar = 0.0f;
		}
		else
		{
			m_fPitchVar = DJRADIANS(m_fPitchVar);
		}
	}
	else
	{
		m_fPitch = 0.0f;
		m_fPitchVar = 0.0f;
	}

	pLine = pDir->GetTag("SPEED");
	if (pLine && pLine->GetArgFloat(0, m_fSpeed))
	{
		if (!pLine->GetArgFloat(1, m_fSpeedVar))
		{
			m_fSpeedVar = 0.0f;
		}
	}
	else
	{
		m_fSpeed = 0.0f;
		m_fSpeedVar = 0.0f;
	}

	pLine = pDir->GetTag("EMITS");
	if (pLine && pLine->GetArgFloat(0, m_fEmitsPerFrame))
	{
		if (!pLine->GetArgFloat(1, m_fEmitVar))
		{
			m_fEmitVar = 0.0f;
		}
	}
	else
	{
		m_fEmitsPerFrame = 1.0f;
		m_fEmitVar = 0.0f;
	}

	pLine = pDir->GetTag("LIFE");
	if (pLine && pLine->GetArgInt(0, m_nLife))
	{
		if (!pLine->GetArgInt(1, m_nLifeVar))
		{
			m_nLifeVar = 0;
		}
	}
	else
	{
		m_nLife = 0;
		m_nLifeVar = 0;
	}

	pLine = pDir->GetTag("ROTATION");
	if (pLine && pLine->GetArgFloat(0, m_fRot))
	{
		m_fRot = DJRADIANS(m_fRot);
		if (!pLine->GetArgFloat(1, m_fRotVar))
		{
			m_fRotVar = 0.0f;
		}
		else
		{
			m_fRotVar = DJRADIANS(m_fRotVar);
		}
	}
	else
	{
		m_fRot = 0.0f;
		m_fRotVar = 0.0f;
	}

	pLine = pDir->GetTag("ROTATION_SPEED");
	if (pLine && pLine->GetArgFloat(0, m_fRotSpeed))
	{
		m_fRotSpeed = DJRADIANS(m_fRotSpeed);
		if (!pLine->GetArgFloat(1, m_fRotSpeedVar))
		{
			m_fRotSpeedVar = 0.0f;
		}
		else
		{
			m_fRotSpeedVar = DJRADIANS(m_fRotSpeedVar);
		}
	}
	else
	{
		m_fRotSpeed = 0.0f;
		m_fRotSpeedVar = 0.0f;
	}

	pLine = pDir->GetTag("START_SIZE");
	if (pLine && pLine->GetArgFloat(0, m_fStartSize))
	{
		if (!pLine->GetArgFloat(1, m_fStartSizeVar))
		{
			m_fStartSizeVar = 10.0f;
		}
	}
	else
	{
		m_fStartSize = 10.0f;
		m_fStartSizeVar = 30.0f;
	}

	pLine = pDir->GetTag("END_SIZE");
	if (pLine && pLine->GetArgFloat(0, m_fEndSize))
	{
		if (!pLine->GetArgFloat(1, m_fEndSizeVar))
		{
			m_fEndSizeVar = 30.0f;
		}
	}
	else
	{
		m_fEndSize = 40.0f;
		m_fEndSizeVar = 30.0f;
	}

	pLine = pDir->GetTag("START_COLOR");
	if (pLine && pLine->GetArgColor(0, m_cStartColor))
	{
		if (!pLine->GetArgColor(1, m_cStartColorVar))
		{
			m_cStartColorVar = DJColor(1,1,1,1);
		}
	}
	else
	{
		m_cStartColor = DJColor(1,1,1,1);
		m_cStartColorVar = DJColor(0,0,0,0);
	}

	pLine = pDir->GetTag("END_COLOR");
	if (pLine && pLine->GetArgColor(0, m_cEndColor))
	{
		if (!pLine->GetArgColor(1, m_cEndColorVar))
		{
			m_cEndColorVar = DJColor(1,1,1,1);
		}
	}
	else
	{
		m_cEndColor = DJColor(1,1,1,1);
		m_cEndColorVar = DJColor(0,0,0,0);
	}

	return 0;
}

///

djint32 DJDynamicEffectDesc::SaveAscii(DJTagFile &file, DJTagDir *pDir) const
{
	if (pDir == NULL)
		pDir = &file.m_root;

	pDir->m_name = "EFFECT";
	pDir->m_attribute = GetName();

	// Todo...

	return 0;
}

///

DJResource* DJDynamicEffectDesc::CreateResource(const char *szName, djuint32 uFlags)
{
	DJTagFile file;
	if (file.Load(szName))
	{
		DJWarning("Failed to load effect tagfile!");
		return NULL;
	}

	DJDynamicEffectDesc *pDesc = DJ_NEW(DJDynamicEffectDesc);
	if (pDesc->LoadAscii(file, &file.m_root))
	{
		DJWarning("Failed to parse effect from ASCII tagfile!");
		delete pDesc;
		return NULL;
	}
	pDesc->SetName(szName);

	return pDesc;
}

///

djresult DJDynamicEffectDesc::OnResourceReload()
{
	// TODO: Release first before reloading

	const char * szName = GetResourceFileName();
	DJTagFile file;
	if (file.Load(szName))
	{
		DJWarning("Failed to load effect tagfile!");
		return DJ_FAILURE;
	}

	if (LoadAscii(file, &file.m_root))
	{
		DJWarning("Failed to parse effect from ASCII tagfile!");
		return DJ_FAILURE;
	}
	SetName(szName);

	return DJ_SUCCESS;
}
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
DJDynamicEffect::DJDynamicEffect()
{
	m_pVB				= NULL;
	m_pIB				= NULL;
	m_nActiveCount		= 0;
	m_pActiveParticles	= NULL;
	m_pEffectDesc		= NULL;
	m_fEmitCounter		= 0.0f;
	m_bUsePointSprites	= DJTRUE;
}

///

DJDynamicEffect::~DJDynamicEffect()
{
	TermEffect();
}

///

djbool DJDynamicEffect::InitEffect(const DJDynamicEffectDesc * pEffectDesc)
{
	DJAssert(pEffectDesc != NULL);

	if (m_nActiveCount != 0)
	{
		TermEffect();
	}

	if (pTheRenderDevice->CheckSupport(DJRenderDevice::featPointSprites))
		m_bUsePointSprites = DJTRUE;
	else
		m_bUsePointSprites = DJFALSE;

	m_pEffectDesc = pEffectDesc;

	if (m_bUsePointSprites)
	{
		m_pVB = pTheRenderDevice->CreateVertexBuffer(m_pEffectDesc->m_nMaxParticles, sizeof(DJEffectVertexPS), DJEffectVertexPS_DescCount, DJEffectVertexPS_Descs, DJVertexBuffer::ufDynamic | DJVertexBuffer::ufWriteOnly);
	}
	else
	{
		m_pVB = pTheRenderDevice->CreateVertexBuffer(m_pEffectDesc->m_nMaxParticles*4, sizeof(DJEffectVertex), DJEffectVertex_DescCount, DJEffectVertex_Descs, DJVertexBuffer::ufDynamic | DJVertexBuffer::ufWriteOnly);
		m_pIB = pTheRenderDevice->CreateIndexBuffer(m_pEffectDesc->m_nMaxParticles*6, 0, DJIndexBuffer::ifIndex16);
		djuint16* pIdx = (djuint16*)m_pIB->Lock();
		for (int q=0; q<m_pEffectDesc->m_nMaxParticles; q++)
		{
			*pIdx++ = q*4;
			*pIdx++ = q*4+1;
			*pIdx++ = q*4+2;
			*pIdx++ = q*4;
			*pIdx++ = q*4+2;
			*pIdx++ = q*4+3;
		}
		m_pIB->Unlock();
	}

	return DJTRUE;
}

///

djbool DJDynamicEffect::TermEffect()
{
	while (m_pActiveParticles)
	{
		DJEffectParticle *pPart = m_pActiveParticles;
		m_pActiveParticles = m_pActiveParticles->pNext;
		ReturnParticle(pPart);
	}
	m_nActiveCount = 0;
	DJ_SAFE_DELETE(m_pVB);
	DJ_SAFE_DELETE(m_pIB);
	return DJTRUE;
}

///

djbool DJDynamicEffect::AddParticle(const DJVector3 &vPos)
{
	DJEffectParticle *pPart;
	DJColor	start,end;
	float yaw,pitch,speed;
	if (m_nActiveCount < m_pEffectDesc->m_nMaxParticles)
	{
		pPart = GetParticle();		// THE CURRENT PARTICLE 
		if (!pPart)
			return DJFALSE;

		pPart->pNext = m_pActiveParticles;	// SET ITS NEXT POINTER
		m_pActiveParticles = pPart;		// SET IT IN THE EMITTER
		pPart->vPos = vPos;
		pPart->vPrevPos = pPart->vPos;	// USED FOR ANTI ALIAS

		// CALCULATE THE STARTING DIRECTION VECTOR
		yaw = m_pEffectDesc->m_fYaw + (m_pEffectDesc->m_fYawVar * djRandomGetFloat2());
		pitch = m_pEffectDesc->m_fPitch + (m_pEffectDesc->m_fPitchVar * djRandomGetFloat2());

		// CONVERT THE ROTATIONS TO A VECTOR
		//RotationToDirection(pitch,yaw,&pPart->vDir);
		//pPart->vDir = DJVector3(0,0,1);

		pPart->vDir[0] = (float)(-djSin(yaw) * djCos(pitch));
		pPart->vDir[1] = (float)djSin(pitch);
		pPart->vDir[2] = (float)(djCos(pitch) * djCos(yaw));

		// CALCULATE THE LIFE SPAN
		pPart->nLife = m_pEffectDesc->m_nLife + (djint32)((float)m_pEffectDesc->m_nLifeVar * djRandomGetFloat2());

		float fStartSize = m_pEffectDesc->m_fStartSize + (m_pEffectDesc->m_fStartSizeVar * djRandomGetFloat2());
		float fEndSize = m_pEffectDesc->m_fEndSize + (m_pEffectDesc->m_fEndSizeVar * djRandomGetFloat2());
		if (fStartSize < 0.0f)
			fStartSize = 0.0f;
		if (fEndSize < 0.0f)
			fEndSize = 0.0f;
		pPart->fSize = fStartSize;
		pPart->fDeltaSize = (fEndSize - fStartSize) / (float)pPart->nLife;
		
		// MULTIPLY IN THE SPEED FACTOR
		speed = m_pEffectDesc->m_fSpeed + (m_pEffectDesc->m_fSpeedVar * djRandomGetFloat2());
		pPart->vDir *= speed;

		// CALCULATE THE COLORS
		start = m_pEffectDesc->m_cStartColor + m_pEffectDesc->m_cStartColorVar * djRandomGetFloat2();
		end = m_pEffectDesc->m_cEndColor + (m_pEffectDesc->m_cEndColorVar * djRandomGetFloat2());
		start.ClampZero();
		start.ClampOne();
		end.ClampZero();
		end.ClampOne();

		pPart->cCol = start;

		// ROTATION
		pPart->fRot = m_pEffectDesc->m_fRot + m_pEffectDesc->m_fRotVar * djRandomGetFloat2();
		pPart->fDeltaRot = m_pEffectDesc->m_fRotSpeed + m_pEffectDesc->m_fRotSpeedVar * djRandomGetFloat2();

		// CREATE THE COLOR DELTA
		pPart->cDeltaCol = (end - start) / (float)pPart->nLife;

		m_nActiveCount++;	// A NEW PARTICLE IS BORN
		return DJTRUE;
	}
	return DJFALSE;
}

///

void DJDynamicEffect::UpdateEffect(const DJVector3 & vPos)
{
	DJEffectParticle *pPart = m_pActiveParticles;
	DJEffectParticle *pNext = NULL;
	DJEffectParticle *pPrev = NULL;
	while (pPart)
	{
		pNext = pPart->pNext;
		
		// IF THIS IS AN VALID PARTICLE
		if (pPart->nLife > 0)
		{
			// SAVE ITS OLD POS FOR ANTI ALIASING
			pPart->vPrevPos = pPart->vPos;

			// CALCULATE THE NEW
			pPart->vPos += pPart->vDir;

			// UPDATE ROTATION
			pPart->fRot += pPart->fDeltaRot;

			// APPLY GLOBAL FORCE TO DIRECTION
			pPart->vDir += m_pEffectDesc->m_vForce;

			// SAVE THE OLD SIZE
			pPart->fPrevSize = pPart->fSize;

			// GET THE NEW SIZE
			pPart->fSize += pPart->fDeltaSize;

			// SAVE THE OLD COLOR
			pPart->cPrevCol = pPart->cCol;

			// GET THE NEW COLOR
			pPart->cCol += pPart->cDeltaCol;

			pPart->nLife--; // IT IS A CYCLE OLDER

			pPrev = pPart;
		}
		else if (pPart->nLife == 0)
		{
			// FREE THIS SUCKER UP BACK TO THE MAIN POOL
			if (pPart == m_pActiveParticles)
			{
				m_pActiveParticles = pNext;
			}
			else
			{
				pPrev->pNext = pNext;
			}
			ReturnParticle(pPart);
			
			m_nActiveCount--;
		}
		pPart = pNext;
	}

	// Emit new particles

	m_fEmitCounter += m_pEffectDesc->m_fEmitsPerFrame + ((float)m_pEffectDesc->m_fEmitVar * djRandomGetFloat2());
	for (; m_fEmitCounter>=1.0f; m_fEmitCounter-=1.0f)
	{
		AddParticle(vPos);
	}
}

///

void DJDynamicEffect::UpdateEffectBuffers()
{
	if (m_nActiveCount == 0)
		return;

	if (m_bUsePointSprites)
	{
		DJEffectVertexPS* pVtx = (DJEffectVertexPS*)m_pVB->Lock();
		if (pVtx == NULL)
			return;

		DJEffectParticle *pPart = m_pActiveParticles;
		while (pPart)
		{
			pVtx->fX = pPart->vPos[0];
			pVtx->fY = pPart->vPos[1];
			pVtx->fSize = pPart->fSize;
			pVtx->fCol[0] = pPart->cCol[0];
			pVtx->fCol[1] = pPart->cCol[1];
			pVtx->fCol[2] = pPart->cCol[2];
			pVtx->fCol[3] = pPart->cCol[3];
			pVtx->fRot = pPart->fRot;
			pPart = pPart->pNext;
			pVtx++;
		}

		m_pVB->Unlock();
	}
	else
	{
		DJEffectVertex* pVtx = (DJEffectVertex*)m_pVB->Lock();
		if (pVtx == NULL)
			return;

		DJEffectParticle *pPart = m_pActiveParticles;
		float fSin,fCos;
		while (pPart)
		{
			djSinCos(pPart->fRot, &fSin, &fCos);

			pVtx->fX = pPart->vPos[0] - pPart->fSize*0.5f;
			pVtx->fY = pPart->vPos[1] - pPart->fSize*0.5f;
			pVtx->fU = -0.5f * fCos + 0.5f * fSin + 0.5f;
			pVtx->fV = -0.5f * fSin - 0.5f * fCos + 0.5f;
			pVtx->fCol[0] = pPart->cCol[0];
			pVtx->fCol[1] = pPart->cCol[1];
			pVtx->fCol[2] = pPart->cCol[2];
			pVtx->fCol[3] = pPart->cCol[3];
			pVtx++;

			pVtx->fX = pPart->vPos[0] - pPart->fSize*0.5f;
			pVtx->fY = pPart->vPos[1] + pPart->fSize*0.5f;
			pVtx->fU = -0.5f * fCos - 0.5f * fSin + 0.5f;
			pVtx->fV = -0.5f * fSin + 0.5f * fCos + 0.5f;
			pVtx->fCol[0] = pPart->cCol[0];
			pVtx->fCol[1] = pPart->cCol[1];
			pVtx->fCol[2] = pPart->cCol[2];
			pVtx->fCol[3] = pPart->cCol[3];
			pVtx++;

			pVtx->fX = pPart->vPos[0] + pPart->fSize*0.5f;
			pVtx->fY = pPart->vPos[1] + pPart->fSize*0.5f;
			pVtx->fU = 0.5f * fCos - 0.5f * fSin + 0.5f;
			pVtx->fV = 0.5f * fSin + 0.5f * fCos + 0.5f;
			pVtx->fCol[0] = pPart->cCol[0];
			pVtx->fCol[1] = pPart->cCol[1];
			pVtx->fCol[2] = pPart->cCol[2];
			pVtx->fCol[3] = pPart->cCol[3];
			pVtx++;

			pVtx->fX = pPart->vPos[0] + pPart->fSize*0.5f;
			pVtx->fY = pPart->vPos[1] - pPart->fSize*0.5f;
			pVtx->fU = 0.5f * fCos + 0.5f * fSin + 0.5f;
			pVtx->fV = 0.5f * fSin - 0.5f * fCos + 0.5f;
			pVtx->fCol[0] = pPart->cCol[0];
			pVtx->fCol[1] = pPart->cCol[1];
			pVtx->fCol[2] = pPart->cCol[2];
			pVtx->fCol[3] = pPart->cCol[3];
			pVtx++;

			pPart = pPart->pNext;
		}

		m_pVB->Unlock();
	}
}

///

void DJDynamicEffect::RenderEffect()
{
	if (m_nActiveCount == 0)
		return;

	DJMaterial *pMat;

	if (m_bUsePointSprites)
		pMat = m_pEffectDesc->m_pMatPS;
	else
		pMat = m_pEffectDesc->m_pMat;
	pTheRenderDevice->SetMaterial(pMat);
	DJMaterialMode *mode = pMat->GetCurrentRenderMode();
	if (!mode)
		return;
	pTheRenderDevice->UpdateUniforms(mode->GetLocalUniformList(), NULL);
	if (m_pEffectDesc->m_pTexOverload)
		pTheRenderDevice->SetTexture(m_pEffectDesc->m_pTexOverload);
	pTheRenderDevice->SetTextureClampU(0);
	pTheRenderDevice->SetTextureClampV(0);
	pTheRenderDevice->SetAlphaBlendMode(m_pEffectDesc->m_uSrcBlendMode, m_pEffectDesc->m_uDstBlendMode);
	if (m_bUsePointSprites)
		pTheRenderDevice->RenderPrimitive(DJRenderDevice::ptPointList, m_nActiveCount, m_pVB);
	else
		pTheRenderDevice->RenderIndexedPrimitive(DJRenderDevice::ptTriangleList, m_nActiveCount*2, m_nActiveCount*4, m_pVB, m_pIB);
}
/////////////////////////////////////////////////////////////////


