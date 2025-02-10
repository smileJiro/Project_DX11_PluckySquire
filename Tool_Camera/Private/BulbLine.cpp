#include "stdafx.h"
#include "BulbLine.h"

#include "GameInstance.h"
#include "Bulb.h"

CBulbLine::CBulbLine(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : m_pDevice(_pDevice)
    , m_pContext(_pContext)
    , m_pGameInstance(CGameInstance::GetInstance())
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
    Safe_AddRef(m_pGameInstance);
}

CBulbLine::CBulbLine(const CBulbLine& _Prototype)
    : m_pDevice(_Prototype.m_pDevice)
    , m_pContext(_Prototype.m_pContext)
    , m_pGameInstance(_Prototype.m_pGameInstance)
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
    Safe_AddRef(m_pGameInstance);
}

HRESULT CBulbLine::Initialize(void* pArg)
{
    return S_OK;
}

HRESULT CBulbLine::Initialize_Clone()
{
    return S_OK;
}

void CBulbLine::Priority_Update(_float fTimeDelta)
{
}

void CBulbLine::Update(_float fTimeDelta)
{
}

void CBulbLine::Late_Update(_float fTimeDelta)
{
}

void CBulbLine::Set_Line(CGameObject* _pPoint)
{
   // for(auto& BulbLine : m_BulbLines)
}

CBulbLine* CBulbLine::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* pArg)
{
    CBulbLine* pInstance = new CBulbLine(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CBulbLine");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CBulbLine* CBulbLine::Clone()
{
    CBulbLine* pInstance = new CBulbLine(*this);

    if (FAILED(pInstance->Initialize_Clone()))
    {
        MSG_BOX("Failed to Cloned : CBulbLine");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBulbLine::Free()
{
    Safe_Release(m_pGameInstance);
    Safe_Release(m_pContext);
    Safe_Release(m_pDevice);

    __super::Free();
}
