#include "ModelObject.h"
#include "GameInstance.h"

CModelObject::CModelObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CPartObject(_pDevice, _pContext)
{
}

CModelObject::CModelObject(const CModelObject& _Prototype)
    : CPartObject(_Prototype)
{
}

HRESULT CModelObject::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CModelObject::Initialize(void* _pArg)
{
    MODELOBJECT_DESC* pDesc = static_cast<MODELOBJECT_DESC*>(_pArg);

    // Save 
    m_iShaderPasses[COORDINATE_2D] = pDesc->iShaderPass_2D;
    m_iShaderPasses[COORDINATE_3D] = pDesc->iShaderPass_3D;

    // Add


    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components(pDesc)))
        return E_FAIL;


    D3D11_VIEWPORT ViewportDesc{};
    _uint iNumViewports = 1;
    m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

    // View Matrix는 IdentityMatrix
    XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
    // Projection Matrix는 Viewport Desc 를 기반으로 생성.
    XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)ViewportDesc.Width, (_float)ViewportDesc.Height, 0.0f, 1.0f));

    return S_OK;
}

HRESULT CModelObject::Ready_Components(MODELOBJECT_DESC* _pDesc)
{
    switch (_pDesc->eStartCoord)
    {
    case Engine::COORDINATE_2D:
    {
        /* Com_VIBuffer */
        if (FAILED(Add_Component(1, TEXT("Prototype_Component_VIBuffer_Rect"),
            TEXT("Com_Model_2D"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
            return E_FAIL;

        /* Com_Shader_2D */
        if (FAILED(Add_Component(1, _pDesc->strShaderPrototypeTag_3D,
            TEXT("Com_Shader_2D"), reinterpret_cast<CComponent**>(&m_pShaderComs[COORDINATE_2D]))))
            return E_FAIL;

        if (true == _pDesc->isCoordChangeEnable)
        {
            /* Com_Model */
            if (FAILED(Add_Component(m_iCurLevelID, _pDesc->strModelPrototypeTag,
                TEXT("Com_Model_3D"), reinterpret_cast<CComponent**>(&m_pModelCom))))
                return E_FAIL;

            /* Com_Shader_3D */
            if (FAILED(Add_Component(1, _pDesc->strShaderPrototypeTag_3D,
                TEXT("Com_Shader_3D"), reinterpret_cast<CComponent**>(&m_pShaderComs[COORDINATE_3D]))))
                return E_FAIL;
        }
    }
    break;
    case Engine::COORDINATE_3D:
    {
        /* Com_Model */
        if (FAILED(Add_Component(m_iCurLevelID, _pDesc->strModelPrototypeTag,
            TEXT("Com_Model_3D"), reinterpret_cast<CComponent**>(&m_pModelCom))))
            return E_FAIL;

        /* Com_Shader_3D */
        if (FAILED(Add_Component(1, _pDesc->strShaderPrototypeTag_3D,
            TEXT("Com_Shader_3D"), reinterpret_cast<CComponent**>(&m_pShaderComs[COORDINATE_3D]))))
            return E_FAIL;

        if (true == _pDesc->isCoordChangeEnable)
        {
            /* Com_VIBuffer */
            if (FAILED(Add_Component(1, TEXT("Prototype_Component_VIBuffer_Rect"),
                TEXT("Com_Model_2D"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
                return E_FAIL;

            /* Com_Shader_2D */
            if (FAILED(Add_Component(1, _pDesc->strShaderPrototypeTag_3D,
                TEXT("Com_Shader_2D"), reinterpret_cast<CComponent**>(&m_pShaderComs[COORDINATE_2D]))))
                return E_FAIL;
        }
    }
    break;
    default:
        break;
    }

    return S_OK;
}

HRESULT CModelObject::Bind_ShaderResources_WVP()
{
    switch (m_pControllerTransform->Get_CurCoord())
    {
    case Engine::COORDINATE_2D:
        m_WorldMatrices[COORDINATE_2D]._44 = 1.0f;
        if (FAILED(m_pShaderComs[COORDINATE_2D]->Bind_Matrix("g_WorldMatrix", &m_WorldMatrices[COORDINATE_2D])))
            return E_FAIL;

        if (FAILED(m_pShaderComs[COORDINATE_2D]->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
            return E_FAIL;

        /* 추후 우리 그리고자하는 렌더 타겟 사이즈로 projectionMatrix 를 변경해줘야함. */
        // m_pGameInstance->Get_RT_Size(TEXT("")); // 렌더타겟 사이즈 받아오는 함수는 만들어뒀음. 2D 3D 공간
        if (FAILED(m_pShaderComs[COORDINATE_2D]->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
            return E_FAIL;

        break;
    case Engine::COORDINATE_3D:
        if (FAILED(m_pShaderComs[COORDINATE_3D]->Bind_Matrix("g_WorldMatrix", &m_WorldMatrices[COORDINATE_3D])))
            return E_FAIL;

        if (FAILED(m_pShaderComs[COORDINATE_3D]->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
            return E_FAIL;

        if (FAILED(m_pShaderComs[COORDINATE_3D]->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
            return E_FAIL;

        break;
    default:
        break;
    }

    return S_OK;
}

HRESULT CModelObject::Render_2D()
{

    // 어떠한 Pass로 그릴 것인지
    m_pShaderComs[COORDINATE_2D]->Begin(m_iShaderPasses[COORDINATE_2D]);
    // Buffer정보 전달
    m_pVIBufferCom->Bind_BufferDesc();

    // 그리기 연산 수행
    m_pVIBufferCom->Render();

    return S_OK;
}

HRESULT CModelObject::Render_3D()
{
    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    /* Mesh 단위 렌더. */
    for (size_t i = 0; i < iNumMeshes; ++i)
    {
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderComs[COORDINATE_3D], "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0)))
        {
            //continue;
        }

        /* Bind Bone Matrices */
        if (CModel::TYPE::ANIM == m_pModelCom->Get_ModelType())
        {
            if (FAILED(m_pModelCom->Bind_Matrices(m_pShaderComs[COORDINATE_3D], "g_BoneMatrices", i)))
                return E_FAIL;
        }


        /* Shader Pass */
        m_pShaderComs[COORDINATE_3D]->Begin(m_iShaderPasses[COORDINATE_3D]);

        /* Bind Mesh Vertex Buffer */
        m_pModelCom->Render(i);
    }

    return S_OK;
}

void CModelObject::Free()
{
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pModelCom);

    for (_int i = 0; i < COORDINATE_LAST; ++i)
    {
        Safe_Release(m_pShaderComs[i]);
    }

    __super::Free();
}

