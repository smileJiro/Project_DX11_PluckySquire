#include "pch.h"
#include "TestTrail.h"
#include "GameInstance.h"
#include "EffectTool_Defines.h"

CTestTrail::CTestTrail(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CGameObject(_pDevice, _pContext)
{
}

CTestTrail::CTestTrail(const CTestTrail& _Prototype)
    : CGameObject(_Prototype)
{
}

HRESULT CTestTrail::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CTestTrail::Initialize(void* _pArg)
{
    CGameObject::GAMEOBJECT_DESC Desc = {};
    Desc.eStartCoord = COORDINATE_3D;
    Desc.iCurLevelID = LEVEL_TOOL;
    Desc.isCoordChangeEnable = false;
    Desc.tTransform3DDesc.fRotationPerSec = XMConvertToRadians(18.f);
    Desc.tTransform3DDesc.fSpeedPerSec = 0.5f;

    if (FAILED(__super::Initialize(&Desc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;


    m_pControllerTransform->Set_State(CTransform::STATE_POSITION, _float4(0.f, 0.f, 0.f, 1.f));
    return S_OK;
}

void CTestTrail::Update(_float _fTimeDelta)
{
    m_fAccTime += _fTimeDelta;

    //if (0.2f <= m_fAccTime)
    //{
    //    m_pBufferCom->Add_Point(m_pControllerTransform->Get_WorldMatrix_Ptr(), XMVectorSet(0.f, 0.5f, 0.f, 1.f), XMVectorSet(0.f, -0.5f, 0.f, 1.f));
    //
    //    m_fAccTime = 0.f;
    //}

    m_pBufferCom->Update(_fTimeDelta);

}

void CTestTrail::Late_Update(_float _fTimeDelta)
{
    m_pGameInstance->Add_RenderObject_New(RG_3D, PR3D_PARTICLE, this);
}

HRESULT CTestTrail::Render()
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

HRESULT CTestTrail::Bind_ShaderResources()
{
    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

    //_float4x4 matIdentity;
    //XMStoreFloat4x4(&matIdentity, XMMatrixIdentity());
    //if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &matIdentity)))
    //    return E_FAIL;
    
    // TEST 
    _uint iCount = 32;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_iCount", &iCount, sizeof(_uint))))
        return E_FAIL;

    _float fLength = 1.f; 
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fLength", &fLength, sizeof(_float))))
        return E_FAIL;

    _float4 vPosition;
    XMStoreFloat4(&vPosition, m_pControllerTransform->Get_State(CTransform::STATE_POSITION));

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

HRESULT CTestTrail::Ready_Components()
{
    if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Trail"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Trail"),
        TEXT("Com_Buffer"), reinterpret_cast<CComponent**>(&m_pBufferCom))))
        return E_FAIL;

    if (FAILED(Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Texture_Trail"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;
        
    return S_OK;
}

CTestTrail* CTestTrail::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CTestTrail* pInstance = new CTestTrail(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CTestTrail");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CTestTrail::Clone(void* _pArg)
{
    CTestTrail* pInstance = new CTestTrail(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CTestTrail");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CTestTrail::Free()
{
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pBufferCom);
    Safe_Release(m_pTextureCom);

    __super::Free();
}

HRESULT CTestTrail::Cleanup_DeadReferences()
{
    return S_OK;
}
