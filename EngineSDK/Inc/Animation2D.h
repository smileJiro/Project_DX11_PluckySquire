#pragma once
#include "Animation.h" 
#include "Texture.h"

BEGIN(Engine)
//애니메이션 내 하나의 프레임 데이터
class ENGINE_DLL CSpriteFrame : public CBase
{
protected:
	CSpriteFrame();
	CSpriteFrame(const CSpriteFrame& _Prototype);
	~CSpriteFrame();

public:
	virtual HRESULT			Initialize(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _char* szDirPath,ifstream& _infIle, map<string, CTexture*>& _Textures);

	HRESULT Bind_ShaderResource(class CShader* _pShader);
	const _matrix* Get_Transform() const { 
		return &matSpriteTransform; 
	}
protected:
	_float2 vSpriteStartUV = { 0,0};
	_float2 vSpriteEndUV= { 0,0};
	_float fPixelsPerUnrealUnit = { 1.0f };
	_matrix matSpriteTransform;
	CTexture* pTexture = { nullptr };
public:
	static CSpriteFrame* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _char* szDirPath, ifstream& _infIle, map<string, CTexture*>& _Textures);
	virtual CSpriteFrame* Clone();
	virtual void Free() override;
};

//하나의 애니메이션 데이터
class ENGINE_DLL CAnimation2D : public CAnimation
{
protected:
	CAnimation2D();
	CAnimation2D(const CAnimation2D& _Prototype);
public:
	HRESULT			Initialize(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _char* szDirPath, ifstream& _infIle, map<string, CTexture*>& _Textures);


	HRESULT Bind_ShaderResource(class CShader* _pShader);
	_bool Play_Animation(_float _fTimeDelta);

	void Add_SpriteFrame(CSpriteFrame* _pSpriteFrame, _uint _iFrameRun);
	void Reset_CurrentTrackPosition();

	const CSpriteFrame* GetCurrentSprite() { return SpriteFrames[iCurrentFrame].first; }
	const _matrix* Get_CurrentSpriteTransform() ;
	virtual _float	Get_Progress() override;

	void Set_Loop(_bool bIsLoop) { bLoop = bIsLoop; }

protected:
	_uint Get_AccumulativeSubFrameCount(_uint _iFrameIndex);

protected:
	string strName;
	_float fFramesPerSecond = 60;
	_uint iFrameCount = 0;
	_bool bLoop = false;
	vector<pair<CSpriteFrame*,_uint>> SpriteFrames;

	_float fCurrentFrameTime = 0;
	_uint iCurrentFrame = { 0};
	_uint iCurrentSubFrame = { 0};

public:
	static CAnimation2D* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _char* szDirPath, ifstream& _infIle, map<string, CTexture*>& _Textures);
	virtual CAnimation2D* Clone();
	virtual void Free() override;
};

END