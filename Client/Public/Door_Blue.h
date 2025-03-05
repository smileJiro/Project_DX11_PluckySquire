#pragma once
#include "Door_2D.h"

BEGIN(Client)

class CDoor_Blue : public CDoor_2D
{
private:
	CDoor_Blue(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CDoor_Blue(const CDoor_Blue& _Prototype);
	virtual ~CDoor_Blue() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;


public:
	void			On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx);

public:
	virtual void			On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject) override;

private:
	void			Set_AnimLoop();
	virtual void	Switch_Animation_By_State() override;


public:
	static CDoor_Blue* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;
};

END