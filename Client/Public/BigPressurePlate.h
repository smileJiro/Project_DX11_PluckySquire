#pragma once
#include "Pressure_Plate.h"
BEGIN(Client)
class CBigPressurePlate :
    public CPressure_Plate
{
public:
	CBigPressurePlate(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CBigPressurePlate(const CBigPressurePlate& _Prototype);
	virtual ~CBigPressurePlate() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;


public:
	virtual void	On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject) override;
	virtual void	On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject) override;
	void			On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx);

private:

public:
	static CBigPressurePlate* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;
};

END