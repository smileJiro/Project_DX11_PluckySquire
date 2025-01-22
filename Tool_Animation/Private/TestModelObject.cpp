#include "stdafx.h"
#include "TestModelObject.h"
#include "GameInstance.h"
#include "Test3DModel.h"
#include "Test2DModel.h"
#include "Controller_Model.h"
#include "TestController_Model.h"
#include "Engine_Defines.h"

CTestModelObject::CTestModelObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CModelObject(_pDevice, _pContext)
{
}

CTestModelObject::CTestModelObject(const CTestModelObject& _Prototype)
	:CModelObject(_Prototype)
{
}


HRESULT CTestModelObject::Initialize(void* _pArg)
{
	TESTMODELOBJ_DESC* pDesc = (TESTMODELOBJ_DESC*)_pArg;
    m_iShaderPasses[COORDINATE_2D] = pDesc->iShaderPass_2D;
    m_iShaderPasses[COORDINATE_3D] = pDesc->iShaderPass_3D;
	m_eCurCoord = pDesc->eStartCoord;
    if (FAILED(CPartObject::Initialize(_pArg)))
        return E_FAIL;

    if (FAILED(Ready_TestComponents(pDesc)))
        return E_FAIL;
    // View Matrix는 IdentityMatrix
    XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());

    // Projection Matrix는 Viewport Desc 를 기반으로 생성.
    // 2025-01-16 박예슬 수정 : Viewport Desc -> Rendertarget Size
    _float2 fRTSize = m_pGameInstance->Get_RT_Size(L"Target_Book_2D");

    XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)fRTSize.x, (_float)fRTSize.y, 0.0f, 1.0f));


    return S_OK;
}

void CTestModelObject::Late_Update(_float _fTimeDelta)
{
    /* Add Render Group */
    if (COORDINATE_3D == m_pControllerTransform->Get_CurCoord())
        m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
    else if (COORDINATE_2D == m_pControllerTransform->Get_CurCoord())
        m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);
	CPartObject::Late_Update(_fTimeDelta);
}

HRESULT CTestModelObject::Ready_TestComponents(TESTMODELOBJ_DESC* _pDesc)
{
    _int iStaticLevelID = m_pGameInstance->Get_StaticLevelID();

    switch (_pDesc->eStartCoord)
    {
    case Engine::COORDINATE_2D:
    {
        /* Com_Shader_2D */
        if (FAILED(Add_Component(iStaticLevelID, _pDesc->strShaderPrototypeTag_2D,
            TEXT("Com_Shader_2D"), reinterpret_cast<CComponent**>(&m_pShaderComs[COORDINATE_2D]))))
            return E_FAIL;

        break;
    }
    case Engine::COORDINATE_3D:
    {
        /* Com_Shader_3D */
        if (FAILED(Add_Component(iStaticLevelID, _pDesc->strShaderPrototypeTag_3D,
            TEXT("Com_Shader_3D"), reinterpret_cast<CComponent**>(&m_pShaderComs[COORDINATE_3D]))))
            return E_FAIL;
        break;
    }
    default:
        break;
    }

    CTestController_Model::TESTCONTMODEL_DESC tModelDesc{};
    tModelDesc.eStartCoord = _pDesc->eStartCoord;
    tModelDesc.isCoordChangeEnable = _pDesc->isCoordChangeEnable;
    tModelDesc.iCurLevelID = _pDesc->iCurLevelID;
    tModelDesc.pModel = _pDesc->pModel;


    m_pControllerModel = CTestController_Model::Create(m_pDevice, m_pContext, &tModelDesc);
    if (nullptr == m_pControllerModel)
        return E_FAIL;


    return S_OK;
}

void CTestModelObject::Set_2DProjMatrix(_fmatrix _vProjMatrix)
{
    XMStoreFloat4x4(&m_ProjMatrix, _vProjMatrix);
}

void CTestModelObject::Get_TextureNames(list<wstring>& _outTextureNames)
{
	assert(m_pControllerModel);
    assert(COORDINATE_LAST != m_eCurCoord);
	switch (m_eCurCoord)
	{
	case Engine::COORDINATE_2D:
		static_cast<CTest2DModel*>(m_pControllerModel->Get_Model(m_eCurCoord))->Get_TextureNames(_outTextureNames);
		break;
	case Engine::COORDINATE_3D:
		static_cast<CTest3DModel*>(m_pControllerModel->Get_Model(m_eCurCoord))->Get_TextureNames(_outTextureNames);
		break;
	}
}

HRESULT CTestModelObject::Export_Model(ofstream& _outfile, const _char* _szDirPath, _bool _bExportTextures)
{
	assert(m_pControllerModel); 
	assert(COORDINATE_LAST != m_eCurCoord);
    switch (m_eCurCoord)
    {
    case Engine::COORDINATE_2D:
	    return static_cast<CTest2DModel*>(m_pControllerModel->Get_Model(m_eCurCoord))->Export_Model(_outfile);
    case Engine::COORDINATE_3D:
	    return static_cast<CTest3DModel*>(m_pControllerModel->Get_Model(m_eCurCoord))->Export_Model(_outfile);
    }
    return S_OK;
}


CTestModelObject* CTestModelObject::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CTestModelObject* pInstance = new CTestModelObject(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : TestModelObject");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CTestModelObject::Clone(void* _pArg)
{
    CTestModelObject* pInstance = new CTestModelObject(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : TestModelObject");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CTestModelObject::Free()
{
    __super::Free();
}
