#include "stdafx.h"
#include "Word_Container.h"

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
	__super::Update(_fTimeDelta);
}

void CWord_Container::Late_Update(_float _fTimeDelta)
{
	__super::Late_Update(_fTimeDelta);
}

HRESULT CWord_Container::Render()
{
	__super::Render();
	return S_OK;

}

CWord_Container* CWord_Container::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	return nullptr;
}

CGameObject* CWord_Container::Clone(void* _pArg)
{
	return nullptr;
}

void CWord_Container::Free()
{
}

void CWord_Container::Interact(CPlayer* _pUser)
{
}

_bool CWord_Container::Is_Interactable(CPlayer* _pUser)
{
	return _bool();
}

_float CWord_Container::Get_Distance(COORDINATE _eCoord, CPlayer* _pUser)
{
	return _float();
}

void CWord_Container::Set_Word(CWord* _pWord)
{
	// 있는 단어가 있으면, 뺀다.
	// 단어를 넣는다.
	// 단어 교체 액션을 실행한다.
	// 단어를 섹션에서 제거한다.
	// 단어를 비활성화한다.
	if (nullptr != m_pMyWord)
	{
		Pop_Word();
		m_pMyWord = _pWord;
	}
}

void CWord_Container::Pop_Word()
{
	//단어가 있다면, 활성화한다
	// 단어를 현재 섹션에 추가한다.
	if (nullptr != m_pMyWord)
	{
		m_pMyWord->Set_Active(true);
		m_pMyWord = nullptr;
	}
}
