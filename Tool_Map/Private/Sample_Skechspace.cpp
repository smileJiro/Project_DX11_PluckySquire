#include "stdafx.h"
#include "Sample_Skechspace.h"
#include "GameInstance.h"
#include "Controller_Model.h"
#include "3DModel.h"


CSample_Skechspace::CSample_Skechspace(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CModelObject(_pDevice, _pContext)
{
}

CSample_Skechspace::CSample_Skechspace(const CSample_Skechspace& _Prototype)
    : CModelObject(_Prototype)
{
}


HRESULT CSample_Skechspace::Initialize_Prototype()
{
    __super::Initialize_Prototype();
    return S_OK;
}

HRESULT CSample_Skechspace::Initialize(void* _pArg)
{

    SAMPLE_SKSP_DESC* pDesc = static_cast<SAMPLE_SKSP_DESC*>(_pArg);
    pDesc->eStartCoord = COORDINATE_3D;
    pDesc->isCoordChangeEnable = false;
    pDesc->strShaderPrototypeTag_3D = TEXT("Prototype_Component_Shader_VtxMesh");
    pDesc->iShaderPass_3D = (_uint)PASS_VTXMESH::DEFAULT;
    pDesc->tTransform3DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);
    pDesc->tTransform3DDesc.vInitialScaling = _float3(1.0f, 1.0f, 1.0f);
    pDesc->tTransform3DDesc.fRotationPerSec = XMConvertToRadians(180.f);
    pDesc->tTransform3DDesc.fSpeedPerSec = 0.f;
    m_isPreView = pDesc->isPreview;

    __super::Initialize(_pArg);


    Set_Position(XMVectorSet(2.f, 0.f, -17.3f, 1.f));

    return S_OK;
}

void CSample_Skechspace::Priority_Update(_float _fTimeDelta)
{
    __super::Priority_Update(_fTimeDelta);
}

void CSample_Skechspace::Update(_float _fTimeDelta)
{
    __super::Update(_fTimeDelta);
}

void CSample_Skechspace::Late_Update(_float _fTimeDelta)
{
    Register_RenderGroup(RG_3D, PRIORITY_3D::PR3D_NONBLEND);

    __super::Update(_fTimeDelta);
}

HRESULT CSample_Skechspace::Render()
{
    // 10. 11

    if (FAILED(Bind_ShaderResources_WVP()))
        return E_FAIL;
    COORDINATE eCoord = m_pControllerTransform->Get_CurCoord();
    CShader* pShader = m_pShaderComs[eCoord];
    _uint iShaderPass = m_iShaderPasses[eCoord];
    C3DModel* pModel = static_cast<C3DModel*>(m_pControllerModel->Get_Model(Get_CurCoord()));

    for (_uint i = 0; i < (_uint)pModel->Get_Meshes().size(); ++i)
    {
        _uint iShaderPass = 0;
        auto pMesh = pModel->Get_Mesh(i);
        _uint iMaterialIndex = pMesh->Get_MaterialIndex();
      
        if(m_isPreView)
        { 
        
        }
        else
        {
            if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(pShader, "g_DiffuseTexture", TEXT("Target_2D"))))
                return E_FAIL;
        }

        pShader->Begin(iShaderPass);
        pMesh->Bind_BufferDesc();
        pMesh->Render();
    }

    return S_OK;
}

HRESULT CSample_Skechspace::Render_Shadow()
{
    return S_OK;
}

CSample_Skechspace* CSample_Skechspace::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CSample_Skechspace* pInstance = new CSample_Skechspace(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CSample_Skechspace");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CSample_Skechspace::Clone(void* _pArg)
{
    CSample_Skechspace* pInstance = new CSample_Skechspace(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CSample_Skechspace");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CSample_Skechspace::Free()
{

    __super::Free();
}
