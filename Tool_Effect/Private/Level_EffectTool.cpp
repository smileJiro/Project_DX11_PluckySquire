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
	if (FAILED(Ready_Layer_Effect(TEXT("Layer_Effect"))))
		return E_FAIL;
	if (FAILED(Ready_Layer_TestTerrain(TEXT("Layer_Terrain"))))
		return E_FAIL;

	return S_OK;
}

void CLevel_EffectTool::Update(_float _fTimeDelta)
{

}

HRESULT CLevel_EffectTool::Render()
{
#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("이펙트 툴입니다."));
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
	CCamera_Free::CAMERA_FREE_DESC Desc{};

	Desc.fMouseSensor = 0.1f;

	Desc.fFovy = XMConvertToRadians(30.f);
	Desc.fAspect = static_cast<_float>(g_iWinSizeX) / g_iWinSizeY;
	Desc.fNear = 0.1f;
	Desc.fFar = 1000.f;
	Desc.vEye = _float3(0.f, 0.f, -5.f);
	Desc.vAt = _float3(0.f, 0.f, 1.f);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_Camera_Free"),
		LEVEL_TOOL, _strLayerTag, &Desc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CLevel_EffectTool::Ready_Layer_Effect(const _wstring& _strLayerTag)
{
	CEffect::EFFECT_DESC Desc = {};
	Desc.eStartCoord = COORDINATE_3D;
	Desc.iCurLevelID = LEVEL_TOOL;
	Desc.isCoordChangeEnable = false;
	Desc.iProtoShaderLevel = LEVEL_STATIC;
	Desc.szShaderTag = L"Prototype_Component_Shader_VtxPointInstance";

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_TOOL, TEXT("Prototype_Effect_Temp"),
		LEVEL_TOOL, _strLayerTag, &Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_EffectTool::Ready_Layer_TestTerrain(const _wstring& _strLayerTag)
{
	CGameObject::GAMEOBJECT_DESC Desc = {};
	Desc.iCurLevelID = LEVEL_TOOL;
	Desc.isCoordChangeEnable = false;
	Desc.eStartCoord = COORDINATE_3D;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_Object_Background"),
		LEVEL_TOOL, _strLayerTag, &Desc)))
		return E_FAIL;

	return S_OK;
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
