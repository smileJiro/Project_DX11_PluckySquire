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

	void Detonate();
private:
	_bool m_bBombInstalled = false;
};

END