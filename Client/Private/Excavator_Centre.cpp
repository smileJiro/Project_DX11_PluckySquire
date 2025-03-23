#include "stdafx.h"
#include "Excavator_Centre.h"
#include "GameInstance.h"
#include "Section_Manager.h"
#include "ExcavatorGame.h"
#include "Effect2D_Manager.h"

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

    if (FAILED(Ready_Components(pDesc)))
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

HRESULT CExcavator_Centre::Render()
{
#ifdef _DEBUG
    if (m_p2DColliderComs[0]->Is_Active())
        m_p2DColliderComs[0]->Render(SECTION_MGR->Get_Section_RenderTarget_Size(m_strSectionName));

#endif // _DEBUG

    return S_OK;
}

void CExcavator_Centre::On_Hit(CGameObject* _pHitter, _int _iDamg, _fvector _vForce)
{
    _int iHP = CExcavatorGame::GetInstance()->Minus_HP();
    _float fRand = m_pGameInstance->GetInstance()->Compute_Random(0.0f, 4.f);
    _int iRand2 = round(fRand);
    if (iRand2 == 0)
        START_SFX_DELAY(TEXT("A_sfx_tank_fire_0"), 0.0f, g_SFXVolume * 1.0f, false);
    else if (iRand2 == 1)
        START_SFX_DELAY(TEXT("A_sfx_tank_fire_1"), 0.0f, g_SFXVolume * 1.0f, false);
    else if (iRand2 == 2)
        START_SFX_DELAY(TEXT("A_sfx_tank_projectile_explosion-0"), 0.1f, g_SFXVolume * 1.0f, false);
    else if (iRand2 == 3)
        START_SFX_DELAY(TEXT("A_sfx_tank_projectile_explosion-3"), 0.0f, g_SFXVolume * 1.0f, false);
    else if (iRand2 == 4)
        START_SFX_DELAY(TEXT("A_sfx_tank_projectile_explosion-2"), 0.1f, g_SFXVolume * 1.0f, false);

    if (iHP == 0)
    {
        static_cast<CModelObject*>(m_PartObjects[CENTRE_PART::CENTRE_REGULATOR])->Switch_Animation(0); // ±úÁø¸ðµ¨
    }
    _matrix matFX =  Get_ControllerTransform()->Get_WorldMatrix();
    matFX = XMMatrixScaling(1.0f, 1.0f, 1.0f) * XMMatrixTranslation(m_pGameInstance->Compute_Random(-150.f, 150.f), m_pGameInstance->Compute_Random(-170.f, -150.f), 0.0f) * matFX;
    _wstring strFXTag = L"Generic_Explosion";

    CEffect2D_Manager::GetInstance()->Play_Effect(strFXTag, Get_Include_Section_Name(), matFX); 

    for (_uint i = 0; i < CENTRE_LAST; ++i)
    {
        if (i == CENTRE_PART::CENTRE_BG || i == CENTRE_PART::CENTRE_TROOPER_L || i == CENTRE_PART::CENTRE_TROOPER_R)
        {

        }
        else
        {
            static_cast<CModelObject*>(m_PartObjects[i])->Start_HitRender();
        }
    }

}

void CExcavator_Centre::Start_Part_HitRender()
{
    for (_uint i = 0; i < CENTRE_LAST; ++i)
    {
        if (i == CENTRE_PART::CENTRE_BG || i == CENTRE_PART::CENTRE_TROOPER_L || i == CENTRE_PART::CENTRE_TROOPER_R)
        {

        }
        else
        {
            static_cast<CModelObject*>(m_PartObjects[i])->Start_HitRender();
        }
    }
}

void CExcavator_Centre::Render_DeadEffect()
{
    _matrix matFX = Get_ControllerTransform()->Get_WorldMatrix();
    matFX = XMMatrixScaling(1.4f, 1.4f, 1.4f) * XMMatrixTranslation(m_pGameInstance->Compute_Random(-600.f, 600.f), m_pGameInstance->Compute_Random(-220.f, -150.f), 0.0f) * matFX;
    _wstring strFXTag = L"Generic_Explosion";

    CEffect2D_Manager::GetInstance()->Play_Effect(strFXTag, Get_Include_Section_Name(), matFX);
}

CExcavator_Switch* CExcavator_Centre::Get_ActiveSwitch()
{
    for (_uint i = CENTRE_PART::CENTRE_SWITCH_0; i < CENTRE_PART::CENTRE_LAST; ++i)
    {
        if (true == m_PartObjects[i]->Is_Active())
        {
            return dynamic_cast<CExcavator_Switch*>(m_PartObjects[i]);
        }
    }
    return nullptr;
}

_int CExcavator_Centre::Get_NumActiveSwitches()
{
    _int iNumActiveSwitches = 0;
    for (_uint i = CENTRE_PART::CENTRE_SWITCH_0; i < CENTRE_PART::CENTRE_LAST; ++i)
    {
        if (true == m_PartObjects[i]->Is_Active())
        {
            ++iNumActiveSwitches;
        }
    }
    return iNumActiveSwitches;
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

HRESULT CExcavator_Centre::Ready_Components(CENTRE_DESC* _pDesc)
{

    m_p2DColliderComs.resize(1);

    CCollider_Circle::COLLIDER_CIRCLE_DESC Desc = {};
    Desc.pOwner = this;
    Desc.fRadius = 200.0f;
    Desc.vScale = { 1.0f, 1.0f };
    Desc.vOffsetPosition = { 0.0f, 0.0f };
    Desc.isBlock = false;
    Desc.isTrigger = true;
    Desc.iCollisionGroupID = OBJECT_GROUP::MONSTER;
    if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Circle"),
        TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_p2DColliderComs[0]), &Desc)))
        return E_FAIL;

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
        START_SFX_DELAY(TEXT("A_sfx_ShutterLift"), 0.0f, g_SFXVolume, false);
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
