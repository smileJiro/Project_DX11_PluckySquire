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

    XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.0f, 1.0f));


    return S_OK;
}

void CTestModelObject::Late_Update(_float _fTimeDelta)
{
    /* Add Render Group */
    if (COORDINATE_3D == m_pControllerTransform->Get_CurCoord())
        Register_RenderGroup(RG_3D, PR3D_GEOMETRY);
    else if (COORDINATE_2D == m_pControllerTransform->Get_CurCoord())
        Register_RenderGroup(RG_3D, PR3D_UI);
	CPartObject::Late_Update(_fTimeDelta);
}

HRESULT CTestModelObject::Render()
{
    if (COORDINATE_2D == m_pControllerTransform->Get_CurCoord())
    {
        m_pShaderComs[COORDINATE_2D]->Bind_Matrix("g_ViewMatrix",&m_ViewMatrix);
        m_pShaderComs[COORDINATE_2D]->Bind_Matrix("g_ProjMatrix",&m_ProjMatrix);
    }
    return __super::Render();
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

void CTestModelObject::Set_Progerss(_float _fTrackPos)
{
    assert(m_pControllerModel);
    assert(COORDINATE_LAST != m_eCurCoord);
    switch (m_eCurCoord)
    {
    case Engine::COORDINATE_2D:
        return static_cast<CTest2DModel*>(m_pControllerModel->Get_Model(m_eCurCoord))->Set_Progerss(_fTrackPos);
    case Engine::COORDINATE_3D:
    {
        static_cast<CTest3DModel*>(m_pControllerModel->Get_Model(m_eCurCoord))->Set_Progress(_fTrackPos);
        m_pControllerModel->Get_Model(m_eCurCoord)->Play_Animation(0);
        return;
    }
    }
}

void CTestModelObject::Set_AnimSpeedMagnifier(COORDINATE _eCoord, _uint iAnimIndex, _float _fMagnifier)
{
    assert(m_pControllerModel);
    assert(COORDINATE_LAST != _eCoord);
	m_pControllerModel->Get_Model(_eCoord)->Set_AnimSpeedMagnifier(iAnimIndex,_fMagnifier);
}


void CTestModelObject::Get_TextureNames(set<wstring>& _outTextureNames)
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

_uint CTestModelObject::Get_AnimationCount()
{
    return m_pControllerModel->Get_Model(m_eCurCoord)->Get_AnimCount();
}


_float CTestModelObject::Get_Progress()
{
    assert(m_pControllerModel);
    assert(COORDINATE_LAST != m_eCurCoord);
    switch (m_eCurCoord)
    {
    case Engine::COORDINATE_2D:
        return static_cast<CTest2DModel*>(m_pControllerModel->Get_Model(m_eCurCoord))->Get_Progerss();
    case Engine::COORDINATE_3D:
        return static_cast<CTest3DModel*>(m_pControllerModel->Get_Model(m_eCurCoord))->Get_Progress();
    default:
        return -1.f;
    }
   
}

_float CTestModelObject::Get_AnimSpeedMagnifier(COORDINATE _eCoord, _uint iAnimIndex)
{
    assert(m_pControllerModel);
    assert(COORDINATE_LAST != _eCoord);
    switch (_eCoord)
    {
    case Engine::COORDINATE_2D:
        return static_cast<CTest2DModel*>(m_pControllerModel->Get_Model(_eCoord))->Get_AnimSpeedMagnifier(iAnimIndex);
    case Engine::COORDINATE_3D:
        return static_cast<CTest3DModel*>(m_pControllerModel->Get_Model(_eCoord))->Get_AnimSpeedMagnifier(iAnimIndex);
    default:
        return -1.f;
    }
}

_bool CTestModelObject::Is_LoopAnimation(COORDINATE _eCoord, _uint iAnimIndex)
{
    assert(m_pControllerModel);
    assert(COORDINATE_LAST != _eCoord);
    switch (_eCoord)
    {
    case Engine::COORDINATE_2D:
        return static_cast<CTest2DModel*>(m_pControllerModel->Get_Model(_eCoord))->Is_LoopAnimation(iAnimIndex);
    case Engine::COORDINATE_3D:
        return static_cast<CTest3DModel*>(m_pControllerModel->Get_Model(_eCoord))->Is_LoopAnimation(iAnimIndex);
    default:
        return false;
    }
}

void CTestModelObject::Get_AnimatinNames(list<string>& _Names)
{
    assert(m_pControllerModel);
    assert(COORDINATE_LAST != m_eCurCoord);
    switch (m_eCurCoord)
    {
    case Engine::COORDINATE_2D:
        return static_cast<CTest2DModel*>(m_pControllerModel->Get_Model(m_eCurCoord))->Get_AnimationNames(_Names);
    case Engine::COORDINATE_3D:
        return static_cast<CTest3DModel*>(m_pControllerModel->Get_Model(m_eCurCoord))->Get_AnimationNames(_Names);
    default:
        return;
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
    default:
        return E_FAIL;
    }
    return S_OK;
}

HRESULT CTestModelObject::Copy_Textures(const filesystem::path& _szDestPath)
{
    assert(m_pControllerModel);
    assert(COORDINATE_LAST != m_eCurCoord);
    switch (m_eCurCoord)
    {
    case Engine::COORDINATE_2D:
        return static_cast<CTest2DModel*>(m_pControllerModel->Get_Model(m_eCurCoord))->Copy_Textures(_szDestPath);
    case Engine::COORDINATE_3D:
        return static_cast<CTest3DModel*>(m_pControllerModel->Get_Model(m_eCurCoord))->Copy_Textures(_szDestPath);
    default:
        return E_FAIL;
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
