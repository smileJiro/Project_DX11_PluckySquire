#pragma once
#include "Texture.h"

BEGIN(Engine)
//애니메이션 내 하나의 프레임 데이터
class CSpriteFrame : public CBase
{
private:
	CSpriteFrame() {}
	CSpriteFrame(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, string _strDirectory, json& _jData);
	CSpriteFrame(const CSpriteFrame& _Prototype);
	~CSpriteFrame();

public:
	HRESULT Bind_ShaderResource(class CShader* _pShader);
	_uint Get_FrameRun() { return iFrameRun; }
private:
	_float2 vSpriteStartUV = { 0,0};
	_float2 vSpriteEndUV= { 0,0};
	CTexture* pTexture = { nullptr };
	_float fPixelsPerUnrealUnit = { 1.0f };
	vector<_float4> fBakedRenderData ;
	_uint iFrameRun = { 1 };

public:
	static CSpriteFrame* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, string _strDirectory, json& _jData);
	virtual CSpriteFrame* Clone();
	virtual void Free() override;
};

//하나의 애니메이션 데이터
class CAnimation2D : public CBase
{
private:
	CAnimation2D() {}
	CAnimation2D(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, string _strDirectory, json& _jData);
	CAnimation2D(const CAnimation2D& _Prototype);
public:
	HRESULT Bind_ShaderResource(class CShader* _pShader);
	_bool Play_Animation(_float _fTimeDelta);
	const CSpriteFrame* GetCurrentSprite() { return SpriteFrames[iCurrentFrame]; }
	void Set_Loop(_bool bIsLoop) { bLoop = bIsLoop; }

private:
	string strName;
	_float fFramesPerSecond = 60;
	_float fCurrentFrameTime = 0;
	_uint iCurrentFrame = { 0};
	_uint iCurrentSubFrame = { 0};
	_uint iFrameCount = 0;
	_bool bLoop = true;

	vector<CSpriteFrame*> SpriteFrames;

public:
	static CAnimation2D* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, string _strDirectory, json& _jData);
	virtual CAnimation2D* Clone();
	virtual void Free() override;
};

END