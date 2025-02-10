#include "stdafx.h"
#include "Section_2D_Narration.h"
#include "GameInstance.h"
#include "2DMapObject.h"
#include "Map_2D.h"
#include "Section_Manager.h"
#include "Engine_Macro.h"
#include "MapObjectFactory.h"
#include "Trigger_Manager.h"

CSection_2D_Narration::CSection_2D_Narration(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CSection_2D(_pDevice, _pContext)
{
}

HRESULT CSection_2D_Narration::Initialize(SECTION_2D_DESC* _pDesc, _uint _iPriorityKey)
{
	if (FAILED(__super::Initialize(_pDesc, _iPriorityKey)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSection_2D_Narration::Import(json _SectionJson, _uint _iPriorityKey)
{

#pragma region json 객체의 기본 정보 읽습니다. 하위 주석 참조.
	if (FAILED(__super::Import(_SectionJson, _iPriorityKey)))
		return E_FAIL;
#pragma endregion
	
#pragma region Narraion의 경우, TextureName이 있는지 검사하고 해당 이름을 통해 백그라운드 텍스쳐를 불러옵니다.
	_wstring strSectionBackGroundFileName = L"";

	if(_SectionJson["Section_Info"].contains("Section_TextureName"))
	{ 
		_string strTextureName = _SectionJson["Section_Info"]["Section_TextureName"];
		strSectionBackGroundFileName = StringToWstring(strTextureName);
	}

	if (FAILED(Ready_Map_2D(strSectionBackGroundFileName)))
		return E_FAIL;
#pragma endregion
	return S_OK;
}
HRESULT CSection_2D_Narration::Section_AddRenderGroup_Process()
{
	if (FAILED(Copy_DefaultMap_ToRenderTarget()))
		return E_FAIL;

	if (FAILED(Add_RenderGroup_GameObjects()))
		return E_FAIL;

	return S_OK;
}
;

CSection_2D_Narration* CSection_2D_Narration::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, _uint _iPriorityKey, SECTION_2D_DESC* _pDesc)
{
	CSection_2D_Narration* pInstance = new CSection_2D_Narration(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize(_pDesc, _iPriorityKey)))
	{
		MSG_BOX("Failed Create CSection_2D_Narration");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CSection_2D_Narration* CSection_2D_Narration::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, _uint _iPriorityKey, json _SectionJson)
{
	CSection_2D_Narration* pInstance = new CSection_2D_Narration(_pDevice, _pContext);

	if (FAILED(pInstance->Import(_SectionJson, _iPriorityKey)))
	{
		MSG_BOX("Failed Create CSection_2D_Narration");
		Safe_Release(pInstance);
	}

	return pInstance;
}
void CSection_2D_Narration::Free()
{
	__super::Free();
}
