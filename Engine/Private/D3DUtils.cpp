#include "D3DUtils.h"
#include "GameInstance.h"

CD3DUtils::CD3DUtils(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :m_pDevice(_pDevice)
    ,m_pContext(_pContext)
    ,m_pGameInstance(CGameInstance::GetInstance())
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
    Safe_AddRef(m_pGameInstance);
}

HRESULT CD3DUtils::Initialize()
{
    return S_OK;
}

CD3DUtils* CD3DUtils::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CD3DUtils* pInstance = new CD3DUtils(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CD3DUtils");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CD3DUtils::Free()
{
    Safe_Release(m_pGameInstance);
    Safe_Release(m_pContext);
    Safe_Release(m_pDevice);

    __super::Free();
}
