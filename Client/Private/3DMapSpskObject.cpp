#include "stdafx.h"
#include "3DMapSpskObject.h"
#include "GameInstance.h"
#include "Section_Manager.h"


C3DMapSpskObject::C3DMapSpskObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :C3DMapObject(_pDevice, _pContext)
{
}

C3DMapSpskObject::C3DMapSpskObject(const C3DMapSpskObject& _Prototype)
    :C3DMapObject(_Prototype)
{
}

HRESULT C3DMapSpskObject::Initialize(void* _pArg)
{

    if (nullptr == _pArg)
        return E_FAIL;

    MAPOBJ_3D_DESC* pDesc = static_cast<MAPOBJ_3D_DESC*>(_pArg);


    if (!pDesc->isSpsk)
        return E_FAIL;

    m_strRenderSectionTag = StringToWstring(pDesc->strSpskTag);

    CSection* pSection = SECTION_MGR->Find_Section(m_strRenderSectionTag);

    if (nullptr == pSection)
        return E_FAIL;

    CSection_2D* p2DSection = dynamic_cast<CSection_2D*>(pSection);

    if (nullptr == p2DSection)
        E_FAIL;


    p2DSection->Register_Capcher_WorldTexture(this);


    return __super::Initialize(_pArg);
}

void C3DMapSpskObject::Late_Update(_float _fTimeDelta)
{
    CGameObject::Late_Update_Component(_fTimeDelta);

    /* Add Render Group */
    if (!m_isCulling || true == m_pGameInstance->isIn_Frustum_InWorldSpace(Get_FinalPosition(), 5.0f))
    {
        Register_RenderGroup(RENDERGROUP::RG_3D, PRIORITY_3D::PR3D_GEOMETRY);
        SECTION_MGR->SetActive_Section(m_strRenderSectionTag, true);
    }
    else
    {
        //SECTION_MGR->SetActive_Section(m_strRenderSectionTag, false);
    }
}

HRESULT C3DMapSpskObject::Render()
{
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

        if (FAILED(pModel->Bind_Material_PixelConstBuffer(iMaterialIndex, pShader)))
            return E_FAIL;
        ID3D11ShaderResourceView* pResourceView = nullptr;
        pResourceView = SECTION_MGR->Get_SRV_FromRenderTarget(m_strRenderSectionTag);
        
        if (nullptr == pResourceView)
            return E_FAIL;

        if (FAILED(pShader->Bind_SRV("g_AlbedoTexture", pResourceView)))
            return E_FAIL;

        if (FAILED(pModel->Bind_Material(pShader, "g_NormalTexture", i, aiTextureType_NORMALS, 0)))
        {
        }
         
        
        if (FAILED(pModel->Bind_Material(pShader, "g_ORMHTexture", i, aiTextureType_BASE_COLOR, 0)))
        {
        } 
        
        if (FAILED(pModel->Bind_Material(pShader, "g_MetallicTexture", i, aiTextureType_METALNESS, 0)))
        {
        }
       
        if (FAILED(pModel->Bind_Material(pShader, "g_RoughnessTexture", i, aiTextureType_DIFFUSE_ROUGHNESS, 0)))
        {
        }
        
        if (FAILED(pModel->Bind_Material(pShader, "g_AOTexture", i, aiTextureType_AMBIENT_OCCLUSION, 0)))
        {
        }

        pShader->Begin(iShaderPass);

        pMesh->Bind_BufferDesc();

        pMesh->Render();
    }

    return S_OK;
}

HRESULT C3DMapSpskObject::Render_Shadow()
{
    return __super::Render_Shadow();
}

HRESULT C3DMapSpskObject::Render_WorldPosMap()
{

    CSection* pSection = SECTION_MGR->Find_Section(m_strRenderSectionTag);
    if (nullptr == pSection)
        return E_FAIL;
    CSection_2D* p2DSection = static_cast<CSection_2D*>(pSection);
    ID3D11Texture2D* pTexture = p2DSection->Get_WorldTexture();
    if (nullptr == pTexture)
        return E_FAIL;

    if (FAILED(m_pShaderComs[COORDINATE_3D]->Bind_Matrix("g_WorldMatrix", m_pControllerTransform->Get_WorldMatrix_Ptr())))
        return E_FAIL;
    C3DModel* p3DModel = static_cast<C3DModel*>(m_pControllerModel->Get_Model(COORDINATE_3D));
    if (nullptr == p3DModel)
        return E_FAIL;

    m_pControllerModel->Render(m_pShaderComs[COORDINATE_3D], (_uint)PASS_VTXMESH::WORLDPOSMAP_BOOK);

#pragma endregion // 1


    ID3D11ShaderResourceView* pSRV = m_pGameInstance->Get_RT_SRV(p2DSection->Get_WorldRenderTarget_Tag());
    ID3D11Resource* pResource = nullptr;
    pSRV->GetResource(&pResource);
    m_pContext->CopyResource(pTexture, pResource);
    Safe_Release(pResource);

    return S_OK;
}

C3DMapSpskObject* C3DMapSpskObject::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    C3DMapSpskObject* pInstance = new C3DMapSpskObject(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : C3DMapSpskObject");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* C3DMapSpskObject::Clone(void* _pArg)
{
    C3DMapSpskObject* pInstance = new C3DMapSpskObject(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : C3DMapSpskObject");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void C3DMapSpskObject::Free()
{
    __super::Free();
}
