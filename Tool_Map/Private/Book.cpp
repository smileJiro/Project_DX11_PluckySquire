#include "stdafx.h"
#include "Book.h"
#include "GameInstance.h"
#include "Controller_Model.h"
#include "3DModel.h"


CBook::CBook(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CModelObject(_pDevice, _pContext)
{
}

CBook::CBook(const CBook& _Prototype)
    : CModelObject(_Prototype)
{
}


HRESULT CBook::Initialize_Prototype()
{
    __super::Initialize_Prototype();
    return S_OK;
}

HRESULT CBook::Initialize(void* _pArg)
{
    
    MODELOBJECT_DESC* pDesc = static_cast<MODELOBJECT_DESC*>(_pArg);
    pDesc->eStartCoord = COORDINATE_3D;
    pDesc->isCoordChangeEnable = false;
    pDesc->strShaderPrototypeTag_3D = TEXT("Prototype_Component_Shader_VtxAnimMesh");
    pDesc->strModelPrototypeTag_3D = TEXT("book");
    pDesc->iShaderPass_3D = (_uint)PASS_VTXANIMMESH::DEFAULT;
    pDesc->tTransform3DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);
    pDesc->tTransform3DDesc.vInitialScaling = _float3(1.0f, 1.0f, 1.0f);
    pDesc->tTransform3DDesc.fRotationPerSec = XMConvertToRadians(180.f);
    pDesc->tTransform3DDesc.fSpeedPerSec = 0.f;


    __super::Initialize(_pArg);
    Set_AnimationLoop(COORDINATE_3D, 4, true);
    Set_Animation(0);


    Set_Position(XMVectorSet(2.f,0.f,-17.3f,1.f));

    return S_OK;
}

void CBook::Priority_Update(_float _fTimeDelta)
{
    __super::Priority_Update(_fTimeDelta);
}

void CBook::Update(_float _fTimeDelta)
{
    __super::Update(_fTimeDelta);
}

void CBook::Late_Update(_float _fTimeDelta)
{
    Register_RenderGroup(RG_3D, PRIORITY_3D::PR3D_GEOMETRY);

    __super::Update(_fTimeDelta);
}

HRESULT CBook::Render()
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
        if (10 == i || 11 == i)
        {
            _float2 fStartUV = {};
            _float2 fEndUV = {};
            if (11 == i)
            {
                fStartUV = { 0.f,0.f };
                fEndUV = { 0.5f,1.f };
                if (FAILED(pShader->Bind_RawValue("g_fStartUV", &fStartUV, sizeof(_float2))))
                    return E_FAIL;
                if (FAILED(pShader->Bind_RawValue("g_fEndUV", &fEndUV, sizeof(_float2))))
                    return E_FAIL;
            }
            else
            {
                fStartUV = { 0.5f,0.f };
                fEndUV = { 1.f,1.f };
                if (FAILED(pShader->Bind_RawValue("g_fStartUV", &fStartUV, sizeof(_float2))))
                    return E_FAIL;
                if (FAILED(pShader->Bind_RawValue("g_fEndUV", &fEndUV, sizeof(_float2))))
                    return E_FAIL;
            }

            if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(pShader, "g_DiffuseTexture", TEXT("Target_Book_2D"))))
                return E_FAIL;
            iShaderPass = 4;
        }
        else
        {
            if (FAILED(pModel->Bind_Material(pShader, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0)))
            {
                //continue;
            }
        }

        if (FAILED(pModel->Bind_Matrices(pShader, "g_BoneMatrices", i)))
            return E_FAIL;

        pShader->Begin(iShaderPass);

        pMesh->Bind_BufferDesc();
        pMesh->Render();
    }

    return S_OK;
}

CBook* CBook::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CBook* pInstance = new CBook(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CBook");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBook::Clone(void* _pArg)
{
    CBook* pInstance = new CBook(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CBook");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBook::Free()
{

    __super::Free();
}
