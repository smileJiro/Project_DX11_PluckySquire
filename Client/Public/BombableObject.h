#pragma once
#include "ModelObject.h"
#include "Bombable.h"

BEGIN(Client)
class CBombableObject :
	public CModelObject, public IBombable
{

protected:
	CBombableObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CBombableObject(const CBombableObject& _Prototype);
	virtual ~CBombableObject() = default;
public:
	void Install_Bomb();

	virtual void Detonate() override;

	CCollider* Get_BodyCollider();
protected:
	virtual void On_BombInstalled() {};
	virtual void On_Detonated() {};
private:
	_bool m_bBombInstalled = false;
};

END