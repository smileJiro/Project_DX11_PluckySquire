#include "ModelObject.h"
#include "GameInstance.h"
#include "3DModel.h"
#include "2DModel.h"

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

HRESULT CModelObject::Render()
{

    if (FAILED(Bind_ShaderResources_WVP()))
        return E_FAIL;

    switch (m_pControllerTransform->Get_CurCoord())
    {
    case Engine::COORDINATE_2D:
        Render_2D();
        break;
    case Engine::COORDINATE_3D:
        Render_3D();
        break;
    default:
        break;
    }

    return S_OK;
}

_bool CModelObject::Is_PickingCursor_Model(_float2 _fCursorPos, _float& _fDst)
{
    // 예외처리
    if (nullptr == m_p3DModelCom)
        return false;
    if (nullptr == m_pRayCom)
        return false;

    // 레이 매트릭스 설정.
    m_pRayCom->Update_RayInfoFromCursor(_float2(_fCursorPos.x, _fCursorPos.y),
        m_pGameInstance->Get_TransformInverseMatrix(CPipeLine::D3DTS_VIEW),
        m_pGameInstance->Get_TransformInverseMatrix(CPipeLine::D3DTS_PROJ));

    // 모델의 메쉬를 순회하며, 충돌했다면 바로 리턴하고 dst 값을 넘기자.
    size_t iNumMeshes = m_p3DModelCom->Get_NumMeshes();
    const vector<CMesh*>& vecMeshes = m_p3DModelCom->Get_Meshes();

    for (size_t i = 0; i < iNumMeshes; ++i)
    {
        const vector<_float3>& vecVerticesPos = vecMeshes[i]->Get_VerticesPos();
        const vector<_uint>& vecIndexBuffer = vecMeshes[i]->Get_IndexBuffer();
        _uint iNumTriangles = vecMeshes[i]->Get_NumTriangles();

        for (size_t j = 0; j < iNumTriangles; ++j)
        {
            _bool bResult = false;
            _uint iIdx = j * 3;
            _matrix WorldMatrix = m_pControllerTransform->Get_WorldMatrix();

            bResult = m_pRayCom->Compute_Intersect_Triangle(XMVector3TransformCoord(XMLoadFloat3(&vecVerticesPos[vecIndexBuffer[iIdx]]), WorldMatrix),
                XMVector3TransformCoord(XMLoadFloat3(&vecVerticesPos[vecIndexBuffer[iIdx + 1]]), WorldMatrix),
                XMVector3TransformCoord(XMLoadFloat3(&vecVerticesPos[vecIndexBuffer[iIdx + 2]]), WorldMatrix), _fDst);
            if (bResult)
            {
                return true;
            }
        }
    }


    return false;
}

void CModelObject::Update(_float fTimeDelta)
{
    if (m_p3DModelCom->Is_AnimModel())
    {
        if (m_p3DModelCom->Play_Animation(fTimeDelta))
        {
            //for (auto& callback : m_listAnimEndCallBack)
            //    callback(m_pModelCom->Get_AnimIndex());
        }
    }

	__super::Update(fTimeDelta);
}

void CModelObject::Set_AnimationLoop(_uint iIdx, _bool bIsLoop)
{
    m_p3DModelCom->Set_AnimationLoop(iIdx, bIsLoop);
}

void CModelObject::Set_Animation(_uint iIdx)
{
    m_p3DModelCom->Set_Animation(iIdx);
}

void CModelObject::Switch_Animation(_uint iIdx)
{
    m_p3DModelCom->Switch_Animation(iIdx);
}

HRESULT CModelObject::Ready_Components(MODELOBJECT_DESC* _pDesc)
{
    _int iStaticLevelID = m_pGameInstance->Get_StaticLevelID();
    switch (_pDesc->eStartCoord)
    {
    case Engine::COORDINATE_2D:
    {
        /* Com_VIBuffer */
        if (FAILED(Add_Component(iStaticLevelID, TEXT("Prototype_Component_VIBuffer_Rect"),
            TEXT("Com_Model_2D"), reinterpret_cast<CComponent**>(&m_p2DModelCom))))
            return E_FAIL;

        /* Com_Shader_2D */
        if (FAILED(Add_Component(iStaticLevelID, _pDesc->strShaderPrototypeTag_2D,
            TEXT("Com_Shader_2D"), reinterpret_cast<CComponent**>(&m_pShaderComs[COORDINATE_2D]))))
            return E_FAIL;

        if (true == _pDesc->isCoordChangeEnable)
        {
            /* Com_Model */
            if (FAILED(Add_Component(m_iCurLevelID, _pDesc->strModelPrototypeTag,
                TEXT("Com_Model_3D"), reinterpret_cast<CComponent**>(&m_p3DModelCom))))
                return E_FAIL;

            /* Com_Shader_3D */
            if (FAILED(Add_Component(iStaticLevelID, _pDesc->strShaderPrototypeTag_3D,
                TEXT("Com_Shader_3D"), reinterpret_cast<CComponent**>(&m_pShaderComs[COORDINATE_3D]))))
                return E_FAIL;
        }
    }
    break;
    case Engine::COORDINATE_3D:
    {
        /* Com_Model */
        if (FAILED(Add_Component(m_iCurLevelID, _pDesc->strModelPrototypeTag,
            TEXT("Com_Model_3D"), reinterpret_cast<CComponent**>(&m_p3DModelCom))))
            return E_FAIL;

        /* Com_Shader_3D */
        if (FAILED(Add_Component(iStaticLevelID, _pDesc->strShaderPrototypeTag_3D,
            TEXT("Com_Shader_3D"), reinterpret_cast<CComponent**>(&m_pShaderComs[COORDINATE_3D]))))
            return E_FAIL;

        if (true == _pDesc->isCoordChangeEnable)
        {
            /* Com_VIBuffer */
            if (FAILED(Add_Component(iStaticLevelID, TEXT("Prototype_Component_VIBuffer_Rect"),
                TEXT("Com_Model_2D"), reinterpret_cast<CComponent**>(&m_p2DModelCom))))
                return E_FAIL;

            /* Com_Shader_2D */
            if (FAILED(Add_Component(iStaticLevelID, _pDesc->strShaderPrototypeTag_2D,
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


    return S_OK;
}

HRESULT CModelObject::Render_3D()
{
    _uint iNumMeshes = m_p3DModelCom->Get_NumMeshes();

    /* Mesh 단위 렌더. */
    for (_uint i = 0; i < iNumMeshes; ++i)
    {
        if (FAILED(m_p3DModelCom->Bind_Material(m_pShaderComs[COORDINATE_3D], "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0)))
        {
            continue;
        }

        /* Bind Bone Matrices */
        if (m_p3DModelCom->Is_AnimModel())
        {
            if (FAILED(m_p3DModelCom->Bind_Matrices(m_pShaderComs[COORDINATE_3D], "g_BoneMatrices", i)))
                return E_FAIL;
        }


        /* Shader Pass */
        m_pShaderComs[COORDINATE_3D]->Begin(m_iShaderPasses[COORDINATE_3D]);

        /* Bind Mesh Vertex Buffer */
        m_p3DModelCom->Render(i);
    }

    return S_OK;
}

void CModelObject::Free()
{
    Safe_Release(m_p2DModelCom);
    Safe_Release(m_p3DModelCom);

    for (_int i = 0; i < COORDINATE_LAST; ++i)
    {
        Safe_Release(m_pShaderComs[i]);
    }

    __super::Free();
}

