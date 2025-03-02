#include "stdafx.h"
#include "3DMapSkspObject.h"
#include "GameInstance.h"
#include "Section_Manager.h"


C3DMapSkspObject::C3DMapSkspObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :C3DMapObject(_pDevice, _pContext)
{
}

C3DMapSkspObject::C3DMapSkspObject(const C3DMapSkspObject& _Prototype)
    :C3DMapObject(_Prototype)
{
}

HRESULT C3DMapSkspObject::Initialize(void* _pArg)
{

    if (nullptr == _pArg)
        return E_FAIL;

    MAPOBJ_3D_DESC* pDesc = static_cast<MAPOBJ_3D_DESC*>(_pArg);


    if (pDesc->iSksp == SKSP_NONE)
        return E_FAIL;
    m_eSkspType = (SKSP_RENDER_TYPE)pDesc->iSksp;
    m_strRenderSectionTag = StringToWstring(pDesc->strSkspTag);



    CSection* pSection = SECTION_MGR->Find_Section(m_strRenderSectionTag);

    //if (nullptr == pSection)
    //    return E_FAIL;
    if (nullptr != pSection)
    {
    CSection_2D* p2DSection = dynamic_cast<CSection_2D*>(pSection);

    //if (nullptr == p2DSection)
    //    return E_FAIL;

   if (nullptr != p2DSection)
        p2DSection->Register_WorldCapture(this);
    }



    HRESULT hr = __super::Initialize(_pArg);

    switch (m_eSkspType)
    {
    case Client::C3DMapSkspObject::SKSP_NONE:
        break;
    case Client::C3DMapSkspObject::SKSP_DEFAULT:
        break;
    case Client::C3DMapSkspObject::SKSP_CUP:
        break;
    case Client::C3DMapSkspObject::SKSP_TUB:
    {
        m_isCulling = false;
    }
    break;
    case Client::C3DMapSkspObject::SKSP_PLAG:
        m_isCulling = false;
        break;
    case Client::C3DMapSkspObject::SKSP_STORAGE:
    {
        m_isCulling = false;
        _wstring strModelName = pDesc->strModelPrototypeTag_3D;
        _int iNum = 0;
        _int iFactor = 1;
        for (_int i = (_int)strModelName.size() - 1; i >= 0; --i) {
            if (isdigit(strModelName[i])) {
                iNum = (strModelName[i] - '0') * iFactor + iNum;
                iFactor *= 10;
            }
            else
                break;
        }

        if (iNum == 2)
            m_isRight = true;
    }
    break;
    case Client::C3DMapSkspObject::SKSP_LAST:
        break;
    default:
        break;
    }

    return hr;

}

void C3DMapSkspObject::Late_Update(_float _fTimeDelta)
{
    if (KEY_DOWN(KEY::X))
    {
        _int isRenderState = m_eCurRenderState;
        isRenderState ^= 1;

        Change_RenderState((RT_RENDERSTATE)isRenderState, false);
    }
    

    CGameObject::Late_Update_Component(_fTimeDelta);

    /* Add Render Group */
    if (false == m_isFrustumCulling)
    {
        Register_RenderGroup(RENDERGROUP::RG_3D, PRIORITY_3D::PR3D_GEOMETRY);
        SECTION_MGR->SetActive_Section(m_strRenderSectionTag, true);
    }
    else
    {
        SECTION_MGR->SetActive_Section(m_strRenderSectionTag, false);
    }

}

HRESULT C3DMapSkspObject::Render()
{

    switch (m_eSkspType)
    {
    case Client::C3DMapSkspObject::SKSP_DEFAULT:
        if (FAILED(Render_Default()))
            return E_FAIL;
        break;
    case Client::C3DMapSkspObject::SKSP_PLAG:
        if (FAILED(Render_Flags()))
            return E_FAIL;
        break;
    case Client::C3DMapSkspObject::SKSP_CUP:
        if (FAILED(Render_Cup()))
            return E_FAIL;
        break;
    case Client::C3DMapSkspObject::SKSP_TUB:
        if (FAILED(Render_Tub()))
            return E_FAIL;
        break;
    case Client::C3DMapSkspObject::SKSP_STORAGE:
        if (FAILED(Render_Storage()))
            return E_FAIL;
        break;
    default:
        break;
    }

    return S_OK;
}

HRESULT C3DMapSkspObject::Render_Default()
{
    if (FAILED(Bind_ShaderResources_WVP()))
        return E_FAIL;

    COORDINATE eCoord = m_pControllerTransform->Get_CurCoord();
    CShader* pShader = m_pShaderComs[eCoord];
    _uint iShaderPass = m_iShaderPasses[eCoord];
    C3DModel* pModel = static_cast<C3DModel*>(m_pControllerModel->Get_Model(Get_CurCoord()));

    ID3D11ShaderResourceView* pResourceView = nullptr;
    pResourceView = SECTION_MGR->Get_SRV_FromRenderTarget(m_strRenderSectionTag);

    if (nullptr == pResourceView)
        return E_FAIL;


    for (_uint i = 0; i < (_uint)pModel->Get_Meshes().size(); ++i)
    {
        auto pMesh = pModel->Get_Mesh(i);
        _uint iMaterialIndex = pMesh->Get_MaterialIndex();

        if (FAILED(pModel->Bind_Material_PixelConstBuffer(iMaterialIndex, pShader)))
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


HRESULT C3DMapSkspObject::Render_Flags()
{
    if (FAILED(Bind_ShaderResources_WVP()))
        return E_FAIL;

    COORDINATE eCoord = m_pControllerTransform->Get_CurCoord();
    CShader* pShader = m_pShaderComs[eCoord];
    _uint iShaderPass = m_iShaderPasses[eCoord];
    C3DModel* pModel = static_cast<C3DModel*>(m_pControllerModel->Get_Model(Get_CurCoord()));

    ID3D11ShaderResourceView* pResourceView = nullptr;
    pResourceView = SECTION_MGR->Get_SRV_FromRenderTarget(m_strRenderSectionTag);

    if (nullptr == pResourceView)
        return E_FAIL;

    for (_uint i = 0; i < (_uint)pModel->Get_Meshes().size(); ++i)
    {
        auto pMesh = pModel->Get_Mesh(i);
        _uint iMaterialIndex = pMesh->Get_MaterialIndex();

        if (FAILED(pModel->Bind_Material_PixelConstBuffer(iMaterialIndex, pShader)))
            return E_FAIL;

        
        if (
            2 == i 
            || 
            5 == i
            )
        {
            _float2 fStartUV = {};
            _float2 fEndUV = {};
            if (2 == i)
            {
                fStartUV = { 0.5f,0.f };
                fEndUV = { 1.f,1.f };
            }
            else if (5 == i)
            {
                fStartUV = { 0.5f,0.f };
                fEndUV = { 1.f,1.f };

            }
            if (FAILED(pShader->Bind_RawValue("g_fStartUV", &fStartUV, sizeof(_float2))))
                return E_FAIL;
            if (FAILED(pShader->Bind_RawValue("g_fEndUV", &fEndUV, sizeof(_float2))))
                return E_FAIL;
            iShaderPass = m_iShaderPasses[eCoord] == (_uint)PASS_VTXMESH::AFTERPOSTPROCESSING ?
                (_uint)PASS_VTXMESH::RENDERTARGET_MAPP_AFTERPOSTPROCESSING
                : (_uint)PASS_VTXMESH::RENDERTARGET_MAPP;

            if (FAILED(pShader->Bind_SRV("g_AlbedoTexture", pResourceView)))
                return E_FAIL;
        }
        else 
        {
            iShaderPass = m_iShaderPasses[eCoord];

            if (FAILED(pModel->Bind_Material(pShader, "g_AlbedoTexture", i, aiTextureType_DIFFUSE, 0)))
            {
            }
        }
        


        

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

HRESULT C3DMapSkspObject::Render_Cup()
{
    if (FAILED(Bind_ShaderResources_WVP()))
        return E_FAIL;
    COORDINATE eCoord = m_pControllerTransform->Get_CurCoord();
    CShader* pShader = m_pShaderComs[eCoord];
    C3DModel* pModel = static_cast<C3DModel*>(m_pControllerModel->Get_Model(Get_CurCoord()));

    _bool iFlag = false;

    m_pShaderComs[COORDINATE_3D]->Bind_RawValue("g_iFlag", &iFlag, sizeof(_uint));

    for (_uint i = 0; i < (_uint)pModel->Get_Meshes().size(); ++i)
    {

        _uint iShaderPass = (_uint)PASS_VTXMESH::DEFAULT;
        auto pMesh = pModel->Get_Mesh(i);
        _uint iMaterialIndex = pMesh->Get_MaterialIndex();

        if (FAILED(pModel->Bind_Material_PixelConstBuffer(iMaterialIndex, pShader)))
            return E_FAIL;

        if (0 == i)
        {
            ID3D11ShaderResourceView* pResourceView = nullptr;
            pResourceView = SECTION_MGR->Get_SRV_FromRenderTarget(m_strRenderSectionTag);
            if (FAILED(pShader->Bind_SRV("g_AlbedoTexture", pResourceView)))
                return E_FAIL;
        }
        else
        {

            if (FAILED(pModel->Bind_Material(pShader, "g_AlbedoTexture", i, aiTextureType_DIFFUSE, 0)))
            {
                int a = 0;
            }

        }


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

HRESULT C3DMapSkspObject::Render_Tub()
{
    if (FAILED(Bind_ShaderResources_WVP()))
        return E_FAIL;

    COORDINATE eCoord = m_pControllerTransform->Get_CurCoord();
    CShader* pShader = m_pShaderComs[eCoord];
    _uint iShaderPass = m_iShaderPasses[eCoord];
    C3DModel* pModel = static_cast<C3DModel*>(m_pControllerModel->Get_Model(Get_CurCoord()));

    ID3D11ShaderResourceView* pResourceView = nullptr;
    pResourceView = SECTION_MGR->Get_SRV_FromRenderTarget(m_strRenderSectionTag);

    if (nullptr == pResourceView)
        return E_FAIL;

    for (_uint i = 0; i < (_uint)pModel->Get_Meshes().size(); ++i)
    {
        auto pMesh = pModel->Get_Mesh(i);
        _uint iMaterialIndex = pMesh->Get_MaterialIndex();

        if (FAILED(pModel->Bind_Material_PixelConstBuffer(iMaterialIndex, pShader)))
            return E_FAIL;


        if (1 == i || 2 == i)
        {
            _float2 fStartUV = {};
            _float2 fEndUV = {};
            if (1 == i)
            {
                fStartUV = { 0.f,-0.64f };
                fEndUV = { 0.51f,3.03f };
            }
            else if (2 == i)
            {
                fStartUV = { 0.5f,-0.64f };
                fEndUV = { 1.01f,3.03f };

            }
            if (FAILED(pShader->Bind_RawValue("g_fStartUV", &fStartUV, sizeof(_float2))))
                return E_FAIL;
            if (FAILED(pShader->Bind_RawValue("g_fEndUV", &fEndUV, sizeof(_float2))))
                return E_FAIL;
            iShaderPass = m_iShaderPasses[eCoord] == (_uint)PASS_VTXMESH::AFTERPOSTPROCESSING ?
                (_uint)PASS_VTXMESH::RENDERTARGET_MAPP_AFTERPOSTPROCESSING
                : (_uint)PASS_VTXMESH::RENDERTARGET_MAPP;

            if (FAILED(pShader->Bind_SRV("g_AlbedoTexture", pResourceView)))
                return E_FAIL;
        }
        else
        {
            iShaderPass = m_iShaderPasses[eCoord];

            if (FAILED(pModel->Bind_Material(pShader, "g_AlbedoTexture", i, aiTextureType_DIFFUSE, 0)))
            {
            }
        }





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

HRESULT C3DMapSkspObject::Render_Storage()
{
    if (FAILED(Bind_ShaderResources_WVP()))
        return E_FAIL;
    COORDINATE eCoord = m_pControllerTransform->Get_CurCoord();
    CShader* pShader = m_pShaderComs[eCoord];
    _uint iShaderPass = m_iShaderPasses[eCoord];
    C3DModel* pModel = static_cast<C3DModel*>(m_pControllerModel->Get_Model(Get_CurCoord()));

    _bool iFlag = false;
    ID3D11ShaderResourceView* pResourceView = nullptr;

    pResourceView = SECTION_MGR->Get_SRV_FromRenderTarget(m_strRenderSectionTag);
    if (FAILED(pShader->Bind_SRV("g_AlbedoTexture", pResourceView)))
        return E_FAIL;

    m_pShaderComs[COORDINATE_3D]->Bind_RawValue("g_iFlag", &iFlag, sizeof(_uint));

    for (_uint i = 0; i < (_uint)pModel->Get_Meshes().size(); ++i)
    {
        _uint iShaderPass = (_uint)PASS_VTXMESH::DEFAULT;
        auto pMesh = pModel->Get_Mesh(i);
        _uint iMaterialIndex = pMesh->Get_MaterialIndex();
        if (FAILED(pModel->Bind_Material_PixelConstBuffer(iMaterialIndex, pShader)))
            return E_FAIL;
        _float2 fStartUV = {};
        _float2 fEndUV = {};
        if (!m_isRight)
        {
            fStartUV = { 0.f,0.f };
            fEndUV = { 0.5f,1.f };
        }
        else
        {
            fStartUV = { 0.5f,0.f };
            fEndUV = { 1.f,1.f };
        }
        iShaderPass = (_uint)PASS_VTXMESH::RENDERTARGET_MAPP;

        if (FAILED(pShader->Bind_RawValue("g_fStartUV", &fStartUV, sizeof(_float2))))
            return E_FAIL;
        if (FAILED(pShader->Bind_RawValue("g_fEndUV", &fEndUV, sizeof(_float2))))
            return E_FAIL;
        iShaderPass = (_uint)PASS_VTXMESH::RENDERTARGET_MAPP;

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

void C3DMapSkspObject::Change_RenderState(RT_RENDERSTATE _eRenderState, _bool _isMapped)
{
    switch (_eRenderState)
    {
    case Client::RT_RENDERSTATE::RENDERSTATE_LIGHT:
        m_iPriorityID_3D = PR3D_GEOMETRY;
        m_iShaderPasses[COORDINATE_3D] = (_uint)PASS_VTXMESH::DEFAULT;
        break;
    case Client::RT_RENDERSTATE::RENDERSTATE_NONLIGHT:
        m_iPriorityID_3D = PR3D_AFTERPOSTPROCESSING;
        m_iShaderPasses[COORDINATE_3D] = (_uint)PASS_VTXMESH::AFTERPOSTPROCESSING;
        break;
    default:
        break;
    }

    m_eCurRenderState = _eRenderState;
}

void C3DMapSkspObject::Check_FrustumCulling()
{
    if (COORDINATE_3D == Get_CurCoord() && true == m_isCulling)
        m_isFrustumCulling = !m_pGameInstance->isIn_Frustum_InWorldSpace(Get_FinalPosition(), 10.f);
    else
        m_isFrustumCulling = false;
}

HRESULT C3DMapSkspObject::Render_WorldPosMap(const _wstring& _strCopyRTTag, const _wstring& _strSectionTag)
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
    _uint iFlag = 0;

    C3DModel* p3DModel = static_cast<C3DModel*>(m_pControllerModel->Get_Model(COORDINATE_3D));
    if (nullptr == p3DModel)
        return E_FAIL;



    switch (m_eSkspType)
    {

    case Client::C3DMapSkspObject::SKSP_PLAG:
    {

        CMesh* pLeftMesh = p3DModel->Get_Mesh(2);
        CMesh* pRightMesh = p3DModel->Get_Mesh(5);
        iFlag = 3;


        _float2 vStartCoord = { 0.5f, 0.0f };
        _float2 vEndCoord = { 1.f,1.f };
        m_pShaderComs[COORDINATE_3D]->Bind_RawValue("g_fStartUV", &vStartCoord, sizeof(_float2));
        m_pShaderComs[COORDINATE_3D]->Bind_RawValue("g_fEndUV", &vEndCoord, sizeof(_float2));

        m_pShaderComs[COORDINATE_3D]->Bind_RawValue("g_iFlag", &iFlag, sizeof(_uint));
        m_pShaderComs[COORDINATE_3D]->Begin((_uint)PASS_VTXMESH::WORLDPOSMAP_BOOK);
        pLeftMesh->Bind_BufferDesc();
        pLeftMesh->Render();

        //vStartCoord = { 0.5f, 0.0f };
        //vEndCoord = { 1.f, 1.f };
        m_pShaderComs[COORDINATE_3D]->Bind_RawValue("g_fStartUV", &vStartCoord, sizeof(_float2));
        m_pShaderComs[COORDINATE_3D]->Bind_RawValue("g_fEndUV", &vEndCoord, sizeof(_float2));
        m_pShaderComs[COORDINATE_3D]->Begin((_uint)PASS_VTXMESH::WORLDPOSMAP_BOOK);
        pRightMesh->Bind_BufferDesc();
        pRightMesh->Render();
    }
    break;
    case Client::C3DMapSkspObject::SKSP_TUB:
    {
        CMesh* pLeftMesh = p3DModel->Get_Mesh(1);
        CMesh* pRightMesh = p3DModel->Get_Mesh(2);
        iFlag = 2;

        _float2 vStartCoord = { 0.f,-0.64f };
        _float2 vEndCoord = { 0.51f,3.03f };
        m_pShaderComs[COORDINATE_3D]->Bind_RawValue("g_fStartUV", &vStartCoord, sizeof(_float2));
        m_pShaderComs[COORDINATE_3D]->Bind_RawValue("g_fEndUV", &vEndCoord, sizeof(_float2));

        m_pShaderComs[COORDINATE_3D]->Bind_RawValue("g_iFlag", &iFlag, sizeof(_uint));
        m_pShaderComs[COORDINATE_3D]->Begin((_uint)PASS_VTXMESH::WORLDPOSMAP_BOOK);
        pLeftMesh->Bind_BufferDesc();
        pLeftMesh->Render();

        vStartCoord = { 0.5f,-0.64f };
        vEndCoord = { 1.01f,3.03f };
        m_pShaderComs[COORDINATE_3D]->Bind_RawValue("g_fStartUV", &vStartCoord, sizeof(_float2));
        m_pShaderComs[COORDINATE_3D]->Bind_RawValue("g_fEndUV", &vEndCoord, sizeof(_float2));
        m_pShaderComs[COORDINATE_3D]->Begin((_uint)PASS_VTXMESH::WORLDPOSMAP_BOOK);
        pRightMesh->Bind_BufferDesc();
        pRightMesh->Render();
    }
    break;
    case Client::C3DMapSkspObject::SKSP_STORAGE:
    {

        CMesh* pMesh = p3DModel->Get_Mesh(0);
        iFlag = 2;


        _float2 vStartCoord = { 0.5f, 0.0f };
        _float2 vEndCoord = { 1.f,1.f };
        if (!m_isRight)
        {
            vStartCoord = { 0.f,0.f };
            vEndCoord = { 0.5f,1.f };
        }
        else
        {
            vStartCoord = { 0.5f,0.f };
            vEndCoord = { 1.f,1.f };
        }
        m_pShaderComs[COORDINATE_3D]->Bind_RawValue("g_fStartUV", &vStartCoord, sizeof(_float2));
        m_pShaderComs[COORDINATE_3D]->Bind_RawValue("g_fEndUV", &vEndCoord, sizeof(_float2));

        m_pShaderComs[COORDINATE_3D]->Bind_RawValue("g_iFlag", &iFlag, sizeof(_uint));
        m_pShaderComs[COORDINATE_3D]->Begin((_uint)PASS_VTXMESH::WORLDPOSMAP_BOOK);
        pMesh->Bind_BufferDesc();
        pMesh->Render();

    }
    break;
    case Client::C3DMapSkspObject::SKSP_CUP:
    {
        CMesh* pMesh = p3DModel->Get_Mesh(0);
        m_pShaderComs[COORDINATE_3D]->Bind_RawValue("g_iFlag", &iFlag, sizeof(_uint));
        m_pShaderComs[COORDINATE_3D]->Begin((_uint)PASS_VTXMESH::WORLDPOSMAP_BOOK);
        pMesh->Bind_BufferDesc();
        pMesh->Render();
    }
    break;
    case Client::C3DMapSkspObject::SKSP_DEFAULT:
    case Client::C3DMapSkspObject::SKSP_LAST:
    default:
    {
        iFlag = 0;
        m_pShaderComs[COORDINATE_3D]->Bind_RawValue("g_iFlag", &iFlag, sizeof(_uint));
        p3DModel->Render(m_pShaderComs[COORDINATE_3D], (_uint)PASS_VTXMESH::WORLDPOSMAP_BOOK);
    }
    break;
    }


    ID3D11ShaderResourceView* pSRV = m_pGameInstance->Get_RT_SRV(_strCopyRTTag);
    ID3D11Resource* pResource = nullptr;
    pSRV->GetResource(&pResource);
    m_pContext->CopyResource(pTexture, pResource);

    Safe_AddRef(pTexture);
    p2DSection->Set_WorldTexture(pTexture);
    //어차피 똑같은건데... 코드 분리하기 좀 늦은거같아서 그냥 똑같은거 한번 넣어서 포탈 생성 타게 만듬.
                                           // 그를 위한 Safe_Addref(내부에서 한번 릴리즈 해주니까.) 
    Safe_Release(pResource);

    return S_OK;
}


C3DMapSkspObject* C3DMapSkspObject::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    C3DMapSkspObject* pInstance = new C3DMapSkspObject(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : C3DMapSkspObject");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* C3DMapSkspObject::Clone(void* _pArg)
{
    C3DMapSkspObject* pInstance = new C3DMapSkspObject(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : C3DMapSkspObject");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void C3DMapSkspObject::Free()
{
    __super::Free();
}
