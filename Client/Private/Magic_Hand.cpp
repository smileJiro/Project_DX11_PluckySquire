#include "stdafx.h"
#include "Magic_Hand.h"
#include "GameInstance.h"

#include "Magic_Hand_Body.h"
#include "Effect_System.h"

CMagic_Hand::CMagic_Hand(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CContainerObject(_pDevice, _pContext)
{
}

CMagic_Hand::CMagic_Hand(const CMagic_Hand& _Prototype)
    : CContainerObject(_Prototype)
{
}

HRESULT CMagic_Hand::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CMagic_Hand::Initialize(void* _pArg)
{
    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;



    if (FAILED(Ready_PartObjects()))
        return E_FAIL;

    m_pEffectSystem->Inactive_All();

    return S_OK;
}

void CMagic_Hand::Priority_Update(_float _fTimeDelta)
{
    if (false == m_isTrigger)
        return;
    __super::Priority_Update(_fTimeDelta);
}

void CMagic_Hand::Update(_float _fTimeDelta)
{
    if (false == m_isTrigger)
        return;

#ifdef _DEBUG
    ImGui::Begin("Hand");

    if (ImGui::Button("Replay All"))
    {
        m_pMagicHandbody->Set_Animation(0);
        m_pEffectSystem->Active_All(true);
    }

    ImGui::End();
#endif
    __super::Update(_fTimeDelta);
}

void CMagic_Hand::Late_Update(_float _fTimeDelta)
{
    if (false == m_isTrigger)
        return;

    __super::Late_Update(_fTimeDelta);

}

HRESULT CMagic_Hand::Render()
{
    return S_OK;
}

HRESULT CMagic_Hand::Ready_PartObjects()
{
    CModelObject::MODELOBJECT_DESC Desc = {};
    Desc.eStartCoord = COORDINATE_3D;
    Desc.iCurLevelID = m_iCurLevelID;
    Desc.isCoordChangeEnable = true;
    Desc.pParentMatrices[COORDINATE_3D] = m_pControllerTransform->Get_WorldMatrix_Ptr();

    Desc.iModelPrototypeLevelID_3D = m_iCurLevelID;
    Desc.strModelPrototypeTag_3D = L"Prototype_Model_MagicHand";
    Desc.strShaderPrototypeTag_3D = L"Prototype_Component_Shader_VtxAnimMesh";
    Desc.iShaderPass_3D = 6;
    Desc.iPriorityID_3D = PR3D_BLEND;
    Desc.iRenderGroupID_3D = RG_3D;
    Desc.tTransform3DDesc.fSpeedPerSec = 1.f;

    //Desc.iModelPrototypeLevelID_2D = m_iCurLevelID;
    //Desc.strModelPrototypeTag_2D = L"MagicHand";
    //Desc.strShaderPrototypeTag_2D = L"Prototype_Component_Shader_VtxPosTex";
    //Desc.iShaderPass_2D = 4;
    //Desc.iPriorityID_2D = PR2D_


    m_PartObjects[MAGICHAND_BODY] = m_pMagicHandbody = static_cast<CMagic_Hand_Body*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, m_iCurLevelID, TEXT("Prototype_GameObject_MagicHandBody"), &Desc));
    Safe_AddRef(m_pMagicHandbody);

    const _float4x4* pMatrix = static_cast<C3DModel*>(m_pMagicHandbody->Get_Model(COORDINATE_3D))->Get_BoneMatrix("j_palm");

    CEffect_System::EFFECT_SYSTEM_DESC EffectDesc = {};

    EffectDesc.eStartCoord = COORDINATE_3D;
    EffectDesc.iCurLevelID = m_iCurLevelID;
    EffectDesc.isCoordChangeEnable = false;
    EffectDesc.iSpriteShaderLevel = LEVEL_STATIC;
    EffectDesc.szSpriteShaderTags = L"Prototype_Component_Shader_VtxPointInstance";
    EffectDesc.iModelShaderLevel = LEVEL_STATIC;
    EffectDesc.szModelShaderTags = L"Prototype_Component_Shader_VtxMeshInstance";
    EffectDesc.iEffectShaderLevel = LEVEL_STATIC;
    EffectDesc.szEffectShaderTags = L"Prototype_Component_Shader_VtxMeshEffect";
    EffectDesc.iSingleSpriteShaderLevel = LEVEL_STATIC;
    EffectDesc.szSingleSpriteShaderTags = L"Prototype_Component_Shader_VtxPoint";
    EffectDesc.iSingleSpriteBufferLevel = LEVEL_STATIC;
    EffectDesc.szSingleSpriteBufferTags = L"Prototype_Component_VIBuffer_Point";
    EffectDesc.szSpriteComputeShaderTag = L"Prototype_Component_Compute_Shader_SpriteInstance";
    EffectDesc.szMeshComputeShaderTag = L"Prototype_Component_Compute_Shader_MeshInstance";
    // Level ¹Ù²ã¾ßÇÔ
    m_PartObjects[MAGICHAND_EFFECT] = m_pEffectSystem = static_cast<CEffect_System*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("BookOut.json"), &EffectDesc));
    m_pEffectSystem->Set_Position(XMVectorSet(2.f, 0.4f, -17.3f, 1.f));
    m_pEffectSystem->Set_SpawnMatrix(pMatrix);

    return S_OK;
}

CMagic_Hand* CMagic_Hand::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CMagic_Hand* pInstance = new CMagic_Hand(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CMagic_Hand");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CMagic_Hand::Clone(void* _pArg)
{
    CMagic_Hand* pInstance = new CMagic_Hand(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CMagic_Hand");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMagic_Hand::Free()
{
    Safe_Release(m_pMagicHandbody);
   
    __super::Free();
}