#include "stdafx.h"
#include "TurnBookEffect.h"
#include "GameInstance.h"
#include "Effect_Beam.h"

CTurnBookEffect::CTurnBookEffect(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CPartObject(_pDevice, _pContext)
{
}

CTurnBookEffect::CTurnBookEffect(const CTurnBookEffect& _Prototype)
	: CPartObject(_Prototype)
{
}

HRESULT CTurnBookEffect::Initialize(void* _pArg)
{
    TURNBOOKEFFECT_DESC* pDesc = static_cast<TURNBOOKEFFECT_DESC*>(_pArg);

    if (nullptr == pDesc)
        return E_FAIL;

    pDesc->eStartCoord = COORDINATE_3D;
    pDesc->isCoordChangeEnable = false;

    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    CEffect_Beam::EFFECTBEAM_DESC EffectBeamDesc = {};
    EffectBeamDesc.iCurLevelID = m_iCurLevelID;
    EffectBeamDesc.isCoordChangeEnable = false;
    EffectBeamDesc.eStartCoord = COORDINATE_3D;
    EffectBeamDesc.szBufferTag = L"Prototype_Component_VIBuffer_Beam16";
    EffectBeamDesc.szTextureTag = L"Prototype_Component_Texture_Grad01_180";
    EffectBeamDesc.vRandomMin = _float3(0.f, 0.f, 0.f);
    EffectBeamDesc.vRandomMax = _float3(0.f, 0.f, 0.f);
    EffectBeamDesc.fWidth = 0.2f;
    EffectBeamDesc.vColor = _float4(0.85f, 0.525f, 0.12f, 2.5f);
    EffectBeamDesc.isConverge = false;
    EffectBeamDesc.isRenderPoint = false;
    EffectBeamDesc.pParentMatrices[COORDINATE_3D] = pDesc->pBodyMatrix;
    EffectBeamDesc.pStartPointBoneMatrix = pDesc->pLHandMatrix;

    m_pLHandBeam = static_cast<CEffect_Beam*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_EffectBeam"), &EffectBeamDesc));
    if (nullptr == m_pLHandBeam)
        return E_FAIL;

    EffectBeamDesc.pStartPointBoneMatrix = pDesc->pRHandMatrix;

    m_pRHandBeam = static_cast<CEffect_Beam*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_EffectBeam"), &EffectBeamDesc));
    if (nullptr == m_pRHandBeam)
        return E_FAIL;

    CEffect_System::EFFECT_SYSTEM_DESC EffectDesc = {};
    EffectDesc.eStartCoord = COORDINATE_3D;
    EffectDesc.isCoordChangeEnable = false;
    EffectDesc.iSpriteShaderLevel = LEVEL_STATIC;
    EffectDesc.szSpriteShaderTags = L"Prototype_Component_Shader_VtxPointInstance";
    EffectDesc.iEffectShaderLevel = LEVEL_STATIC;
    EffectDesc.szEffectShaderTags = L"Prototype_Component_Shader_VtxMeshEffect";
    EffectDesc.szSpriteComputeShaderTag = L"Prototype_Component_Compute_Shader_SpriteInstance";
    EffectDesc.pParentMatrices[COORDINATE_3D] = pDesc->pBodyMatrix;

    m_pLHandRing = static_cast<CEffect_System*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("TurnBookRing.json"), &EffectDesc));
    if (nullptr == m_pLHandRing)
        return E_FAIL;
    m_pLHandRing->Set_SpawnMatrix(pDesc->pLHandMatrix);

    m_pRHandRing = static_cast<CEffect_System*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("TurnBookRing.json"), &EffectDesc));
    if (nullptr == m_pRHandRing)
        return E_FAIL;
    m_pRHandRing->Set_SpawnMatrix(pDesc->pRHandMatrix);

	return S_OK;
}

void CTurnBookEffect::Update(_float _fTimeDelta)
{
    if (nullptr != m_pLHandBeam)
        m_pLHandBeam->Update(_fTimeDelta);
    if (nullptr != m_pRHandBeam)
        m_pRHandBeam->Update(_fTimeDelta);
    if (nullptr != m_pLHandRing)
        m_pLHandRing->Update(_fTimeDelta);
    if (nullptr != m_pRHandRing)
        m_pRHandRing->Update(_fTimeDelta);
}

void CTurnBookEffect::Late_Update(_float _fTimeDelta)
{
    if (nullptr != m_pLHandBeam)
        m_pLHandBeam->Late_Update(_fTimeDelta);
    if (nullptr != m_pRHandBeam)
        m_pRHandBeam->Late_Update(_fTimeDelta);
    if (nullptr != m_pLHandRing)
        m_pLHandRing->Late_Update(_fTimeDelta);
    if (nullptr != m_pRHandRing)
        m_pRHandRing->Late_Update(_fTimeDelta);
}

void CTurnBookEffect::Set_BookPosition(_fvector _vPosition)
{
    if (nullptr != m_pLHandBeam)
        m_pLHandBeam->Set_EndPosition(_vPosition);
    if (nullptr != m_pRHandBeam)
        m_pRHandBeam->Set_EndPosition(_vPosition);

}

void CTurnBookEffect::Active_OnEnable()
{
    __super::Active_OnEnable();

    if (nullptr != m_pLHandRing)
        m_pLHandRing->Active_Effect(true);
    if (nullptr != m_pRHandRing)
        m_pRHandRing->Active_Effect(true);
}

void CTurnBookEffect::Active_OnDisable()
{
    if (nullptr != m_pLHandRing)
        m_pLHandRing->Inactive_Effect();
    if (nullptr != m_pRHandRing)
        m_pRHandRing->Inactive_Effect();
}

CTurnBookEffect* CTurnBookEffect::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CTurnBookEffect* pInstance = new CTurnBookEffect(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CTurnBookEffect");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CTurnBookEffect::Clone(void* _pArg)
{
    CTurnBookEffect* pInstance = new CTurnBookEffect(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CTurnBookEffect");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CTurnBookEffect::Free()
{
    Safe_Release(m_pLHandBeam);
    Safe_Release(m_pRHandBeam);
    Safe_Release(m_pLHandRing);
    Safe_Release(m_pRHandRing);

	__super::Free();
}
