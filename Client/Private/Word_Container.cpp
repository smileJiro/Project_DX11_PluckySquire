#include "stdafx.h"
#include "Word_Container.h"
#include "Section_Manager.h"
#include "Player.h"
#include "Collider.h"
//#include "Collider_Circle.h"
#include "Collider_AABB.h"
#include "Word_Controller.h"
#include "Effect2D_Manager.h"

CWord_Container::CWord_Container(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CPartObject(_pDevice,_pContext)
{
}

CWord_Container::CWord_Container(const CWord_Container& _Prototype)
	:CPartObject(_Prototype)

{
}

HRESULT CWord_Container::Initialize_Prototype()
{
	return __super::Initialize_Prototype();
}

HRESULT CWord_Container::Initialize(void* _pArg)
{
	WORD_CONTAINER_DESC* pDesc = static_cast<WORD_CONTAINER_DESC*>(_pArg);
	pDesc->iObjectGroupID = OBJECT_GROUP::INTERACTION_OBEJCT;
	pDesc->eStartCoord = COORDINATE_2D;
	pDesc->isCoordChangeEnable= false;

	HRESULT hr = __super::Initialize(_pArg);

	m_iContainerIndex = pDesc->iContainerIndex;
	m_iControllerIndex = pDesc->iControllerIndex;
	m_pOnwer = pDesc->pOnwer;


	m_p2DColliderComs.resize(1);
	CCollider_AABB::COLLIDER_AABB_DESC CircleDesc = {};
	CircleDesc.pOwner = this;
	CircleDesc.vExtents = {40.f,30.f};
	CircleDesc.vScale = { 1.0f, 1.0f };
	CircleDesc.vOffsetPosition = { 0.f, 0.f };
	CircleDesc.isBlock = false;
	CircleDesc.isTrigger = true;
	CircleDesc.iCollisionGroupID = OBJECT_GROUP::WORD_GAME;
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_2DCollider"), reinterpret_cast<CComponent**>(&m_p2DColliderComs[0]), &CircleDesc)))
		return E_FAIL;


	return hr;
}

void CWord_Container::Priority_Update(_float _fTimeDelta)
{
	__super::Priority_Update(_fTimeDelta);
}

void CWord_Container::Update(_float _fTimeDelta)
{
	__super::Update(_fTimeDelta);

	if (nullptr != m_pMyWord)
	{
		m_fEffectAccTime += _fTimeDelta;
		if (m_fEffectAccTime > m_fEffectInterval)
		{
			_vector vPos = Get_FinalPosition();
			_matrix matFX = XMMatrixTranslation(XMVectorGetX(vPos), XMVectorGetY(vPos),0.f);
			//_matrix matFX = Get_ControllerTransform()->Get_WorldMatrix();
			_wstring strFXTag = L"Word_HitEffect";
			CEffect2D_Manager::GetInstance()->Play_Effect(strFXTag, m_strSectionName, matFX,0.f,3,false, SECTION_2D_PLAYMAP_EFFECT);
			m_fEffectAccTime = 0.f;
		}
	}

	//if (nullptr != m_pMyWord)
	//{
	//	CWord::WORD_MODE eMode = m_pMyWord->Get_WordMode();
	//	_bool isComplete = m_pMyWord->Is_Complete();

	//	switch (eMode)
	//	{
	//		case Client::CWord::WORD_MODE_ENTER_SETUP:
	//			if (isComplete)
	//			{
	//				m_pMyWord->Set_WordMode();
	//				SECTION_MGR->Word_Action_To_Section(m_strSectionName, m_iControllerIndex, m_iContainerIndex, m_pMyWord->Get_WordType());
	//				SECTION_MGR->Remove_GameObject_FromSectionLayer(m_strSectionName, m_pMyWord);
	//				m_pMyWord->Set_Active(false);
	//				m_pOnwer->Update_Text();
	//			}
	//			break;
	//		case Client::CWord::WORD_MODE_EXIT_SETUP:
	//			
	//			break;
	//		default:
	//			break;
	//	}


	//	

	//}

}

void CWord_Container::Late_Update(_float _fTimeDelta)
{
	__super::Late_Update(_fTimeDelta);
}

HRESULT CWord_Container::Render()
{
	#ifdef _DEBUG
		m_p2DColliderComs[0]->Render(SECTION_MGR->Get_Section_RenderTarget_Size(m_strSectionName));
	#endif // _DEBUG
	return S_OK;

}

void CWord_Container::On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
	if (OBJECT_GROUP::INTERACTION_OBEJCT == _pOtherObject->Get_ObjectGroupID())
	{
		CWord* pWord = dynamic_cast<CWord*>(_pOtherObject);
		if (nullptr != pWord)
		{
			if (pWord->Is_LayedDown())
			{
				_vector fDir = XMVector2Normalize(XMLoadFloat2(&_pMyCollider->Get_Position()) - XMLoadFloat2(&_pOtherCollider->Get_Position()));
				_float3 fPos = {};
				XMStoreFloat3(&fPos, fDir);
				// 위 치 설 정 
				Pop_Word(fPos);
				Set_Word(pWord);
				pWord->Set_LayDown(false);
			}
		}
	}

}




CWord_Container* CWord_Container::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CWord_Container* pInstance = new CWord_Container(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Created CWord_Container Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* CWord_Container::Clone(void* _pArg)
{
	CWord_Container* pInstance = new CWord_Container(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Clone CWord_Container Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CWord_Container::Free()
{
	Safe_Release(m_pMyWord);
	__super::Free();
}

void CWord_Container::Interact(CPlayer* _pUser)
{
	//이제 사실상 Interact오브젝트일 필요가 없어졌어요.
	//그냥 OnCOntatct2DSaty에서 word가 들어오면 흡수함.
	if (_pUser->Is_CarryingObject())
	{
		CCarriableObject* pObj =  _pUser->Get_CarryingObject();
		//CWord* pWord = dynamic_cast<CWord*>(pObj);
		//if (nullptr != pWord)
		//{
		//	Set_Word(pWord);
		//	m_pOnwer->Update_Text();
		//	_pUser->Set_CarryingObject(nullptr);
		//	pWord->Set_Carrier(nullptr);
		//}
	}
	else 
	{
		/*if (nullptr != m_pMyWord)
		{
			Pop_Word();
			_pUser->Set_CarryingObject(m_pMyWord);
		}*/
	}
}

_bool CWord_Container::Is_Interactable(CPlayer* _pUser)
{
	return false;//_pUser->Is_CarryingObject();
}

_float CWord_Container::Get_Distance(COORDINATE _eCoord, CPlayer* _pUser)
{
	return XMVector3Length(m_pControllerTransform->Get_Transform(_eCoord)->Get_State(CTransform::STATE_POSITION)
		- _pUser->Get_ControllerTransform()->Get_Transform(_eCoord)->Get_State(CTransform::STATE_POSITION)).m128_f32[0];
}

void CWord_Container::On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
	if (_pOtherCollider->Get_CollisionGroupID() == OBJECT_GROUP::PLAYER_PROJECTILE)
	{
		if (m_pMyWord != nullptr)
		{
			// 위 치 설 정 
			_vector fDir = XMVector2Normalize(XMLoadFloat2(&_pMyCollider->Get_Position()) - XMLoadFloat2(&_pOtherCollider->Get_Position()));
			_float3 fPos = {};
			XMStoreFloat3(&fPos, fDir);
			// 위 치 설 정 
			Pop_Word(fPos);
			m_pOnwer->Update_Text();
		}
	}

}

void CWord_Container::Set_Word(CWord* _pWord, CWord::WORD_MODE _eMode, _bool _isWordActionRegister)
{


	// 있는 단어가 있으면, 지운다.
	// 단어를 넣는다.
	// 단어 교체 액션을 실행한다.
	// 단어를 섹션에서 제거한다.
	// 단어를 비활성화한다.
	Safe_Release(m_pMyWord);

	m_fEffectAccTime = 0.f;

	m_pMyWord = _pWord;
	Safe_AddRef(m_pMyWord);
	SECTION_MGR->Word_Action_To_Section(m_strSectionName, m_iControllerIndex, m_iContainerIndex, m_pMyWord->Get_WordType(), _isWordActionRegister);
	SECTION_MGR->Remove_GameObject_FromSectionLayer(m_strSectionName, m_pMyWord);
	m_pMyWord->Set_WordMode();
	m_pMyWord->Set_Active(false);
	m_pOnwer->Update_Text();

}

void CWord_Container::Pop_Word(_float3 _fDir)
{
	//단어가 있다면, 활성화한다
	// 단어를 현재 섹션에 추가한다.
	if (nullptr != m_pMyWord)
	{
		m_pMyWord->Set_Active(true);
		SECTION_MGR->Add_GameObject_ToSectionLayer(m_strSectionName, m_pMyWord, SECTION_2D_PLAYMAP_WORD);
		
		_vector vPos = Get_FinalPosition();
		vPos = XMVectorSetX(vPos, XMVectorGetX(vPos) + 10.f);
		vPos = XMVectorSetY(vPos, XMVectorGetY(vPos) - 10.f);
		m_pMyWord->Set_Position(vPos);
		m_pMyWord->Set_WordMode(CWord::WORD_MODE_EXIT_SETUP);
		m_pMyWord->Set_OutputDirection(_fDir);
		
		Safe_Release(m_pMyWord);
		m_pMyWord = nullptr;
	}
}
