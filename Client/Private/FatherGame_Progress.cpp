#include "stdafx.h"
#include "FatherGame_Progress.h"
#include "GameInstance.h"

CFatherGame_Progress::CFatherGame_Progress(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :m_pDevice(_pDevice)
    ,m_pContext(_pContext)
    ,m_pGameInstance(CGameInstance::GetInstance())
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
    Safe_AddRef(m_pGameInstance);
}

HRESULT CFatherGame_Progress::Initialize(void* _pArg)
{
    m_isClearProgress = false;
    return S_OK;
}

void CFatherGame_Progress::Active_OnEnable()
{
    Progress_Enter();
}

void CFatherGame_Progress::Active_OnDisable()
{
    Progress_Exit();
}

void CFatherGame_Progress::Free()
{
    Safe_Release(m_pGameInstance);
    Safe_Release(m_pContext);
    Safe_Release(m_pDevice);


    __super::Free();
}
