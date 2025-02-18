#pragma once
#include "ModelObject.h"

typedef struct tagFresnel
{
	_float4 vColor = { 1.f, 1.f, 1.f, 1.f };

	_float	fExp = 2.f;
	_float	fBaseReflect = 1.0f;
	_float	fStrength = 1.0f;
	_float	fDummy;

} FRESNEL_INFO;

typedef struct tagFresnels
{
	FRESNEL_INFO tInner;
	FRESNEL_INFO tOuter;
} MULTI_FRESNEL_INFO;

class CMagic_Hand_Body : public CModelObject
{

private:
	CMagic_Hand_Body(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CMagic_Hand_Body(const CMagic_Hand_Body& _Prototype);
	virtual ~CMagic_Hand_Body() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void Priority_Update(_float _fTimeDelta) override;
	virtual void Update(_float _fTimeDelta) override;
	virtual void Late_Update(_float _fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_float					m_fSpecular = { 0.64f };
	_float					m_fBloomThreshold = { 0.f };
	MULTI_FRESNEL_INFO		m_tFresnels = {};
	ID3D11Buffer*			m_pFresnelsBuffer = { nullptr };

private:
	HRESULT			Bind_ShaderResources();

public:
	static CMagic_Hand_Body* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;

public:
	HRESULT Cleanup_DeadReferences() override;
	virtual void Active_OnEnable() override;
	virtual void Active_OnDisable() override;


};

