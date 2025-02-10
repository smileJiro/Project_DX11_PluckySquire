#include "stdafx.h"
#include "Section_2D_PlayMap_Sksp.h"
#include "GameInstance.h"
#include "2DMapObject.h"
#include "Map_2D.h"
#include "Section_Manager.h"
#include "Engine_Macro.h"
#include "MapObjectFactory.h"
#include "Trigger_Manager.h"

CSection_2D_PlayMap_Sksp::CSection_2D_PlayMap_Sksp(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CSection_2D_PlayMap(_pDevice, _pContext, SECTION_2D_SPSK)
{
}

HRESULT CSection_2D_PlayMap_Sksp::Initialize(SECTION_2D_PLAYMAP_DESC* _pDesc, _uint _iPriorityKey)
{
	if (FAILED(__super::Initialize(_pDesc, _iPriorityKey)))
		return E_FAIL;
	return S_OK;
}

HRESULT CSection_2D_PlayMap_Sksp::Add_GameObject_ToSectionLayer(CGameObject* _pGameObject, _uint _iLayerIndex)
{
	HRESULT hr = __super::Add_GameObject_ToSectionLayer(_pGameObject, _iLayerIndex);
	return hr;
}

HRESULT CSection_2D_PlayMap_Sksp::Section_AddRenderGroup_Process()
{
	// 텍스쳐 복사
	if (FAILED(Copy_DefaultMap_ToRenderTarget()))
		return E_FAIL;

	// 오브젝트 그룹 소트
	Sort_Layer([](const CGameObject* pLeftGameObject, const CGameObject* pRightGameObject)->_bool {
		return XMVectorGetY(pLeftGameObject->Get_FinalPosition()) > XMVectorGetY(pRightGameObject->Get_FinalPosition());
		}, SECTION_2D_PLAYMAP_OBJECT);

	// 레이어 렌더러 삽입
	if (FAILED(Add_RenderGroup_GameObjects()))
		return E_FAIL;

	return S_OK;
}

CSection_2D_PlayMap_Sksp* CSection_2D_PlayMap_Sksp::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, _uint _iPriorityKey, json _SectionJson)
{
	CSection_2D_PlayMap_Sksp* pInstance = new CSection_2D_PlayMap_Sksp(_pDevice, _pContext);

	if (FAILED(pInstance->Import(_SectionJson, _iPriorityKey)))
	{
		MSG_BOX("Failed Create CSection_2D_PlayMap_Sksp");
		Safe_Release(pInstance);
	}

	return pInstance;
}
void CSection_2D_PlayMap_Sksp::Free()
{
	__super::Free();
}
