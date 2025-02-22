#include "stdafx.h"
#include "Sword_Trail.h"
#include "GameInstance.h"

CSword_Trail::CSword_Trail(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CPartObject(_pDevice, _pContext)
{
}

CSword_Trail::CSword_Trail(const CSword_Trail& _Prototype)
	: CPartObject(_Prototype)
{
}

HRESULT CSword_Trail::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSword_Trail::Initialize(void* _pArg)
{
    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CSword_Trail::Update(_float _fTimeDelta)
{
    //ImGui::Begin("Control Trail");

    //static _float fTime;
    //if (ImGui::InputFloat("DisappearTime", &fTime))
    //{
    //    m_pBufferCom->Set_DisappearTime(fTime);
    //}

    //ImGui::End();
    
    __super::Update(_fTimeDelta);

    m_fAccTime += _fTimeDelta;

    
}

void CSword_Trail::Late_Update(_float _fTimeDelta)
{
	m_pGameInstance->Add_RenderObject_New(RG_3D, PR3D_PARTICLE, this);
}

HRESULT CSword_Trail::Render()
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

void CSword_Trail::Add_Point(_fvector _vPosition)
{    
    m_pBufferCom->Update_Point(&m_WorldMatrices[COORDINATE_3D], _vPosition);
}

void CSword_Trail::Delete_Effect()
{
    m_pBufferCom->Reset_Buffer();
}

HRESULT CSword_Trail::Bind_ShaderResources()
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

    _float fLength = 1.f;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fLength", &fLength, sizeof(_float))))
        return E_FAIL;

    _float4 vPosition;
    memcpy(&vPosition, &m_WorldMatrices[COORDINATE_3D].m[3], sizeof(_float4));

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCentral", &vPosition, sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pControllerTransform->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;


    return S_OK;
}

HRESULT CSword_Trail::Ready_Components()
{
    if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Trail"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_SwordTrail"),
        TEXT("Com_Buffer"), reinterpret_cast<CComponent**>(&m_pBufferCom))))
        return E_FAIL;

    if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Trail"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

CSword_Trail* CSword_Trail::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CSword_Trail* pInstance = new CSword_Trail(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CSword_Trail");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CSword_Trail::Clone(void* _pArg)
{
    CSword_Trail* pInstance = new CSword_Trail(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CSword_Trail");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CSword_Trail::Free()
{
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pBufferCom);
    Safe_Release(m_pTextureCom);

    __super::Free();
}

HRESULT CSword_Trail::Cleanup_DeadReferences()
{
	return S_OK;
}
