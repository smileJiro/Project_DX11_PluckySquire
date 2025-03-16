#include "stdafx.h"
#include "PlayerBlocker.h"
#include "Collider.h"
#include "GameInstance.h"
#include "Section_Manager.h"
CPlayerBlocker::CPlayerBlocker(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, COORDINATE _eCoord)
	:CBlocker(_pDevice, _pContext, _eCoord)
{
}

CPlayerBlocker::CPlayerBlocker(const CPlayerBlocker& _Prototype)
	:CBlocker(_Prototype)
{
}

HRESULT CPlayerBlocker::Initialize(void* _pArg)
{
	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;
	Set_ObjectGroupID(OBJECT_GROUP::PLAYER_BLOCKER);
	m_p2DColliderComs[0]->Set_CollisionGroupID(OBJECT_GROUP::PLAYER_BLOCKER);
	return S_OK;
}

void CPlayerBlocker::Update(_float _fTimeDelta)
{
	__super::Update(_fTimeDelta);
}

HRESULT CPlayerBlocker::Render()
{
#ifdef _DEBUG
	for (auto& p2DCollider : m_p2DColliderComs)
	{
		p2DCollider->Render(SECTION_MGR->Get_Section_RenderTarget_Size(m_strSectionName));
	}
#endif // _DEBUG

	return S_OK;
}

CPlayerBlocker* CPlayerBlocker::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, COORDINATE _eCoord)
{
	CPlayerBlocker* pInstance = new CPlayerBlocker(_pDevice, _pContext, _eCoord);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed Create : PlayerBlocker");
		return nullptr;
	}

	return pInstance;
}

CGameObject* CPlayerBlocker::Clone(void* _pArg)
{
	CPlayerBlocker* pInstance = new CPlayerBlocker(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : PlayerBlocker");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerBlocker::Free()
{
	__super::Free();
}
