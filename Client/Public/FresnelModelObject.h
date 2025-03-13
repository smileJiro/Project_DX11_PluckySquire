#pragma once
#include "ModelObject.h"
#include "Client_Defines.h"

BEGIN(Client)

class CFresnelModelObject : public CModelObject
{
public:
	typedef struct tagFresnelModelDesc : public CModelObject::MODELOBJECT_DESC
	{
		ID3D11Buffer* pFresnelBuffer = {};
		ID3D11Buffer* pColorBuffer = {};

	}FRESNEL_MODEL_DESC;
private:
	CFresnelModelObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CFresnelModelObject(const CFresnelModelObject& _Prototype);
	virtual ~CFresnelModelObject() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;
	virtual HRESULT Render() override;

private:
	ID3D11Buffer* m_pFresnelBuffer = { nullptr };
	ID3D11Buffer* m_pColorBuffer = { nullptr };

public:
	static CFresnelModelObject* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CFresnelModelObject* Clone(void* _pArg) override;
	virtual void Free() override;
};

END