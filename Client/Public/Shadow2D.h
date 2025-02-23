#pragma once
#include "ModelObject.h"

BEGIN(Client)

class CShadow2D :
    public CModelObject
{
public:
    typedef struct tag2DShadowDesc
    {
		CModelObject* pTargetObject = nullptr;
    }SHADOW2D_DESC;

public:
	CShadow2D(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CShadow2D(const CShadow2D& _Prototype);
	virtual ~CShadow2D() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;


	virtual HRESULT Render() override;
protected:
	virtual HRESULT			Bind_ShaderResources_WVP();

private:
	CModelObject* m_pTargetObject = nullptr;
public:
	static CShadow2D* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;
};

END