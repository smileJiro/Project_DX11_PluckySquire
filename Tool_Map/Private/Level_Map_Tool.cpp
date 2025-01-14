#include "stdafx.h"
#include "Level_Map_Tool.h"
#include "GameInstance.h"
#include "Camera_Free.h"

CLevel_Map_Tool::CLevel_Map_Tool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CLevel(_pDevice, _pContext)
{
}

HRESULT CLevel_Map_Tool::Initialize()
{
	Ready_Lights();
	CGameObject* pCameraTarget = nullptr;
	Ready_Layer_Player(TEXT("Layer_Player"), &pCameraTarget);
	Ready_Layer_Camera(TEXT("Layer_Camera"), pCameraTarget);
	Ready_Layer_TestTerrain(TEXT("Layer_Terrain"));
	return S_OK;
}

void CLevel_Map_Tool::Update(_float _fTimeDelta)
{
}

HRESULT CLevel_Map_Tool::Render()
{
#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("게임플레이레벨입니다."));
#endif

	return S_OK;
}

HRESULT CLevel_Map_Tool::Ready_Lights()
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


HRESULT CLevel_Map_Tool::Ready_Layer_Camera(const _wstring& _strLayerTag, CGameObject* _pTarget)
{
	CCamera_Free::CAMERA_FREE_DESC		Desc{};

	//Desc.fSpeedPerSec = 10.f;
	//Desc.fRotationPerSec = XMConvertToRadians(180.f);
	Desc.fMouseSensor = 0.1f;
	Desc.eZoomLevel = Engine::CCamera::ZOOM_LEVEL::NORMAL;
	Desc.fAspect = static_cast<_float>(g_iWinSizeX) / g_iWinSizeY;
	Desc.fNear = 0.1f;
	Desc.fFar = 1000.f;
	Desc.vEye = _float3(0.f, 10.f, -7.f);
	Desc.vAt = _float3(0.f, 0.f, 0.f);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_Camera_Free"),
		LEVEL_TOOL_MAP, _strLayerTag, &Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Map_Tool::Ready_Layer_Player(const _wstring& _strLayerTag, CGameObject** _ppOut)
{

	return S_OK;
}

HRESULT CLevel_Map_Tool::Ready_Layer_TestTerrain(const _wstring& _strLayerTag)
{
	return S_OK;
}


CLevel_Map_Tool* CLevel_Map_Tool::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CLevel_Map_Tool* pInstance = new CLevel_Map_Tool(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Map_Tool");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Map_Tool::Free()
{
	__super::Free();
}
