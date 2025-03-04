#include "stdafx.h"
#include "BombableObject.h"

CBombableObject::CBombableObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CModelObject(_pDevice, _pContext)
{
}

CBombableObject::CBombableObject(const CBombableObject& _Prototype)
	:CModelObject(_Prototype)
{
}

void CBombableObject::Install_Bomb()
{
	m_bBombInstalled = true;
	On_BombInstalled();
}

void CBombableObject::Detonate()
{
	m_bBombInstalled = false;
	On_Detonated();
}

CCollider* CBombableObject::Get_BodyCollider()
{
	return m_p2DColliderComs[0];
}

