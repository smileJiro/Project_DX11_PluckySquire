#include "TrailInstance.h"
#include "GameInstance.h"


CTrailInstance::CTrailInstance()
    : CBase()
{
}

HRESULT CTrailInstance::Initialize(TRAIL_DESC* _pTrailDesc)
{
    m_WorldMatrix = _pTrailDesc->WorldMatrix; // Ref로 받고 Copy
    m_vTrailTime.x = _pTrailDesc->fTrailTime;
    m_vTrailColor = _pTrailDesc->vTrailColor;
    m_iNumMeshes = _pTrailDesc->iNumMeshes;
    m_MeshesBoneMatrices = _pTrailDesc->MeshesBoneMatrices;

    return S_OK;
}

void CTrailInstance::Update(_float _fTimeDelta)
{
    if (true == m_isFinished)
        return;

    m_vTrailTime.y += _fTimeDelta;
    if (m_vTrailTime.x <= m_vTrailTime.y)
    {
        m_isFinished = true;
    }
}

HRESULT CTrailInstance::Render(CShader* _pShaderCom, C3DModel* _pModelCom)
{
    /* 범용적으로 사용되는 Trail Resources Bind */
    Bind_TrailResources(_pShaderCom);
    _uint iShaderPass = {};
    if (CModel::ANIM == _pModelCom->Is_AnimModel())
        iShaderPass = (_uint)PASS_VTXANIMMESH::TRAIL;
    else
        iShaderPass = (_uint)PASS_VTXMESH::TRAIL;

    _pModelCom->Render_Default(_pShaderCom, iShaderPass);

    return S_OK;
}

HRESULT CTrailInstance::Bind_TrailMatrices(const _char* _pConstantName, _int _iMeshIndex, CShader* _pShaderCom)
{
    if (true == m_isFinished)
        return E_FAIL;
    
    if (FAILED(_pShaderCom->Bind_Matrices(_pConstantName, m_MeshesBoneMatrices[_iMeshIndex].data(), 256)))
        return E_FAIL;
    
    return S_OK;
}

HRESULT CTrailInstance::Bind_TrailResources(CShader* _pShaderCom)
{
    if (FAILED(_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
        return E_FAIL;

    if (FAILED(_pShaderCom->Bind_RawValue("g_vTrailColor", &m_vTrailColor, sizeof _float4)))
        return E_FAIL;

    if (FAILED(_pShaderCom->Bind_RawValue("g_vTrailTime", &m_vTrailTime, sizeof _float2)))
        return E_FAIL;
    
    return S_OK;
}

CTrailInstance* CTrailInstance::Create(TRAIL_DESC* _pTrailDesc)
{
    CTrailInstance* pInstance = new CTrailInstance;

    if (FAILED(pInstance->Initialize(_pTrailDesc)))
    {
        MSG_BOX("Trail Instance Create Failed");
        return nullptr;
    }

    return pInstance;
}

void CTrailInstance::Free()
{

    __super::Free();
}
