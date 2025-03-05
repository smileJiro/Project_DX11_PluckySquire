#include "stdafx.h"
#include "Section_2D_MiniGame.h"
#include "GameInstance.h"
#include "2DMapObject.h"
#include "Map_2D.h"
#include "Section_Manager.h"
#include "Engine_Macro.h"
#include "MapObjectFactory.h"
#include "Trigger_Manager.h"
#include "Portal.h"
#include "Blocker.h"

CSection_2D_MiniGame::CSection_2D_MiniGame(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, SECTION_2D_PLAY_TYPE _ePlayType, SECTION_2D_RENDER_TYPE _eRenderType)
	:CSection_2D(_pDevice, _pContext, _ePlayType, _eRenderType)
{
}

HRESULT CSection_2D_MiniGame::Initialize(void* _pDesc)
{
	if (FAILED(__super::Initialize(_pDesc)))
		return E_FAIL;


	SECTION_2D_MINIGAME_DESC* pDesc = static_cast<SECTION_2D_MINIGAME_DESC*>(_pDesc);
	
	m_strMiniGameName = pDesc->strMiniGameName	;
	m_eMiniGameType =  pDesc->eMiniGameType;
	




	return S_OK;
}

HRESULT CSection_2D_MiniGame::Add_GameObject_ToSectionLayer(CGameObject* _pGameObject, _uint _iLayerIndex)
{
	HRESULT hr = __super::Add_GameObject_ToSectionLayer(_pGameObject, _iLayerIndex);
	return hr;
}

HRESULT CSection_2D_MiniGame::Section_AddRenderGroup_Process()
{
	// 오브젝트 그룹 소트
	Sort_Layer([](const CGameObject* pLeftGameObject, const CGameObject* pRightGameObject)->_bool {
		return XMVectorGetY(pLeftGameObject->Get_FinalPosition()) > XMVectorGetY(pRightGameObject->Get_FinalPosition());
		}, SECTION_2D_PLAYMAP_OBJECT);

	// 레이어 렌더러 삽입
	if (FAILED(Add_RenderGroup_GameObjects()))
		return E_FAIL;

	return S_OK;
}

void CSection_2D_MiniGame::Free()
{
	__super::Free();
}
