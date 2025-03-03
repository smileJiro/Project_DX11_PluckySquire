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

void CDoor_Blue::Open_Door()
{
    m_eDoorState = OPEN;
    Switch_Animation_By_State();
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
