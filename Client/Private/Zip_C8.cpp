#include "stdafx.h"
#include "Zip_C8.h"
#include "Collider_Circle.h"
#include "Player.h"
#include "Trigger_Manager.h"
CZip_C8::CZip_C8(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CModelObject(_pDevice, _pContext)
{
}

CZip_C8::CZip_C8(const CZip_C8& _Prototype)
	:CModelObject(_Prototype)
{
}

HRESULT CZip_C8::Initialize(void* _pArg)
{
	CModelObject::MODELOBJECT_DESC* pDesc = static_cast<CModelObject::MODELOBJECT_DESC*>(_pArg);

	pDesc->eStartCoord = COORDINATE_3D;

	pDesc->Build_2D_Model(pDesc->iCurLevelID, TEXT("Chpat8Zip"), TEXT("Prototype_Component_Shader_VtxPosTex"));

	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	m_p2DColliderComs.resize(1);
	//몸통 콜라이더
	CCollider_Circle::COLLIDER_CIRCLE_DESC CircleDesc = {};
	CircleDesc.pOwner = this;
	CircleDesc.fRadius = 2000.f;
	CircleDesc.vScale = { 1.0f, 1.0f };
	CircleDesc.vOffsetPosition = { 0.f, 0.f};
	CircleDesc.isBlock = false;
	CircleDesc.isTrigger = true;
	CircleDesc.iCollisionGroupID = OBJECT_GROUP::TRIGGER_OBJECT;
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Circle"),
		TEXT("Com_Body2DCollider"), reinterpret_cast<CComponent**>(&m_p2DColliderComs[0]), &CircleDesc)))
		return E_FAIL;
	//Chapter8_SKSP_11
	Register_OnAnimEndCallBack(bind(&CZip_C8::On_AnimEnd, this, placeholders::_1, placeholders::_2));
	Set_ProgState(WAITING);
	return S_OK;
}

void CZip_C8::Update(_float _fTimeDelta)
{
	__super::Update(_fTimeDelta);
}

void CZip_C8::Late_Update(_float _fTimeDelta)
{
	__super::Late_Update(_fTimeDelta);
}

HRESULT CZip_C8::Render()
{
	return __super::Render();
}

void CZip_C8::On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{    //Entered
	if (OBJECT_GROUP::PLAYER & _pOtherObject->Get_ObjectGroupID()
		&& m_p2DColliderComs[0] == _pMyCollider)
	{
		m_pPlayer = dynamic_cast<CPlayer*>(_pOtherObject);
		if (nullptr == m_pPlayer)
			return;
		Safe_AddRef(m_pPlayer);

		m_pPlayer->Set_BlockPlayerInput(true);
		CTrigger_Manager::GetInstance()->Register_TriggerEvent(TEXT("Chapter8_TransformZip"),0);

	}
}

void CZip_C8::On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
}

void CZip_C8::On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
}

void CZip_C8::On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{
	if(ATTACH_JOT == iAnimIdx)
		Set_ProgState(ATTACH_END);
}

void CZip_C8::Set_ProgState(PROG_STATE _eState)
{
	if (m_eProgState == _eState)
		return;
	m_eProgState = _eState;
	switch (m_eProgState)
	{
	case Client::CZip_C8::WAITING:
		Switch_Animation(IDLE);
		break;
	case Client::CZip_C8::TALK:
		Switch_Animation(TALK1);
		break;
	case Client::CZip_C8::ATTACH:
		Switch_Animation(ATTACH_JOT);
		break;
	case Client::CZip_C8::ATTACH_END:
		Event_DeleteObject(this);
		break;
	case Client::CZip_C8::LAST:
		break;
	default:
		break;
	}
}

CZip_C8* CZip_C8::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CZip_C8* pInstance = new CZip_C8(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CZip_C8");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CZip_C8::Clone(void* _pArg)
{
	CZip_C8* pInstance = new CZip_C8(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CZip_C8");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CZip_C8::Free()
{
	Safe_Release(m_pPlayer);
	__super::Free();
}

