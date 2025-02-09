#include "stdafx.h"
#include "PlayerItem.h"

#include "GameInstance.h"

CPlayerItem::CPlayerItem(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CTriggerObject(_pDevice, _pContext)
{
}

CPlayerItem::CPlayerItem(const CPlayerItem& _Prototype)
	:CTriggerObject(_Prototype)
{
}

HRESULT CPlayerItem::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayerItem::Initialize(void* _pArg)
{
	CPlayerItem::PLAYERITEM_DESC* pDesc = static_cast<CPlayerItem::PLAYERITEM_DESC*>(_pArg);

	pDesc->eStartCoord = COORDINATE_3D;
	pDesc->isCoordChangeEnable = false;
	pDesc->tTransform3DDesc.fRotationPerSec = XMConvertToRadians(360.f);
	pDesc->tTransform3DDesc.fSpeedPerSec = 10.f;

	pDesc->iActorType = (_uint)ACTOR_TYPE::DYNAMIC;
	pDesc->eShapeType = (SHAPE_TYPE)SHAPE_TYPE::SPHERE;
	pDesc->fRadius = 2.f;

	pDesc->iFillterMyGroup = OBJECT_GROUP::TRIGGER_OBJECT;
	pDesc->iFillterOtherGroupMask = OBJECT_GROUP::PLAYER;

	pDesc->iTriggerType = (_uint)TRIGGER_TYPE::EVENT_TRIGGER;
	m_szModelTag = pDesc->szModelTag;

	if (FAILED(__super::Initialize(pDesc))) {
		MSG_BOX("PlayerItem Initialize Falied");
		return E_FAIL;
	}

	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;

	return S_OK;
}

void CPlayerItem::Priority_Update(_float _fTimeDelta)
{
}

void CPlayerItem::Update(_float _fTimeDelta)
{

}

void CPlayerItem::Late_Update(_float _fTimeDelta)
{
	Action_Mode(_fTimeDelta);

	m_pGameInstance->Add_RenderObject_New(RG_3D, PR3D_NONBLEND, this);

}

HRESULT CPlayerItem::Render()
{
	m_pModelCom->Render(m_pShaderCom, (_uint)PASS_VTXMESH::DEFAULT);

	return S_OK;
}

HRESULT CPlayerItem::Ready_Components(PLAYERITEM_DESC* _pArg)
{
	// Shader
	if (FAILED(Add_Component(m_pGameInstance->Get_StaticLevelID(), TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader_3D"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	// Model
	if (FAILED(Add_Component(_pArg->iCurLevelID, m_szModelTag,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

void CPlayerItem::Action_Mode(_float _fTimeDelta)
{
	switch (m_iItemMode) {
	case DEFAULT:
		break;
	case GETTING:
		Action_Getting(_fTimeDelta);
		break;
	case DISAPPEAR:
		Action_Disappear(_fTimeDelta);
		break;
	}
}

void CPlayerItem::Action_Getting(_float _fTimeDelta)
{
}

void CPlayerItem::Action_Disappear(_float _fTimeDelta)
{
}

void CPlayerItem::Free()
{
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

	__super::Free();
}
