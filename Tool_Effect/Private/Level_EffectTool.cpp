#include "pch.h"
#include "GameInstance.h"
#include "Level_EffectTool.h"


CLevel_EffectTool::CLevel_EffectTool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CLevel(_pDevice, _pContext)
{
}

HRESULT CLevel_EffectTool::Initialize()
{
	if (FAILED(Ready_Lights()))
		return E_FAIL;
	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;
	if (FAILED(Ready_Layer_TestTerrain(TEXT("Layer_Terrain"))))
		return E_FAIL;



}

void CLevel_EffectTool::Update(_float _fTimeDelta)
{

}

HRESULT CLevel_EffectTool::Render()
{
#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("게임플레이레벨입니다."));
#endif

	return S_OK;
}

HRESULT CLevel_EffectTool::Ready_Lights()
{
	LIGHT_DESC LightDesc{};

	ZeroMemory(&LightDesc, sizeof LightDesc);

	LightDesc.eType = LIGHT_DESC::TYPE_DIRECTOINAL;
	LightDesc.vDirection = _float4(-1.f, -1.f, 0.5f, 0.f);
	LightDesc.vDiffuse = _float4(1.0f, 1.0f, 1.0f, 1.f);
	LightDesc.vAmbient = _float4(0.6f, 0.6f, 0.6f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CLevel_EffectTool::Ready_Layer_Camera(const _wstring& _strLayerTag)
{
	return E_NOTIMPL;
}

HRESULT CLevel_EffectTool::Ready_Layer_TestTerrain(const _wstring& _strLayerTag)
{
	return E_NOTIMPL;
}

CLevel_EffectTool* CLevel_EffectTool::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CLevel_EffectTool* pInstance = new CLevel_EffectTool(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_EffectTool");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_EffectTool::Free()
{
	__super::Free();

}
