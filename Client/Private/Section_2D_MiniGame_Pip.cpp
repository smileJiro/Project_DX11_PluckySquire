#include "stdafx.h"
#include "Section_2D_MiniGame_Pip.h"
#include "GameInstance.h"
#include "2DMapObject.h"
#include "Map_2D.h"
#include "Section_Manager.h"
#include "Engine_Macro.h"
#include "MapObjectFactory.h"
#include "Trigger_Manager.h"
#include "Portal.h"
#include "Blocker.h"

CSection_2D_MiniGame_Pip::CSection_2D_MiniGame_Pip(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CSection_2D_MiniGame(_pDevice, _pContext, MINIGAME, SECTION_2D_BOOK)
{
}

HRESULT CSection_2D_MiniGame_Pip::Initialize(void* _pDesc)
{
	if (FAILED(__super::Initialize(_pDesc)))
		return E_FAIL;


	if (FAILED(Ready_Objects(_pDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CSection_2D_MiniGame_Pip::Ready_Objects(void* _pDesc)
{
	// 섹션 내부에 생성해놓아야 할 객체가 있다면, 여기서 구현
	// 파일 로드든, 하드코딩이든 자유
	// :: 이 함수가 불리는 시점은? 
	//	Level Loader End
	//	Section_Level_Enter
	//						=> 여기!
	//	Section_Level_Enter End
	// Level Object Create
	return S_OK;
}

HRESULT CSection_2D_MiniGame_Pip::Add_GameObject_ToSectionLayer(CGameObject* _pGameObject, _uint _iLayerIndex)
{
	// 섹션에 넣기 전

	// 실구현부 : CSection::Add_GameObject_ToSectionLayer
	HRESULT hr = __super::Add_GameObject_ToSectionLayer(_pGameObject, _iLayerIndex);
	
	//섹션에 넣기 후 
	return hr;
}

HRESULT CSection_2D_MiniGame_Pip::Section_AddRenderGroup_Process()
{
	// 매 프레임 렌더링 직전에 호출되는 함수
	
	
	// 오브젝트 그룹 소트
	Sort_Layer([](const CGameObject* pLeftGameObject, const CGameObject* pRightGameObject)->_bool {
		return XMVectorGetY(pLeftGameObject->Get_FinalPosition()) > XMVectorGetY(pRightGameObject->Get_FinalPosition());
		}, SECTION_2D_PLAYMAP_OBJECT);

	// 레이어 렌더러 삽입
	if (FAILED(Add_RenderGroup_GameObjects()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSection_2D_MiniGame_Pip::Section_Enter(const _wstring& _strPreSectionTag)
{
	return S_OK;
}

HRESULT CSection_2D_MiniGame_Pip::Section_Exit(const _wstring& _strNextSectionTag)
{
	return S_OK;
}


CSection_2D_MiniGame_Pip* CSection_2D_MiniGame_Pip::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* _pDesc)
{
	CSection_2D_MiniGame_Pip* pInstance = new CSection_2D_MiniGame_Pip(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize(_pDesc)))
	{
		MSG_BOX("Failed Create CSection_2D_MiniGame_Pip");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSection_2D_MiniGame_Pip::Free()
{
	__super::Free();
}
