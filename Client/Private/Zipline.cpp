#include "stdafx.h"
#include "Zipline.h"
#include "GameInstance.h"
#include "Section_Manager.h"

#include "Player.h"

CZipline::CZipline(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CModelObject(_pDevice, _pContext)
{
}

CZipline::CZipline(const CZipline& _Prototype)
    : CModelObject(_Prototype)
{
}

HRESULT CZipline::Initialize(void* _pArg)
{
	if (nullptr == _pArg)
		return E_FAIL;

	ZIPLINE_DESC* pDesc = static_cast<ZIPLINE_DESC*>(_pArg);

	pDesc->eStartCoord = COORDINATE_2D;
	pDesc->isCoordChangeEnable = false;
	pDesc->strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
	pDesc->iShaderPass_2D = (_uint)PASS_VTXPOSTEX::SPRITE2D;

	pDesc->tTransform2DDesc.vInitialScaling = _float3(1.f, 1.f, 1.f);

	pDesc->eActorType = ACTOR_TYPE::LAST;
	pDesc->pActorDesc = nullptr;
	pDesc->iModelPrototypeLevelID_2D = LEVEL_CHAPTER_4;
	pDesc->strModelPrototypeTag_2D = TEXT("Zipline");
	pDesc->iObjectGroupID = OBJECT_GROUP::INTERACTION_OBEJCT;

	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	// 2D 충돌용 콜라이더 추가.
	m_p2DColliderComs.resize(1);

	if (FAILED(CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(pDesc->strSectionTag, this)))
		return E_FAIL;

	CCollider_Circle::COLLIDER_CIRCLE_DESC CircleDesc = {};

	CircleDesc.pOwner = this;
	CircleDesc.fRadius = 0.2f;
	CircleDesc.vScale = { 1.0f, 1.0f };
	CircleDesc.vOffsetPosition = { 0.f, 0.f };
	CircleDesc.isBlock = false;
	CircleDesc.isTrigger = false;
	CircleDesc.iCollisionGroupID = OBJECT_GROUP::INTERACTION_OBEJCT;

	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Circle"),
		TEXT("Com_Body2DCollider"), reinterpret_cast<CComponent**>(&m_p2DColliderComs[0]), &CircleDesc)))
		return E_FAIL;

	m_p2DColliderComs[0]->Set_Active(true);

	m_vDestination = { -1922.93f, 278.69f, 0.f };

    return S_OK;
}

void CZipline::Update(_float _fTimeDelta)
{
	Ride_Down(_fTimeDelta);

	__super::Update(_fTimeDelta);
}

HRESULT CZipline::Render()
{
	__super::Render();
#ifdef _DEBUG
	for (auto& p2DCollider : m_p2DColliderComs)
	{
		p2DCollider->Render(SECTION_MGR->Get_Section_RenderTarget_Size(m_strSectionName));
	}
#endif // _DEBUG

	return S_OK;
}

void CZipline::Interact(CPlayer* _pUser)
{
	m_isRideDown = true;

	m_pRidingObject = _pUser;

	Safe_AddRef(m_pRidingObject);
}

_bool CZipline::Is_Interactable(CPlayer* _pUser)
{
	if (_pUser->Is_CarryingObject())
		return false;

	return true;
}

_float CZipline::Get_Distance(COORDINATE _eCoord, CPlayer* _pUser)
{
	return XMVector3Length(m_pControllerTransform->Get_Transform(_eCoord)->Get_State(CTransform::STATE_POSITION)
		- _pUser->Get_ControllerTransform()->Get_Transform(_eCoord)->Get_State(CTransform::STATE_POSITION)).m128_f32[0];
}

void CZipline::Ride_Down(_float _fTimeDelta)
{
	if (false == m_isRideDown)
		return;
	
	if ((_uint)CPlayer::ANIM_STATE_2D::PLAYER_ZIPLINE != m_pRidingObject->Get_Body()->Get_CurrentAnimIndex()) {
		m_pRidingObject->Get_Body()->Set_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_ZIPLINE);
		m_pRidingObject->Set_2DDirection(E_DIRECTION::LEFT);
		this->Set_Render(false);
		XMStoreFloat3(&m_vStartPos, m_pControllerTransform->Get_State(CTransform::STATE_POSITION));
	}
	//
	_float fRatio = m_pGameInstance->Calculate_Ratio(&m_fZiplineTime, _fTimeDelta, EASE_IN_OUT);

	if (fRatio >= (1.f - EPSILON)) {
		// Player 떨어지기
		m_pRidingObject->Set_GravityCompOn(true);

		this->Set_Active(false);
	}

	_vector vPos = XMVectorLerp(XMLoadFloat3(&m_vStartPos), XMLoadFloat3(&m_vDestination), fRatio);
	
	m_pRidingObject->Get_ControllerTransform()->Set_State(CTransform::STATE_POSITION, XMVectorSetW(vPos, 1.f));

}

CZipline* CZipline::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CZipline* pInstance = new CZipline(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : Zipline");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CZipline::Clone(void* _pArg)
{
	CZipline* pInstance = new CZipline(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : Zipline");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CZipline::Free()
{
	Safe_Release(m_pRidingObject);

	__super::Free();
}
