#include "stdafx.h"
#include "Door_Blue.h"
#include "GameInstance.h"
#include "Section_Manager.h"
#include "Key.h"

CDoor_Blue::CDoor_Blue(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CDoor_2D(_pDevice, _pContext)
{
}

CDoor_Blue::CDoor_Blue(const CDoor_Blue& _Prototype)
	: CDoor_2D(_Prototype)
{
}

HRESULT CDoor_Blue::Initialize(void* _pArg)
{
    if (nullptr == _pArg)
        return E_FAIL;

    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    Set_AnimLoop();
    Switch_Animation_By_State();

    Register_OnAnimEndCallBack(bind(&CDoor_Blue::On_AnimEnd, this, placeholders::_1, placeholders::_2));

    return S_OK;
}

void CDoor_Blue::Update(_float _fTimeDelta)
{
    __super::Update(_fTimeDelta);

    if (OPEN == m_eDoorState)
    {
        _float fProgress = m_pControllerModel->Get_Model(COORDINATE_2D)->Get_CurrentAnimProgeress();

        if (fProgress > 0.9f && m_pGameInstance->Is_SFXPlaying(L"A_sfx_Gate_loop"))
        {
            m_pGameInstance->End_SFX(TEXT("A_sfx_Gate_loop"));
            m_pGameInstance->Start_SFX(TEXT("A_sfx_Gate_FinishedMoving"), g_SFXVolume);
        }
    }
}


void CDoor_Blue::On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{
    if (OPEN == m_eDoorState)
    {
        m_eDoorState = OPENED;
        m_p2DColliderComs[0]->Set_Active(false);
        Switch_Animation_By_State();
    }
}

void CDoor_Blue::On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
    OBJECT_GROUP eGroup = (OBJECT_GROUP)_pOtherCollider->Get_CollisionGroupID();

    // ¹® ¿­¸².
    if (OBJECT_GROUP::GIMMICK_OBJECT == eGroup)
    {
        CKey* pKey = dynamic_cast<CKey*>(_pOtherObject);

        if (nullptr == pKey)
            return;
        else
        {
            m_eDoorState = OPEN;
            Switch_Animation_By_State();

            m_pGameInstance->Start_SFX_Delay(TEXT("A_sfx_gate_start"), 1.f, g_SFXVolume);
            m_pGameInstance->Start_SFX_Delay(TEXT("A_sfx_Gate_loop"), 1.f, g_SFXVolume, true);


        }
    }

}


void CDoor_Blue::Set_AnimLoop()
{
    if (m_isHorizontal)
    {
        if (LARGE == m_eDoorSize)
        {
            Set_AnimationLoop(COORDINATE::COORDINATE_2D, H_LARGE_BLUE_CLOSED, false);
            Set_AnimationLoop(COORDINATE::COORDINATE_2D, H_LARGE_BLUE_OPEN, false);
            Set_AnimationLoop(COORDINATE::COORDINATE_2D, H_LARGE_BLUE_OPENED, true);
        }
        else if (MED == m_eDoorSize)
        {
            Set_AnimationLoop(COORDINATE::COORDINATE_2D, H_MED_BLUE_CLOSED, false);
            Set_AnimationLoop(COORDINATE::COORDINATE_2D, H_MED_BLUE_OPEN, false);
            Set_AnimationLoop(COORDINATE::COORDINATE_2D, H_MED_BLUE_OPENED, true);
        }
        else
        {
            Set_AnimationLoop(COORDINATE::COORDINATE_2D, H_SMALL_BLUE_CLOSED, false);
            Set_AnimationLoop(COORDINATE::COORDINATE_2D, H_SMALL_BLUE_OPEN, false);
            Set_AnimationLoop(COORDINATE::COORDINATE_2D, H_SMALL_BLUE_OPENED, true);
        }
    }
    else
    {
        if (LARGE == m_eDoorSize)
        {
        }
        else if (MED == m_eDoorSize)
        {
        }
        else
        {
            Set_AnimationLoop(COORDINATE::COORDINATE_2D, V_SMALL_YELLOW_CLOSE, false);
            Set_AnimationLoop(COORDINATE::COORDINATE_2D, V_SMALL_YELLOW_CLOSED, true);
            Set_AnimationLoop(COORDINATE::COORDINATE_2D, V_SMALL_YELLOW_OPEN, false);
            Set_AnimationLoop(COORDINATE::COORDINATE_2D, V_SMALL_YELLOW_OPENED, true);
        }
    }
}

void CDoor_Blue::Switch_Animation_By_State()
{
    _uint iAnimIndex = 0;
    if (m_isHorizontal)
    {
        if (LARGE == m_eDoorSize)
        {
            iAnimIndex = H_LARGE_BLUE_CLOSED;
        }
        else if (MED == m_eDoorSize)
        {
            iAnimIndex = H_MED_BLUE_CLOSED;
        }
        else
        {
            iAnimIndex = H_SMALL_BLUE_CLOSED;
        }
    }
    else
    {
        return;
    }

    switch (m_eDoorState)
    {
    case OPEN:
        iAnimIndex += 1;
        break;
    case OPENED:
        iAnimIndex += 2;
        break;
    case CLOSED:
        iAnimIndex += 0;
        break;
    }

    Switch_Animation(iAnimIndex);
}

CDoor_Blue* CDoor_Blue::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CDoor_Blue* pInstance = new CDoor_Blue(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CDoor_Blue");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CDoor_Blue::Clone(void* _pArg)
{
    CDoor_Blue* pInstance = new CDoor_Blue(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CDoor_Blue");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CDoor_Blue::Free()
{
    __super::Free();
}
