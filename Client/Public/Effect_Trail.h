#pragma once
#include "PartObject.h"

BEGIN(Engine)
class CShader;
class CVIBuffer_Trail;
class CTexture;
END

BEGIN(Client)

class CEffect_Trail : public CPartObject
{
public:
	enum TRAILTYPE {CENTRAL_TRAIL, FOLLOW_TRAIL};
public:
	typedef struct tagEffectTrail : public CPartObject::PARTOBJECT_DESC
	{		
		TRAILTYPE   eTrailType = CENTRAL_TRAIL;
		_float fAddTime = 0.002f;
		_float fTrailLifeTime = 0.25f;
		_float fLength = 1.5f;
		_float3 vAddPoint = _float3(0.f, 0.f, 0.f);
		_float4 vColor = _float4(1.f, 1.f, 1.f, 1.f);
		const _tchar* szTextureTag = L"";
		const _tchar* szBufferTag = L"";
		_uint	iTextureLevel = LEVEL_STATIC;
		_uint	iBufferLevel = LEVEL_STATIC;

	}EFFECTTRAIL_DESC;
private:
	CEffect_Trail(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CEffect_Trail(const CEffect_Trail& _Prototype);
	virtual ~CEffect_Trail() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void Update(_float _fTimeDelta) override;
	virtual void Late_Update(_float _fTimeDelta) override;
	virtual HRESULT				Render() override;

public:
	void		Add_Point(_fvector _vPosition);
	void		Add_Point();
	void		Delete_Effect();
	void		Delete_Delay(_float _fDelay);


public:
	void		Set_AddUpdate(_bool _isAddUpdate) { m_isAddUpdate = _isAddUpdate; }
	//void		Set_DeleteUpdate(_bool _isDeleteUpdate) { m_isDeleteUpdate = _isDeleteUpdate; }

private:
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Trail* m_pBufferCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	
private:
	_bool  m_isAddUpdate = { true }; // 정점 더하는(갱신하는) 기능
	//_bool  m_isDeleteUpdate = { true }; // 정점 

private:
	TRAILTYPE m_eTrailType = CENTRAL_TRAIL;
	_float m_fAccAddTime = 0.f;
	_float m_fAddTime = 0.002f;
	_float m_fTrailLifeTime = 0.25f;
	//_float m_fAccDeleteTime = 0.f;
	//_float m_fDeleteTime = 0.019f;
	
	_float m_fDeleteAllDelay = D3D11_FLOAT32_MAX;

private:
	_float	m_fLength = 1.5f;
	_float3 m_vAddPoint = _float3(0.f, 0.f, 0.f);
	_float4 m_vColor = _float4(1.f, 1.f, 1.f, 1.f);

private:
	HRESULT Bind_ShaderResources();
	HRESULT Ready_Components(const EFFECTTRAIL_DESC* _pDesc);

public:
	static CEffect_Trail* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;
	virtual HRESULT Cleanup_DeadReferences() override;
};

END