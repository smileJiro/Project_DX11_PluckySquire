#include "stdafx.h"
#include "Word_Container.h"
#include "Section_Manager.h"
#include "Player.h"
#include "Collider.h"

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
	return __super::Initialize(_pArg);
}

void CWord_Container::Priority_Update(_float _fTimeDelta)
{
	__super::Priority_Update(_fTimeDelta);
}

void CWord_Container::Update(_float _fTimeDelta)
{

	//__super::Update(_fTimeDelta);
}

void CWord_Container::Late_Update(_float _fTimeDelta)
{
	//__super::Late_Update(_fTimeDelta);
}

HRESULT CWord_Container::Render()
{
	//__super::Render();
	return S_OK;

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
	__super::Free();
}

void CWord_Container::Interact(CPlayer* _pUser)
{
	if (_pUser->Is_CarryingObject())
	{
		//CCarriableObject* pObj =  _pUser->Get_CarryingObject();
		//CWord* pWord = dynamic_cast<CWord*>(pObj);
		//if (nullptr != pWord)
		//{
		//	Set_Word(pWord);
		//}
	}
	else 
	{
		if (nullptr != m_pMyWord)
		{
			Pop_Word();
			_pUser->Set_CarryingObject(m_pMyWord);
		}
	}
}

_bool CWord_Container::Is_Interactable(CPlayer* _pUser)
{
	return _bool();
}

_float CWord_Container::Get_Distance(COORDINATE _eCoord, CPlayer* _pUser)
{
	return _float();
}

void CWord_Container::On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{

	if (_pOtherCollider->Get_CollisionGroupID() == OBJECT_GROUP::PLAYER_PROJECTILE)
	{
		if (m_pMyWord != nullptr)
		{
			Pop_Word();
		}
	}

}

void CWord_Container::Set_Word(CWord* _pWord)
{
	// 있는 단어가 있으면, 뺀다.
	// 단어를 넣는다.
	// 단어 교체 액션을 실행한다.
	// 단어를 섹션에서 제거한다.
	// 단어를 비활성화한다.
	if (nullptr != m_pMyWord)
		Pop_Word();

	m_pMyWord = _pWord;

	SECTION_MGR->Word_Action_To_Section(m_strSectionName, m_iControllerIndex, m_iContainerIndex, m_pMyWord->Get_WordType());
	SECTION_MGR->Remove_GameObject_FromSectionLayer(m_strSectionName, m_pMyWord);
	m_pMyWord->Set_Active(false);
}

void CWord_Container::Pop_Word()
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
		//m_pMyWord->Set_Position()
		//m_pMyWord = nullptr;
	}
}
