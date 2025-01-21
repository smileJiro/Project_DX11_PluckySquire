#pragma once
#include "Texture.h"

BEGIN(Engine)
//애니메이션 내 하나의 프레임 데이터
class CSpriteFrame : public CBase
{
private:
	CSpriteFrame() {}
	CSpriteFrame(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, json& _jData , map<string, CTexture*>& _Textures);
	CSpriteFrame(const CSpriteFrame& _Prototype);
	~CSpriteFrame();

public:
	HRESULT Bind_ShaderResource(class CShader* _pShader);
	const _matrix* Get_Transform() const { 
		return &matSpriteTransform; 
	}
private:
	_float2 vSpriteStartUV = { 0,0};
	_float2 vSpriteEndUV= { 0,0};
	_matrix matSpriteTransform ;
	CTexture* pTexture = { nullptr };
	_float fPixelsPerUnrealUnit = { 1.0f };
	vector<_float4> fBakedRenderData ;

public:
	static CSpriteFrame* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, json& _jData, map<string, CTexture*>& _Textures);
	virtual CSpriteFrame* Clone();
	virtual void Free() override;
};

//하나의 애니메이션 데이터
class CAnimation2D : public CBase
{
private:
	CAnimation2D() {}
	CAnimation2D(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, json& _jData, map<string, json>& _jPaperSprites, map<string, CTexture*>& _Textures);
	CAnimation2D(const CAnimation2D& _Prototype);
public:
	HRESULT Bind_ShaderResource(class CShader* _pShader);
	_bool Play_Animation(_float _fTimeDelta);

	void Add_SpriteFrame(CSpriteFrame* _pSpriteFrame, _uint _iFrameRun);
	void Reset_CurrentTrackPosition();

	const CSpriteFrame* GetCurrentSprite() { return SpriteFrames[iCurrentFrame].first; }
	const _matrix* Get_CurrentSpriteTransform() ;

	void Set_Loop(_bool bIsLoop) { bLoop = bIsLoop; }
private:
	string strName;
	_float fFramesPerSecond = 60;
	_float fCurrentFrameTime = 0;
	_uint iCurrentFrame = { 0};
	_uint iCurrentSubFrame = { 0};
	_uint iFrameCount = 0;
	_bool bLoop = false;

	vector<pair<CSpriteFrame*,_uint>> SpriteFrames;

public:
	static CAnimation2D* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, json& _jData, map<string, json>& _jPaperSprites, map<string, CTexture*>& _Textures);
	virtual CAnimation2D* Clone();
	virtual void Free() override;
};

END