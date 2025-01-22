#include "stdafx.h"
#include "TestController_Model.h"
#include "GameInstance.h"
#include "2DModel.h"
#include "3DModel.h"

CTestController_Model::CTestController_Model(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CController_Model(_pDevice, _pContext)
{
}

HRESULT CTestController_Model::Initialize(TESTCONTMODEL_DESC* _pDesc)
{
    m_eCurCoord = _pDesc->eStartCoord;
    m_isCoordChangeEnable = false;

    m_ModelComs[m_eCurCoord] = _pDesc->pModel;
    return S_OK;
}

CTestController_Model* CTestController_Model::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, TESTCONTMODEL_DESC* _pDesc)
{
    CTestController_Model* pInstance = new CTestController_Model(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize(_pDesc)))
    {
        MSG_BOX("Failed to Created : TestController_Model");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CTestController_Model::Free()
{
    __super::Free();
}
