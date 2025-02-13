#pragma once
#include "ModelObject.h"

BEGIN(Client)
class CDomino :
    public CModelObject
{
protected:
	explicit CDomino(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CDomino(const CDomino& _Prototype);
	virtual ~CDomino() = default;
public:
	HRESULT Initialize(void* _pArg);
public:
	static CDomino* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;
};

END