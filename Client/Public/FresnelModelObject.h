#pragma once
#include "ModelObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTexture;
END

BEGIN(Client)

class CFresnelModelObject : public CModelObject
{
public:
	typedef struct tagFresnelModelDesc : public CModelObject::MODELOBJECT_DESC
	{
		ID3D11Buffer* pFresnelBuffer = {};
		ID3D11Buffer* pColorBuffer = {};
		_float2		  vDiffuseScaling = {1.f, 1.f};
		_float2		  vNoiseScaling = {1.f, 1.f};
		const _tchar* szDiffusePrototypeTag = L"";
		const _tchar* szNoisePrototypeTag = L"";
		const _float* pScaling = { nullptr };
	}FRESNEL_MODEL_DESC;
protected:
	CFresnelModelObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CFresnelModelObject(const CFresnelModelObject& _Prototype);
	virtual ~CFresnelModelObject() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;
	virtual HRESULT Render() override;

protected:
	ID3D11Buffer* m_pFresnelBuffer = { nullptr };
	ID3D11Buffer* m_pColorBuffer = { nullptr };
	class CTexture* m_pMainTextureCom = { nullptr };
	class CTexture* m_pNoiseTextureCom = { nullptr };
	_float2			m_vDiffuseScaling = { 1.f, 1.f };
	_float2			m_vNoiseScaling = { 1.f, 1.f };

	const _float* m_pScaling = { nullptr };

	HRESULT	Ready_Components(const FRESNEL_MODEL_DESC* _pDesc);


public:
	static CFresnelModelObject* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;
};

END