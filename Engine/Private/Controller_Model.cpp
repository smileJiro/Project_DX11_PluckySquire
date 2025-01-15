#include "Controller_Model.h"
#include "GameInstance.h"
#include "2DModel.h"
#include "3DModel.h"

CController_Model::CController_Model(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :m_pDevice(_pDevice)
    , m_pContext(_pContext)
    , m_pGameInstance(CGameInstance::GetInstance())
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
    Safe_AddRef(m_pGameInstance);
}

HRESULT CController_Model::Initialize(CON_MODEL_DESC* _pDesc)
{
    m_eCurCoord = _pDesc->eStartCoord;
    m_isCoordChangeEnable = _pDesc->isCoordChangeEnable;

    if (FAILED(Ready_Models(_pDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CController_Model::Render(_uint iMeshIndex)
{
    return S_OK;
}

HRESULT CController_Model::Change_Coordinate(COORDINATE _eCoordinate)
{
    return S_OK;
}

HRESULT CController_Model::Ready_Models(CON_MODEL_DESC* _pDesc)
{
    switch (m_eCurCoord)
    {
    case Engine::COORDINATE_2D:
    {
        /* Com_Model */
        CComponent* pComponent = static_cast<CComponent*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_COMPONENT, _pDesc->i2DModelPrototypeLevelID, _pDesc->wstr2DModelPrototypeTag, nullptr));
        if (nullptr == pComponent)
            return E_FAIL;
        if (true == m_isCoordChangeEnable)
        {
            /* Com_Model */
            pComponent = static_cast<CComponent*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_COMPONENT, _pDesc->i3DModelPrototypeLevelID, _pDesc->wstr3DModelPrototypeTag, nullptr));
            if (nullptr == pComponent)
                return E_FAIL;

            m_p3DModelCom = static_cast<C3DModel*>(pComponent);
        }
    }
    break;
    case Engine::COORDINATE_3D:
    {
        /* Com_Model */
        CComponent* pComponent = static_cast<CComponent*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_COMPONENT, _pDesc->i3DModelPrototypeLevelID, _pDesc->wstr3DModelPrototypeTag, nullptr));
        if (nullptr == pComponent)
            return E_FAIL;

        m_p3DModelCom = static_cast<C3DModel*>(pComponent);
        if (true == m_isCoordChangeEnable)
        {
            /* Com_VIBuffer */
            pComponent = static_cast<CComponent*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_COMPONENT, 1, TEXT("Prototype_Component_VIBuffer_Rect"), nullptr));
            if (nullptr == pComponent)
                return E_FAIL;

            m_p2DModelCom = static_cast<C2DModel*>(pComponent);
        }
    }
    break;
    default:
        break;
    }


    return S_OK;
}

CController_Model* CController_Model::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, CON_MODEL_DESC* _pDesc)
{
    CController_Model* pInstance = new CController_Model(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize(_pDesc)))
    {
        MSG_BOX("Failed to Created : CController_Model");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CController_Model::Free()
{
    Safe_Release(m_p3DModelCom);
    Safe_Release(m_p2DModelCom);

    Safe_Release(m_pGameInstance);
    Safe_Release(m_pContext);
    Safe_Release(m_pDevice);


    __super::Free();
}
