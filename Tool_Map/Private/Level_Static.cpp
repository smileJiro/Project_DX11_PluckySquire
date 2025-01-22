#include "stdafx.h"
#include "Level_Static.h"
#include "Level_Loading.h"
#include "GameInstance.h"

CLevel_Static::CLevel_Static(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CLevel(_pDevice, _pContext)
{
}

HRESULT CLevel_Static::Initialize()
{
 	return S_OK;
}

void CLevel_Static::Update(_float _fTimeDelta)
{
	Event_LevelChange(LEVEL_LOADING, LEVEL_TOOL_3D_MAP);
}

HRESULT CLevel_Static::Render()
{
#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("스태틱 레벨입니다."));
#endif
	return S_OK;
}

CLevel_Static* CLevel_Static::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CLevel_Static* pInstance = new CLevel_Static(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Static");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Static::Free()
{
	__super::Free();
}
