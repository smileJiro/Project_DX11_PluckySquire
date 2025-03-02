#pragma once
#include "ModelObject.h"

BEGIN(Client)
class CMudPit :
    public CModelObject
{
public:
	CMudPit(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CMudPit(const CMudPit& _Prototype);
	virtual ~CMudPit() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;
	virtual HRESULT	Render() override;
public:
	static CMudPit* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;
};

END