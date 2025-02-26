#include "stdafx.h"
#include "Effect_Trail.h"
#include "GameInstance.h"

CEffect_Trail::CEffect_Trail(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CPartObject(_pDevice, _pContext)
{
}

CEffect_Trail::CEffect_Trail(const CEffect_Trail& _Prototype)
	: CPartObject(_Prototype)
{
}

HRESULT CEffect_Trail::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEffect_Trail::Initialize(void* _pArg)
{
    EFFECTTRAIL_DESC* pDesc = static_cast<EFFECTTRAIL_DESC*>(_pArg);

    if (nullptr == pDesc)
        return E_FAIL;

    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components(pDesc)))
        return E_FAIL;

    m_fLength = pDesc->fLength;
    m_vAddPoint = pDesc->vAddPoint;
    m_vColor = pDesc->vColor;
    m_fAddTime = pDesc->fAddTime;
    m_fTrailLifeTime = pDesc->fTrailLifeTime;

    return S_OK;
}

void CEffect_Trail::Update(_float _fTimeDelta)
{   
    __super::Update(_fTimeDelta);

//#ifdef _DEBUGsdda
//    ImGui::Begin("Set Trail Effect");
//
//    ImGui::DragFloat4("Color", (_float*)(&m_vColor), 0.01f);
//    ImGui::DragFloat("Length", &m_fLength);
//    ImGui::DragFloat3("Add Point", (_float*)&m_vAddPoint);
//    ImGui::DragFloat("Add Time", &m_fAddTime);
//    ImGui::DragFloat("Delete Time", &m_fTrailLifeTime);
//
//
//    ImGui::End();
//#endif

    // 추가될 정점으로 update 합니다.
    if (m_isAddUpdate)
    {
        m_fAccAddTime += _fTimeDelta;

        if (m_fAddTime <= m_fAccAddTime)
        {
            m_pBufferCom->Add_Point(&m_WorldMatrices[COORDINATE_3D], XMVectorSetW(XMLoadFloat3(&m_vAddPoint), 1.f), m_fTrailLifeTime);

            m_fAccAddTime = 0.f;
        }
    }
    
    // 마지막 정점을 삭제합니다.
    //if (m_isDeleteUpdate)
    //{
    //    m_fAccDeleteTime += _fTimeDelta;
    //
    //    if (m_fDeleteTime <= m_fAccDeleteTime)
    //    {
    //        m_pBufferCom->Delete_Point();
    //
    //        m_fAccDeleteTime = 0.f;
    //    }
    //}
    
    m_fDeleteAllDelay -= _fTimeDelta;

    // Delay 시간 이후 모든 정점 Reset합니다.
    if (0.f > m_fDeleteAllDelay)
    {
        m_pBufferCom->Reset_Buffer();
        m_fDeleteAllDelay = D3D11_FLOAT32_MAX;
    }
}

void CEffect_Trail::Late_Update(_float _fTimeDelta)
{
	m_pGameInstance->Add_RenderObject_New(RG_3D, PR3D_PARTICLE, this);
}

HRESULT CEffect_Trail::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin_NoInput(0)))
        return E_FAIL;

    if (FAILED(m_pBufferCom->Bind_BufferDesc()))
        return E_FAIL;
    if (FAILED(m_pBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

void CEffect_Trail::Add_Point(_fvector _vPosition)
{    
    m_pBufferCom->Add_Point(&m_WorldMatrices[COORDINATE_3D], _vPosition, m_fTrailLifeTime);
}

void CEffect_Trail::Add_Point()
{
    m_pBufferCom->Add_Point(&m_WorldMatrices[COORDINATE_3D], XMVectorSetW(XMLoadFloat3(&m_vAddPoint), 1.f), m_fTrailLifeTime);
}

void CEffect_Trail::Delete_Effect()
{
    m_pBufferCom->Reset_Buffer();
    m_fAccAddTime = 0.f;

}

void CEffect_Trail::Delete_Delay(_float _fDelay)
{
    m_fDeleteAllDelay = _fDelay;
    m_fAccAddTime = 0.f;

}

HRESULT CEffect_Trail::Bind_ShaderResources()
{
    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

    //_float4x4 matIdentity;
    //XMStoreFloat4x4(&matIdentity, XMMatrixIdentity());
    //if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &matIdentity)))
    //    return E_FAIL;

    _uint iCount = m_pBufferCom->Get_TrailCount();
    if (FAILED(m_pShaderCom->Bind_RawValue("g_iCount", &iCount, sizeof(_uint))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fLength", &m_fLength, sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4))))
        return E_FAIL;

    _float4 vPosition;
    memcpy(&vPosition, &m_WorldMatrices[COORDINATE_3D].m[3], sizeof(_float4));
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCentral", &vPosition, sizeof(_float4))))
        return E_FAIL;


    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;


    return S_OK;
}

HRESULT CEffect_Trail::Ready_Components(const EFFECTTRAIL_DESC* _pDesc)
{
    if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Trail"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(Add_Component(LEVEL_STATIC, _pDesc->szBufferTag,
        TEXT("Com_Buffer"), reinterpret_cast<CComponent**>(&m_pBufferCom))))
        return E_FAIL;

    if (FAILED(Add_Component(LEVEL_STATIC, _pDesc->szTextureTag,
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    //if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_SwordTrail"),
    //    TEXT("Com_Buffer"), reinterpret_cast<CComponent**>(&m_pBufferCom))))
    //    return E_FAIL;

    //if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Trail"),
    //    TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
    //    return E_FAIL;

    return S_OK;
}

CEffect_Trail* CEffect_Trail::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CEffect_Trail* pInstance = new CEffect_Trail(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CEffect_Trail");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CEffect_Trail::Clone(void* _pArg)
{
    CEffect_Trail* pInstance = new CEffect_Trail(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CEffect_Trail");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CEffect_Trail::Free()
{
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pBufferCom);
    Safe_Release(m_pTextureCom);

    __super::Free();
}

HRESULT CEffect_Trail::Cleanup_DeadReferences()
{
	return S_OK;
}
