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
	virtual HRESULT			Initialize(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _char* szTextureKey, map<string, CTexture*>& _Textures);

	HRESULT Bind_ShaderResource(class CShader* _pShader);
	const _matrix* Get_Transform() const { 
		return &m_matSpriteTransform; 
	}

#ifdef NDEBUG
	CTexture* Get_Texture() const { return m_pTexture; }

	const _float2 Get_StartUV() const { return m_vSpriteStartUV; }
	const _float2 Get_EndUV() const { return m_vSpriteEndUV; }
#endif // _DEBUG

protected:
	_float2 m_vSpriteStartUV = { 0,0};
	_float2 m_vSpriteEndUV= { 0,0};
	_float m_fPixelsPerUnrealUnit = { 1.0f };
	_matrix m_matSpriteTransform;
	CTexture* m_pTexture = { nullptr };
public:
	static CSpriteFrame* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _char* szDirPath, ifstream& _infIle, map<string, CTexture*>& _Textures);
	static CSpriteFrame* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _char* szTextureKey, map<string, CTexture*>& _Textures);
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
	_bool Play_Animation(_float _fTimeDelta, _bool bReverse);

	void Add_SpriteFrame(CSpriteFrame* _pSpriteFrame, _uint _iFrameRun);

	const CSpriteFrame* GetCurrentSprite() { return m_SpriteFrames[m_iCurrentFrame].first; }
	const _matrix* Get_CurrentSpriteTransform() ;
	virtual _float	Get_Progress() override;
	virtual _float Get_AnimationTime() override;

	virtual void Set_Progress(_float _fProgerss,_bool _bReverse)override;
protected:
	_uint Get_AccumulativeSubFrameCount(_uint _iFrameIndex);

protected:
	string m_strName;
	_float m_fFramesPerSecond = 60;
	_uint m_iFrameCount = 0;

	vector<pair<CSpriteFrame*,_uint>> m_SpriteFrames;

	_float m_fCurrentFrameTime = 0;
	_int m_iCurrentFrame = { 0};
	_int m_iCurrentSubFrame = { 0};

public:
	static CAnimation2D* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _char* szDirPath, ifstream& _infIle, map<string, CTexture*>& _Textures);
	virtual CAnimation2D* Clone();
	virtual void Free() override;
};

END