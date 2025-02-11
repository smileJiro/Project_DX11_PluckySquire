#pragma once
#include "ModelObject.h"

BEGIN(AnimTool)
class CBackgorund :
    public CModelObject
{
protected:
	CBackgorund(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CBackgorund(const CBackgorund& _Prototype);
	virtual ~CBackgorund() = default;

public:
	virtual void			Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;

protected:
	virtual HRESULT			Bind_ShaderResources_WVP();

private:
	_float4x4 m_matView;
	_float4x4 m_matProj;
public:
	static CBackgorund* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CBackgorund* Clone(void* _pArg) override;
	virtual void			Free() override;

};

END