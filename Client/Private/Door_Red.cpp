#include "stdafx.h"
#include "Door_Red.h"
#include "GameInstance.h"
#include "Section_Manager.h"
#include "Camera_Manager.h"
#include "PlayerData_Manager.h"
#include "Player.h"

CDoor_Red::CDoor_Red(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CDoor_2D(_pDevice, _pContext)
{
}

CDoor_Red::CDoor_Red(const CDoor_Red& _Prototype)
    : CDoor_2D(_Prototype)
{
}

HRESULT CDoor_Red::Initialize(void* _pArg)
{
    if (nullptr == _pArg)
        return E_FAIL;

    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    DOOR_RED_DESC* pDesc = static_cast<DOOR_RED_DESC*>(_pArg);
    m_strLayerTag = pDesc->strLayerTag;
    m_fTargetDiff = pDesc->fTargetDiff;
    m_isCountLayer = pDesc->isCountLayer;

    Set_AnimLoop();
    Switch_Animation_By_State();

    Register_OnAnimEndCallBack(bind(&CDoor_Red::On_AnimEnd, this, placeholders::_1, placeholders::_2));

    return S_OK;
}

void CDoor_Red::Update(_float _fTimeDelta)
{
//#ifdef _DEBUG
    if (KEY_PRESSING(KEY::CTRL))
    {
        if (KEY_DOWN(KEY::NUM0))
            Set_OpeningDoor();
        else if (KEY_DOWN(KEY::NUM1))
            Set_ClosingDoor();
    }
//#endif

    if (m_isCountLayer)
    {
        // 문이 닫혀있을 때 판정.
        if (CLOSED == m_eDoorState)
        {
            _bool isEmpty = true;
            if (false == m_pGameInstance->Is_EmptyLayer(m_iCurLevelID, m_strLayerTag))
                isEmpty = false;

            // 닫혀있을 경우, 몬스터가 다 죽었다 -> 타겟카메라를 나러 해줘!
            if (isEmpty && (false == m_isStartOpen))
            {
                // Target 설정.
                if (0.f < m_fTargetDiff)
                {
                    CCamera_Manager::GetInstance()->Change_CameraTarget(this);
                    m_isStartOpen = true;

                    //CPlayable* pPlayer = CPlayerData_Manager::GetInstance()->Get_CurrentPlayer_Ptr();
                    //if (nullptr != pPlayer)
                    //    pPlayer->Set_BlockPlayerInput(true);
                }
                else
                {
                    m_eDoorState = OPEN;
                    Switch_Animation_By_State();
                }
            }

            // 타겟카메라가 나로 되고 있는데.. 카메라의 Target Position이랑 차이가 적게 나면 진짜로 OPEN!
            if (m_isStartOpen)
            {
                _vector v3DPosition = SECTION_MGR->Get_WorldPosition_FromWorldPosMap(m_strSectionName,
                    _float2(XMVectorGetX(m_pControllerTransform->Get_State(CTransform::STATE_POSITION)), XMVectorGetY(m_pControllerTransform->Get_State(CTransform::STATE_POSITION))));

                _vector vCamPosition = CCamera_Manager::GetInstance()->Get_CameraVector(CTransform::STATE_POSITION);

                // 진짜 OPEN 시작.
                if (XMVectorGetX(XMVector3Length(v3DPosition - vCamPosition)) < m_fTargetDiff)
                {
                    m_eDoorState = OPEN;

                    Switch_Animation_By_State();
                }
            }
        }
    }  
   
    
    __super::Update(_fTimeDelta);
}

void CDoor_Red::Set_OpeningDoor()
{
    m_eDoorState = OPEN;
    Switch_Animation_By_State();
}

void CDoor_Red::Set_ClosingDoor()
{
    m_eDoorState = CLOSE; 
    Switch_Animation_By_State();

    if (0 < m_p2DColliderComs.size() && nullptr != m_p2DColliderComs[0])
        m_p2DColliderComs[0]->Set_Active(true);
}


void CDoor_Red::On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{
    if (OPEN == m_eDoorState)
    {
        m_eDoorState = OPENED;
        m_p2DColliderComs[0]->Set_Active(false);
        Switch_Animation_By_State();

        if (0.f < m_fTargetDiff)
        {
            CCamera_Manager::GetInstance()->Change_CameraTarget(CPlayerData_Manager::GetInstance()->Get_NormalPlayer_Ptr());

            //CPlayable* pPlayer = CPlayerData_Manager::GetInstance()->Get_CurrentPlayer_Ptr();
            //if (nullptr != pPlayer)
            //    pPlayer->Set_BlockPlayerInput(false);
        }
    }

}

void CDoor_Red::Set_AnimLoop()
{
    if (m_isHorizontal)
    {
        if (LARGE == m_eDoorSize)
        {
            Set_AnimationLoop(COORDINATE::COORDINATE_2D, H_LARGE_RED_CLOSE, false);
            Set_AnimationLoop(COORDINATE::COORDINATE_2D, H_LARGE_RED_CLOSED, true);
            Set_AnimationLoop(COORDINATE::COORDINATE_2D, H_LARGE_RED_OPEN, false);
            Set_AnimationLoop(COORDINATE::COORDINATE_2D, H_LARGE_RED_OPENED, true);
        }
        else if (MED == m_eDoorSize)
        {
            Set_AnimationLoop(COORDINATE::COORDINATE_2D, H_MED_RED_CLOSE, false);
            Set_AnimationLoop(COORDINATE::COORDINATE_2D, H_MED_RED_CLOSED, true);
            Set_AnimationLoop(COORDINATE::COORDINATE_2D, H_MED_RED_OPEN, false);
            Set_AnimationLoop(COORDINATE::COORDINATE_2D, H_MED_RED_OPENED, true);
        }
        else
        {
            Set_AnimationLoop(COORDINATE::COORDINATE_2D, H_SMALL_RED_CLOSE, false);
            Set_AnimationLoop(COORDINATE::COORDINATE_2D, H_SMALL_RED_CLOSED, true);
            Set_AnimationLoop(COORDINATE::COORDINATE_2D, H_SMALL_RED_OPEN, false);
            Set_AnimationLoop(COORDINATE::COORDINATE_2D, H_SMALL_RED_OPENED, true);
        }
    }
    else
    {
        if (LARGE == m_eDoorSize)
        {
            Set_AnimationLoop(COORDINATE::COORDINATE_2D, V_LARGE_RED_CLOSE, false);
            Set_AnimationLoop(COORDINATE::COORDINATE_2D, V_LARGE_RED_CLOSED, true);
            Set_AnimationLoop(COORDINATE::COORDINATE_2D, V_LARGE_RED_OPEN, false);
            Set_AnimationLoop(COORDINATE::COORDINATE_2D, V_LARGE_RED_OPENED, true);
        }
        else if (MED == m_eDoorSize)
        {
            Set_AnimationLoop(COORDINATE::COORDINATE_2D, V_MED_RED_CLOSE, false);
            Set_AnimationLoop(COORDINATE::COORDINATE_2D, V_MED_RED_CLOSED, true);
            Set_AnimationLoop(COORDINATE::COORDINATE_2D, V_MED_RED_OPEN, false);
            Set_AnimationLoop(COORDINATE::COORDINATE_2D, V_MED_RED_OPENED, true);
        }
        else
        {
            Set_AnimationLoop(COORDINATE::COORDINATE_2D, V_SMALL_RED_CLOSE, false);
            Set_AnimationLoop(COORDINATE::COORDINATE_2D, V_SMALL_RED_CLOSED, true);
            Set_AnimationLoop(COORDINATE::COORDINATE_2D, V_SMALL_RED_OPEN, false);
            Set_AnimationLoop(COORDINATE::COORDINATE_2D, V_SMALL_RED_OPENED, true);
        }
    }
}

void CDoor_Red::Switch_Animation_By_State()
{
    _uint iAnimIndex = 0;
    if (m_isHorizontal)
    {
        if (LARGE == m_eDoorSize)
        {
            iAnimIndex = H_LARGE_RED_CLOSE;
        }
        else if (MED == m_eDoorSize)
        {
            iAnimIndex = H_MED_RED_CLOSE;
        }
        else
        {
            iAnimIndex = H_SMALL_RED_CLOSE;
        }
    }
    else
    {
        if (LARGE == m_eDoorSize)
        {
            iAnimIndex = V_LARGE_RED_CLOSE;
        }
        else if (MED == m_eDoorSize)
        {
            iAnimIndex = V_MED_RED_CLOSE;
        }
        else
        {
            iAnimIndex = V_SMALL_RED_CLOSE;
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
    case CLOSED:
        iAnimIndex += 1;
        break;
    case CLOSE:
        break;
    }

    Switch_Animation(iAnimIndex);
}

CDoor_Red* CDoor_Red::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CDoor_Red* pInstance = new CDoor_Red(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CDoor_Red");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CDoor_Red::Clone(void* _pArg)
{
    CDoor_Red* pInstance = new CDoor_Red(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CDoor_Red");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CDoor_Red::Free()
{
    __super::Free();
}