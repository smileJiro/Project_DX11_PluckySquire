#include "stdafx.h"
#include "Door_Yellow.h"
#include "GameInstance.h"
#include "Section_Manager.h"

#include "Pressure_Plate.h"

CDoor_Yellow::CDoor_Yellow(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CDoor_2D(_pDevice, _pContext) 
{
}

CDoor_Yellow::CDoor_Yellow(const CDoor_Yellow& _Prototype)
    : CDoor_2D(_Prototype)
{
}

HRESULT CDoor_Yellow::Initialize(void* _pArg)
{
    if (nullptr == _pArg)
        return E_FAIL;

    DOOR_YELLOW_DESC* pDesc = static_cast<DOOR_YELLOW_DESC*>(_pArg);

    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    if (FAILED(Ready_Part(pDesc)))
        return E_FAIL;

    Set_AnimLoop();
    Switch_Animation_By_State();

    Register_OnAnimEndCallBack(bind(&CDoor_Yellow::On_AnimEnd, this, placeholders::_1, placeholders::_2));

    if (FAILED(CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(pDesc->strSectionTag, m_pPressurePlate)))
        return E_FAIL;

    return S_OK;
}

void CDoor_Yellow::Update(_float _fTimeDelta)
{
    if (nullptr != m_pPressurePlate)
    {
        m_pPressurePlate->Update(_fTimeDelta);

        // 문 열려있음 & 감압판 눌려있음 -> OPEN!
        if (m_pPressurePlate->Is_Down())
        {
            if (CLOSED == m_eDoorState)
            {
                m_eDoorState = OPEN;
                Set_ReverseAnimation(false);
                Switch_Animation_By_State();
            }

            if (CLOSE == m_eDoorState)
            {
                m_eDoorState = OPEN;
                if (false == m_bReverseAnimation)
                    Set_ReverseAnimation(true);
                else
                    Set_ReverseAnimation(false);
            }

        }   

        // 감압판 눌려있지 않음 -> Close
        else if (false == m_pPressurePlate->Is_Down())
        {
            if (OPENED == m_eDoorState)
            {
                m_eDoorState = CLOSE;
                Set_ReverseAnimation(false);
                Switch_Animation_By_State();

                m_p2DColliderComs[0]->Set_Active(true);

            }
            else if (OPEN == m_eDoorState)
            {
                m_eDoorState = CLOSE;
                if (false == m_bReverseAnimation)
                    Set_ReverseAnimation(true);
                else
                    Set_ReverseAnimation(false);

                m_p2DColliderComs[0]->Set_Active(true);
            }

        }

    }

    
    __super::Update(_fTimeDelta);    
}

void CDoor_Yellow::Late_Update(_float _fTimeDelta)
{
    if (nullptr != m_pPressurePlate)
        m_pPressurePlate->Late_Update(_fTimeDelta);

    __super::Late_Update(_fTimeDelta);
}

void CDoor_Yellow::On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{
    if (OPEN == m_eDoorState)
    {
        m_eDoorState = OPENED;
        m_p2DColliderComs[0]->Set_Active(false);
        m_bReverseAnimation = false;

        Switch_Animation_By_State();
    }

    else if (CLOSE == m_eDoorState)
    {
        m_eDoorState = CLOSED;
        Switch_Animation_By_State();

        if (false == m_bReverseAnimation)
        {
            Switch_Animation_By_State();
        }
        else
        {

        }
    }
}

HRESULT CDoor_Yellow::Ready_Part(const DOOR_YELLOW_DESC* _pDesc)
{
    CModelObject::MODELOBJECT_DESC Desc = {};

    Desc.tTransform2DDesc.vInitialPosition = _pDesc->vPressurePlatePos;
    Desc.iCurLevelID = m_iCurLevelID;
    
    m_pPressurePlate = dynamic_cast<CPressure_Plate*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, 
        TEXT("Prototype_GameObject_Pressure_Plate"), &Desc));

    if (nullptr == m_pPressurePlate)
        return E_FAIL;

    return S_OK;
}

void CDoor_Yellow::Set_AnimLoop()
{
    if (m_isHorizontal)
    {
        if (LARGE == m_eDoorSize)
        {
            Set_AnimationLoop(COORDINATE::COORDINATE_2D, H_LARGE_YELLOW_CLOSE, false);
            Set_AnimationLoop(COORDINATE::COORDINATE_2D, H_LARGE_YELLOW_CLOSED, true);
            Set_AnimationLoop(COORDINATE::COORDINATE_2D, H_LARGE_YELLOW_OPEN, false);
            Set_AnimationLoop(COORDINATE::COORDINATE_2D, H_LARGE_YELLOW_OPENED, true);
        }
        else if (MED == m_eDoorSize)
        {
            Set_AnimationLoop(COORDINATE::COORDINATE_2D, H_MED_YELLOW_CLOSE, false);
            Set_AnimationLoop(COORDINATE::COORDINATE_2D, H_MED_YELLOW_CLOSED, true);
            Set_AnimationLoop(COORDINATE::COORDINATE_2D, H_MED_YELLOW_OPEN, false);
            Set_AnimationLoop(COORDINATE::COORDINATE_2D, H_MED_YELLOW_OPENED, true);
        }
        else
        {
            Set_AnimationLoop(COORDINATE::COORDINATE_2D, H_SMALL_YELLOW_CLOSE, false);
            Set_AnimationLoop(COORDINATE::COORDINATE_2D, H_SMALL_YELLOW_CLOSED, true);
            Set_AnimationLoop(COORDINATE::COORDINATE_2D, H_SMALL_YELLOW_OPEN, false);
            Set_AnimationLoop(COORDINATE::COORDINATE_2D, H_SMALL_YELLOW_OPENED, true);
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
            Set_AnimationLoop(COORDINATE::COORDINATE_2D,V_SMALL_YELLOW_CLOSE, false);
            Set_AnimationLoop(COORDINATE::COORDINATE_2D,V_SMALL_YELLOW_CLOSED, true);
            Set_AnimationLoop(COORDINATE::COORDINATE_2D,V_SMALL_YELLOW_OPEN, false);
            Set_AnimationLoop(COORDINATE::COORDINATE_2D,V_SMALL_YELLOW_OPENED, true);
        }
    }

}


void CDoor_Yellow::Switch_Animation_By_State()
{
    _uint iAnimIndex = 0;
    if (m_isHorizontal)
    {
        if (LARGE == m_eDoorSize)
        {
            iAnimIndex = H_LARGE_YELLOW_CLOSE;
        }
        else if (MED == m_eDoorSize)
        {
            iAnimIndex = H_MED_YELLOW_CLOSE;
        }
        else
        {
            iAnimIndex = H_SMALL_YELLOW_CLOSE;
        }
    }
    else
    {
        if (LARGE == m_eDoorSize)
        {
            return;
        }
        else if (MED == m_eDoorSize)
        {
            return;
        }
        else
        {
            iAnimIndex = V_SMALL_YELLOW_CLOSE;
        }
    }

    switch (m_eDoorState)
    {
    case OPEN:
        iAnimIndex += 2;
        break;
    case OPENED:
        iAnimIndex += 3;
        break;
    case CLOSE:
        break;
    case CLOSED:
        iAnimIndex += 1;
        break;
    }

    Switch_Animation(iAnimIndex);

}

CDoor_Yellow* CDoor_Yellow::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CDoor_Yellow* pInstance = new CDoor_Yellow(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CDoor_Yellow");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CDoor_Yellow::Clone(void* _pArg)
{
    CDoor_Yellow* pInstance = new CDoor_Yellow(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CDoor_Yellow");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CDoor_Yellow::Free()
{
    Safe_Release(m_pPressurePlate);

    __super::Free();
}
