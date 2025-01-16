#include "stdafx.h"
#include "MapObject.h"
#include "GameInstance.h"
#include <gizmo/ImGuizmo.h>

CMapObject::CMapObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CModelObject(_pDevice, _pContext)
{
}

CMapObject::CMapObject(const CMapObject& _Prototype)
    :CModelObject(_Prototype)
{
}

HRESULT CMapObject::Initialize_Prototype()
{

    return S_OK;
}

HRESULT CMapObject::Initialize(void* _pArg)
{
    MAPOBJ_DESC* pDesc = static_cast<MAPOBJ_DESC*>(_pArg);
    m_strModelName = pDesc->szModelName;
    m_matWorld = pDesc->matWorld;
    pDesc->eStartCoord = COORDINATE_3D;
    pDesc->iCurLevelID = LEVEL_TOOL_MAP;
    pDesc->isCoordChangeEnable = false;
    pDesc->i3DModelPrototypeLevelID = LEVEL_TOOL_MAP;
    pDesc->strShaderPrototypeTag_3D = TEXT("Prototype_Component_Shader_VtxMesh");
    pDesc->strModelPrototypeTag_3D = m_strModelName;

    pDesc->iShaderPass_3D = (_uint)PASS_VTXMESH::DEFAULT;

    pDesc->tTransform3DDesc.vPosition = _float3(0.0f, 0.0f, 0.0f);
    pDesc->tTransform3DDesc.vScaling = _float3(1.0f, 1.0f, 1.0f);
    pDesc->tTransform3DDesc.fRotationPerSec = XMConvertToRadians(180.f);
    pDesc->tTransform3DDesc.fSpeedPerSec = 0.f;
    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;


    D3D11_VIEWPORT ViewportDesc = {};
    _uint iNumViewport = 1;

    m_pContext->RSGetViewports(&iNumViewport, &ViewportDesc);

    CRay::RAY_DESC RayDesc = {};
    RayDesc.fViewportWidth =   (_float)ViewportDesc.Width;
    RayDesc.fViewportHeight =  (_float)ViewportDesc.Height;
    if (FAILED(Add_Component(m_iCurLevelID, L"Prototype_Component_Ray",
            TEXT("Com_Ray"), reinterpret_cast<CComponent**>(&m_pRayCom),&RayDesc)))
        return E_FAIL;


    //if (FAILED(Ready_Components(pDesc)))
    //    return E_FAIL;

    return S_OK;
}

void CMapObject::Priority_Update(_float _fTimeDelta)
{
    CPartObject::Priority_Update(_fTimeDelta);
}

void CMapObject::Update(_float _fTimeDelta)
{
    if (m_eMode == MODE::PICKING)
    {
        _float4x4 fMat = {};
        _float4x4 fMatView = m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW);
        _float4x4 fMatProj = m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ);
        RECT rect{};
        GetWindowRect(g_hWnd, &rect);
        ImGuizmo::SetRect((_float)rect.left, (_float)rect.top, (_float)g_iWinSizeX, (_float)g_iWinSizeY);
        ImGuizmo::Manipulate(fMatView.m[0],
            fMatProj.m[0], (ImGuizmo::OPERATION)m_CurrentGizmoOperation,
            ImGuizmo::WORLD,
            m_matWorld.m[0]
        );

        for (_uint i = 0; i < CTransform::STATE_END; i++)
        {
            Set_WorldMatrix(m_matWorld);
        }
    }

    /* Update Parent Matrix */
    CPartObject::Update(_fTimeDelta);
}

void CMapObject::Late_Update(_float _fTimeDelta)
{

    /* Add Render Group */
    if (COORDINATE_3D == m_pControllerTransform->Get_CurCoord())
        m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
    else if (COORDINATE_2D == m_pControllerTransform->Get_CurCoord())
        m_pGameInstance->Add_RenderObject(CRenderer::RG_BOOK_2D, this);

    /* Update Parent Matrix */
    CPartObject::Late_Update(_fTimeDelta);
}

HRESULT CMapObject::Render()
{
    if (FAILED(CModelObject::Bind_ShaderResources_WVP()))
        return E_FAIL;

    CModelObject::Render();

    return S_OK;
}

HRESULT CMapObject::Render_Shadow()
{
    return S_OK;
}

void CMapObject::Create_Complete()
{
    m_eMode = NORMAL; 
    XMStoreFloat4x4(&m_matWorld, Get_WorldMatrix());
}


CMapObject* CMapObject::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CMapObject* pInstance = new CMapObject(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        //MSG_BOX("Failed to Created : CMapObject");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CMapObject::Clone(void* _pArg)
{
    CMapObject* pInstance = new CMapObject(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CMapObject");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMapObject::Free()
{


    __super::Free();
}
