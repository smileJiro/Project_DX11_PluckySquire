#include "stdafx.h"
#include "Excavator_Centre.h"
#include "GameInstance.h"

#include "Excavator_Switch.h"
#include "Saw.h"

CExcavator_Centre::CExcavator_Centre(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CCharacter(_pDevice, _pContext)
{
}

CExcavator_Centre::CExcavator_Centre(const CExcavator_Centre& _Prototype)
    :CCharacter(_Prototype)
{
}

HRESULT CExcavator_Centre::Initialize(void* _pArg)
{
    CENTRE_DESC* pDesc = static_cast<CENTRE_DESC*>(_pArg);
    // Save Desc

    // Add Desc
    pDesc->eStartCoord = COORDINATE_2D;
    pDesc->iNumPartObjects = CENTRE_LAST;
    pDesc->isCoordChangeEnable = false;
    pDesc->iObjectGroupID = OBJECT_GROUP::MONSTER;

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_PartObjects(pDesc)))
        return E_FAIL;
    return S_OK;
}

void CExcavator_Centre::Priority_Update(_float _fTimeDelta)
{

    __super::Priority_Update(_fTimeDelta);
}

void CExcavator_Centre::Update(_float _fTimeDelta)
{
    Check_SwitchCount();
    __super::Update(_fTimeDelta);
}

void CExcavator_Centre::Late_Update(_float _fTimeDelta)
{

    __super::Late_Update(_fTimeDelta);
}

HRESULT CExcavator_Centre::Ready_PartObjects(CENTRE_DESC* _pDesc)
{
    {/* Part CENTRE_BG */
        CModelObject::MODELOBJECT_DESC Desc{};
        Desc.iCurLevelID = m_iCurLevelID;
        Desc.pParentMatrices[COORDINATE_2D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_2D);
        Desc.Build_2D_Transform(_float2(0.0f, -100.f), _float2(6000.f, 365.0f));
        Desc.Build_2D_Model(LEVEL_STATIC, TEXT("Prototype_Model2D_Rectangle"), TEXT("Prototype_Component_Shader_VtxPosTex"), (_uint)PASS_VTXPOSTEX::COLOR_ALPHA);
        Desc.eStartCoord = COORDINATE_2D;
        
        m_PartObjects[CENTRE_BG] = static_cast<CPartObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &Desc));
        if (nullptr == m_PartObjects[CENTRE_BG])
            return E_FAIL;
        static_cast<CModelObject*>(m_PartObjects[CENTRE_BG])->Set_PosTexColor(_float4(0.01f, 0.01f, 0.01f,1.0f));
    }/* Part CENTRE_BG */

    {/* Part CENTRE_TROOPER_L */
        CModelObject::MODELOBJECT_DESC Desc{};
        Desc.iCurLevelID = m_iCurLevelID;
        Desc.pParentMatrices[COORDINATE_2D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_2D);
        Desc.Build_2D_Transform(_float2(-315.0f, -150.0f), _float2(1.0f, 1.0f));
        Desc.Build_2D_Model(m_iCurLevelID, TEXT("Trooper"), TEXT("Prototype_Component_Shader_VtxPosTex"));
        Desc.eStartCoord = COORDINATE_2D;
    
        m_PartObjects[CENTRE_TROOPER_L] = static_cast<CPartObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &Desc));
        if (nullptr == m_PartObjects[CENTRE_TROOPER_L])
            return E_FAIL;
        static_cast<CModelObject*>(m_PartObjects[CENTRE_TROOPER_L])->Switch_Animation(0);
    }/* Part CENTRE_TROOPER_L */
    
    {/* Part CENTRE_TROOPER_R */
        CModelObject::MODELOBJECT_DESC Desc{};
        Desc.iCurLevelID = m_iCurLevelID;
        Desc.pParentMatrices[COORDINATE_2D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_2D);
        Desc.Build_2D_Transform(_float2(305.0f, -150.0f), _float2(1.0f, 1.0f));
        Desc.Build_2D_Model(m_iCurLevelID, TEXT("Trooper"), TEXT("Prototype_Component_Shader_VtxPosTex"));
        Desc.eStartCoord = COORDINATE_2D;

        m_PartObjects[CENTRE_TROOPER_R] = static_cast<CPartObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &Desc));
        if (nullptr == m_PartObjects[CENTRE_TROOPER_R])
            return E_FAIL;
        static_cast<CModelObject*>(m_PartObjects[CENTRE_TROOPER_R])->Switch_Animation(0);
    }/* Part CENTRE_TROOPER_R */

    {/* Part CENTRE_CENTRE */
        CModelObject::MODELOBJECT_DESC Desc{};
        Desc.iCurLevelID = m_iCurLevelID;
        Desc.pParentMatrices[COORDINATE_2D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_2D);
        Desc.Build_2D_Transform(_float2(0.0f, 0.0f), _float2(1.0f, 1.0f));
        Desc.Build_2D_Model(m_iCurLevelID, TEXT("Excavator_Centre"), TEXT("Prototype_Component_Shader_VtxPosTex"));
        Desc.eStartCoord = COORDINATE_2D;

        m_PartObjects[CENTRE_CENTRE] = static_cast<CPartObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &Desc));
        if (nullptr == m_PartObjects[CENTRE_CENTRE])
            return E_FAIL;
        static_cast<CModelObject*>(m_PartObjects[CENTRE_CENTRE])->Switch_Animation(0);
    }/* Part CENTRE_CENTRE */

    {/* Part CENTRE_REGULATOR */
        CModelObject::MODELOBJECT_DESC Desc{};
        Desc.iCurLevelID = m_iCurLevelID;
        Desc.pParentMatrices[COORDINATE_2D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_2D);
        Desc.Build_2D_Transform(_float2(0.0f, -80.0f), _float2(1.0f, 1.0f));
        Desc.Build_2D_Model(m_iCurLevelID, TEXT("Excavator_Regulator"), TEXT("Prototype_Component_Shader_VtxPosTex"));
        Desc.eStartCoord = COORDINATE_2D;

        m_PartObjects[CENTRE_REGULATOR] = static_cast<CPartObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &Desc));
        if (nullptr == m_PartObjects[CENTRE_REGULATOR])
            return E_FAIL;
        static_cast<CModelObject*>(m_PartObjects[CENTRE_REGULATOR])->Switch_Animation(1);
    }/* Part CENTRE_REGULATOR */

    {/* Part CENTRE_SUTTER */
        CModelObject::MODELOBJECT_DESC Desc{};
        Desc.iCurLevelID = m_iCurLevelID;
        Desc.pParentMatrices[COORDINATE_2D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_2D);
        Desc.Build_2D_Transform(_float2(0.0f, -2.0f), _float2(1.0f, 1.0f));
        Desc.Build_2D_Model(m_iCurLevelID, TEXT("Shutter"), TEXT("Prototype_Component_Shader_VtxPosTex"));
        Desc.eStartCoord = COORDINATE_2D;

        m_PartObjects[CENTRE_SUTTER] = static_cast<CPartObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &Desc));
        if (nullptr == m_PartObjects[CENTRE_SUTTER])
            return E_FAIL;
        static_cast<CModelObject*>(m_PartObjects[CENTRE_SUTTER])->Switch_Animation(1);
    }/* Part CENTRE_SUTTER */

    {/* Part CENTRE_SWITCH_0 */
        CExcavator_Switch::SWITCH_DESC Desc{};
        Desc.iCurLevelID = m_iCurLevelID;
        Desc.pParentMatrices[COORDINATE_2D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_2D);
        Desc.Build_2D_Transform(_float2(-120.0f, -130.0f), _float2(1.0f, 1.0f));

        m_PartObjects[CENTRE_SWITCH_0] = static_cast<CPartObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_CHAPTER_6, TEXT("Prototype_GameObject_Excavator_Switch"), &Desc));
        if (nullptr == m_PartObjects[CENTRE_SWITCH_0])
            return E_FAIL;
        static_cast<CModelObject*>(m_PartObjects[CENTRE_SWITCH_0])->Switch_Animation(0);
    }/* Part CENTRE_SWITCH_0 */
    
    {/* Part CENTRE_SWITCH_1 */
        CExcavator_Switch::SWITCH_DESC Desc{};
        Desc.iCurLevelID = m_iCurLevelID;
        Desc.pParentMatrices[COORDINATE_2D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_2D);
        Desc.Build_2D_Transform(_float2(0.0f, -130.0f), _float2(1.0f, 1.0f));

        m_PartObjects[CENTRE_SWITCH_1] = static_cast<CPartObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_CHAPTER_6, TEXT("Prototype_GameObject_Excavator_Switch"), &Desc));
        if (nullptr == m_PartObjects[CENTRE_SWITCH_1])
            return E_FAIL;
        static_cast<CModelObject*>(m_PartObjects[CENTRE_SWITCH_1])->Switch_Animation(0);
    }/* Part CENTRE_SWITCH_1 */

    {/* Part CENTRE_SWITCH_2 */
        CExcavator_Switch::SWITCH_DESC Desc{};
        Desc.iCurLevelID = m_iCurLevelID;
        Desc.pParentMatrices[COORDINATE_2D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_2D);
        Desc.Build_2D_Transform(_float2(120.0f, -130.0f), _float2(1.0f, 1.0f));

        m_PartObjects[CENTRE_SWITCH_2] = static_cast<CPartObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_CHAPTER_6, TEXT("Prototype_GameObject_Excavator_Switch"), &Desc));
        if (nullptr == m_PartObjects[CENTRE_SWITCH_2])
            return E_FAIL;
        static_cast<CModelObject*>(m_PartObjects[CENTRE_SWITCH_2])->Switch_Animation(0);
    }/* Part CENTRE_SWITCH_2 */




    
    return S_OK;
}

void CExcavator_Centre::Check_SwitchCount()
{
    if (true == m_isFirst)
        return;
    m_iSwitchCount = 0;

    if (true == m_PartObjects[CENTRE_PART::CENTRE_SWITCH_0]->Is_Active())
        ++m_iSwitchCount;

    if (true == m_PartObjects[CENTRE_PART::CENTRE_SWITCH_1]->Is_Active())
        ++m_iSwitchCount;

    if (true == m_PartObjects[CENTRE_PART::CENTRE_SWITCH_2]->Is_Active())
        ++m_iSwitchCount;

    if (0 == m_iSwitchCount)
    {
        m_isFirst = true;
        static_cast<CModelObject*>(m_PartObjects[CENTRE_PART::CENTRE_SUTTER])->Switch_Animation(0);
    }

}

CExcavator_Centre* CExcavator_Centre::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CExcavator_Centre* pInstance = new CExcavator_Centre(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CExcavator_Centre");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CExcavator_Centre::Clone(void* _pArg)
{
    CExcavator_Centre* pInstance = new CExcavator_Centre(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CExcavator_Centre");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CExcavator_Centre::Free()
{

    __super::Free();
}
