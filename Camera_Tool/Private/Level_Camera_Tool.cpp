#include "stdafx.h"
#include "Level_Camera_Tool.h"
#include "Level_Loading.h"
#include "GameInstance.h"

CLevel_Camera_Tool::CLevel_Camera_Tool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CLevel(_pDevice, _pContext)
{
}

HRESULT CLevel_Camera_Tool::Initialize()
{
	return S_OK;
}

void CLevel_Camera_Tool::Update(_float _fTimeDelta)
{
	ImGui::Begin("Test");


	//ImGui::Text("Target_Normal");
	///* Image Render */
	//ImVec2 imageSize(800, 450); // 이미지 크기 설정
	//ID3D11ShaderResourceView* pSelectImage = m_pGameInstance->Get_RT_SRV(TEXT("Target_Normal"));
	//if (nullptr != pSelectImage)
	//{
	//	ImGui::Image((ImTextureID)(uintptr_t)pSelectImage, imageSize);
	//}


	ImGui::End();
}

HRESULT CLevel_Camera_Tool::Render()
{
#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("Camera Tool Level"));
#endif

	return S_OK;
}

CLevel_Camera_Tool* CLevel_Camera_Tool::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CLevel_Camera_Tool* pInstance = new CLevel_Camera_Tool(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Camera_Tool");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Camera_Tool::Free()
{
	__super::Free();
}
