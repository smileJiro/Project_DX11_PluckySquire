#include "stdafx.h"
#include "Level_Logo.h"
#include "Level_Loading.h"
#include "GameInstance.h"

CLevel_Logo::CLevel_Logo(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CLevel(_pDevice, _pContext)
{
}

HRESULT CLevel_Logo::Initialize()
{
 	return S_OK;
}

void CLevel_Logo::Update(_float _fTimeDelta)
{
	if (KEY_DOWN(KEY::ENTER))
		Event_LevelChange(LEVEL_LOADING, LEVEL_GAMEPLAY);


	ImGui::Begin("Test");


	ImGui::Text("Target_Normal");
	/* Image Render */
	ImVec2 imageSize(800, 450); // 이미지 크기 설정
	ID3D11ShaderResourceView* pSelectImage = m_pGameInstance->Get_RT_SRV(TEXT("Target_Normal"));
	if (nullptr != pSelectImage)
	{
		ImGui::Image((ImTextureID)(uintptr_t)pSelectImage, imageSize);
	}


	ImGui::End();

}

HRESULT CLevel_Logo::Render()
{
#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("로고레벨입니다."));
#endif
	return S_OK;
}

CLevel_Logo* CLevel_Logo::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CLevel_Logo* pInstance = new CLevel_Logo(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Logo");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Logo::Free()
{
	__super::Free();
}
