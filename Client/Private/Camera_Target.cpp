#include "Camera_Target.h"
#include "stdafx.h"
#include "Camera_Target.h"
#include "GameInstance.h"

CCamera_Target::CCamera_Target(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CCamera(_pDevice, _pContext)
{
}

CCamera_Target::CCamera_Target(const CCamera_Target& _Prototype)
    : CCamera(_Prototype)
    , m_vArm(_Prototype.m_vArm)
    , m_vArmRotAxis(_Prototype.m_vArmRotAxis)
    , m_fArmAngle(_Prototype.m_fArmAngle)
    , m_fDistance(_Prototype.m_fDistance)
{

}

HRESULT CCamera_Target::Initialize_Prototype()
{


    return S_OK;
}

HRESULT CCamera_Target::Initialize(void* _pArg)
{
    CAMERA_TARGET_DESC* pDesc = static_cast<CAMERA_TARGET_DESC*>(_pArg);
    pDesc->eStartCoord = COORDINATE_3D;
    pDesc->isCoordChangeEnable = false;
    pDesc->tTransform3DDesc.fRotationPerSec = XMConvertToRadians(200.f);

    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    //CAMERA_TARGET_DESC* pDesc = static_cast<CAMERA_TARGET_DESC*>(_pArg);
    if (nullptr == pDesc->pTarget)
        return E_FAIL;

    m_pTarget = pDesc->pTarget; 
    Safe_AddRef(m_pTarget);
    m_vArmRotAxis = pDesc->vArmRotAxis;
    m_fArmAngle = pDesc->fArmAngle;
    m_fDistance = pDesc->fDistance;
    m_fMouseSensor = pDesc->fMouseSensor;
    m_vOffset = { 0.0f, 1.5f, 0.0f };
    MakeArm();
    

    return S_OK;
}

void CCamera_Target::Priority_Update(_float _fTimeDelta)
{
}

void CCamera_Target::Update(_float _fTimeDelta)
{
    Action_Shake(_fTimeDelta);
}

void CCamera_Target::Late_Update(_float _fTimeDelta)
{

    if (m_iAssassinate)
    {
        Action_Assassinate(_fTimeDelta);
    }
    else
    {
        if (nullptr == m_pSubTarget)
            MoveToTarget(_fTimeDelta);
        else
        {
            MoveToSubTarget(_fTimeDelta);
        }
        AutoRotaionCameraArm(_fTimeDelta);

        MouseMove(_fTimeDelta);
    }

    Action_Zoom(_fTimeDelta);

    __super::Compute_PipeLineMatrices();

}


HRESULT CCamera_Target::Change_Target(CGameObject* _pNewTarget)
{

    if (nullptr == _pNewTarget)
        return E_FAIL;

    if (_pNewTarget == m_pTarget)
        return S_OK;

    if (nullptr != m_pTarget)
    {
        Safe_Release(m_pTarget);
        m_pTarget = nullptr;
    }

    m_pTarget = _pNewTarget;
    Safe_AddRef(m_pTarget);

    return S_OK;
}

HRESULT CCamera_Target::Change_SubTarget(CGameObject* _pNewSubTarget)
{
    if (nullptr == _pNewSubTarget)
        return E_FAIL;

    if (_pNewSubTarget == m_pSubTarget)
        return S_OK;

    if (nullptr != m_pSubTarget)
    {
        Safe_Release(m_pSubTarget);
        m_pSubTarget = nullptr;
    }

    m_pSubTarget = _pNewSubTarget;
    Safe_AddRef(m_pSubTarget);

    return S_OK;
}

void CCamera_Target::MakeArm()
{
    static _vector vDefaultArmDir = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
    XMStoreFloat3(&m_vArm, XMVector3TransformNormal(vDefaultArmDir, XMMatrixRotationAxis(XMLoadFloat3(&m_vArmRotAxis), m_fArmAngle)));
}

void CCamera_Target::TurnArmY(_float _fSpeed)
{
    XMStoreFloat4x4(&m_ArmRotMatrixY, XMMatrixRotationAxis(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), _fSpeed));
    XMStoreFloat3(&m_vArm, XMVector3TransformNormal(XMLoadFloat3(&m_vArm), XMLoadFloat4x4(&m_ArmRotMatrixY)));
}

void CCamera_Target::TurnArmX(_float _fSpeed)
{
    _vector vRight = XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&m_vArm), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)));

    XMStoreFloat4x4(&m_ArmRotMatrixX, XMMatrixRotationAxis(vRight, _fSpeed));
    XMStoreFloat3(&m_vArm, XMVector3TransformNormal(XMLoadFloat3(&m_vArm), XMLoadFloat4x4(&m_ArmRotMatrixX)));
}

void CCamera_Target::RotationY(_float _fRadian)
{
    XMStoreFloat4x4(&m_ArmRotMatrixY, XMMatrixRotationAxis(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), _fRadian));
    XMStoreFloat3(&m_vArm, XMVector3TransformNormal(XMLoadFloat3(&m_vArm), XMLoadFloat4x4(&m_ArmRotMatrixY)));
}

void CCamera_Target::Set_TurnTargetRadian_Y(_float _fRadian, _float _fTurnSpeed)
{
    if (_fRadian < 0.0f)
        m_fTurnSpeedY = abs(_fTurnSpeed) * -1.0f;
    else
        m_fTurnSpeedY = abs(_fTurnSpeed);

    m_isTurnFinishedY = false;
    m_vTargetRadianY.x = abs(_fRadian);
    m_vTargetRadianY.y = 0.0f;
}

void CCamera_Target::Turn_TargetY(_float _fTimeDelta)
{
    if (true == m_isTurnFinishedY)
        return;

    m_vTargetRadianY.y += _fTimeDelta * abs(m_fTurnSpeedY);
    if (m_vTargetRadianY.x <= m_vTargetRadianY.y)
    {
        m_vTargetRadianY.y = 0.0f;
        m_isTurnFinishedY = true;

    }
    TurnArmY(m_fTurnSpeedY * _fTimeDelta);
}

void CCamera_Target::Set_TurnTargetRadian_X(_float _fRadian, _float _fTurnSpeed)
{
    if (_fRadian < 0.0f)
        m_fTurnSpeedX = abs(_fTurnSpeed) * -1.0f;
    else
        m_fTurnSpeedX = abs(_fTurnSpeed);

    m_isTurnFinishedX = false;
    m_vTargetRadianX.x = abs(_fRadian);
    m_vTargetRadianX.y = 0.0f;
}

void CCamera_Target::Turn_TargetX(_float _fTimeDelta)
{
    if (true == m_isTurnFinishedX)
        return;

    m_vTargetRadianX.y += _fTimeDelta * abs(m_fTurnSpeedX);
    if (m_vTargetRadianX.x <= m_vTargetRadianX.y)
    {
        m_vTargetRadianX.y = 0.0f;
        m_isTurnFinishedX = true;

    }
    TurnArmX(m_fTurnSpeedX * _fTimeDelta);
}

void CCamera_Target::MoveToTarget(_float _fTimeDelta)
{
    if (nullptr == m_pTarget)
        return;

    _vector vTargetPos = m_pTarget->Get_Position(); 
    _vector vEye = vTargetPos + (XMLoadFloat3(&m_vArm) * m_fDistance);

    m_pControllerTransform->Set_State(CTransform::STATE_POSITION, vEye);
    m_pControllerTransform->LookAt_3D(vTargetPos + XMLoadFloat3(&m_vOffset) + XMLoadFloat3(&m_vShakeMovement));/* Eye는 오프셋 안더한 값으로 적용하고, At만 위로 올림. */
}

void CCamera_Target::MoveToSubTarget(_float _fTimeDelta)
{
    if (nullptr == m_pTarget)
        return;
    if (nullptr == m_pSubTarget)
        return;
    
    _float  fEpsilon = 0.001f;
    _vector vTargetPos = m_pTarget->Get_Position();
    _vector vSubTargetPos = m_pSubTarget->Get_Position();
    _vector vTargetAt = XMVectorSetY(vSubTargetPos, 1.0f);
    _vector vEye = vTargetPos + (XMLoadFloat3(&m_vArm) * m_fDistance);

    if (true == m_isFirst)
    {
        XMStoreFloat3(&m_vTargetAt, vTargetAt);
        XMStoreFloat3(&m_vPrevAt, vTargetAt);
        XMStoreFloat3(&m_vCurAt, vTargetAt);
        m_isFirst = false;
    }

    _float fDiff = XMVectorGetX(XMVector3Length(vTargetAt - XMLoadFloat3(&m_vPrevAt)));
    if (fDiff >= fEpsilon)
    {
        XMStoreFloat3(&m_vTargetAt, vTargetAt);
        /* 만약 서브타겟 대상이 움직임이 있었다면, Speed 다시 계산. */
        Compute_AtSpeed();
    }
    _vector vMoveDir = XMLoadFloat3(&m_vTargetAt) - XMLoadFloat3(&m_vPrevAt);

    m_fAtTimeAcc += _fTimeDelta;
    if (m_fAtTime <= m_fAtTimeAcc)
    {
        m_vCurAt = m_vTargetAt;
        m_fAtTimeAcc = 0.0f;
    }
    else
    {
        vMoveDir = XMVector3Normalize(vMoveDir);
        XMStoreFloat3(&m_vCurAt, XMLoadFloat3(&m_vPrevAt) + vMoveDir * m_fAtSpeed * _fTimeDelta);
    }


    m_pControllerTransform->Set_State(CTransform::STATE_POSITION, vEye);

    m_pControllerTransform->LookAt_3D(XMVectorSetW(XMLoadFloat3(&m_vCurAt) + XMLoadFloat3(&m_vShakeMovement), 1.0f));

    m_vPrevAt = m_vCurAt;
}

void CCamera_Target::Release_SubTarget()
{
    Safe_Release(m_pSubTarget);
    m_pSubTarget = nullptr;
}

void CCamera_Target::AutoRotaionCameraArm(_float _fTimeDelta)
{
    if (nullptr == m_pSubTarget)
        return;

    /* 카메라 암을 돌릴 조건. */
/* 1. 플레이어가 Screen 좌표상의 x 범위를 벗어났을때, */
    _float fMinX = g_iWinSizeX * 0.4f;
    _float fMaxX = g_iWinSizeX * 0.6f;
    _float fMinY = g_iWinSizeY * 0.1f;
    _float fMaxY = g_iWinSizeY * 0.9f;

    /* 2. Target의 위치를 Screen좌표까지 투영. */
    _vector vTargetPos = m_pTarget->Get_Position();
    _matrix ViewMatrix = m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW);
    _matrix ProjMatrix = m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ);


   ;
    vTargetPos = XMVector3TransformCoord(vTargetPos, ViewMatrix * ProjMatrix);
    /* z 나누기 수행. */
    //vTargetPos /= XMVectorGetW(vTargetPos);




    /* 해상도에 맞게 변형.                                                                                          */
    _float fObjectScreenPosX = (XMVectorGetX(vTargetPos) + 1.0f) * 0.5f * g_iWinSizeX;
    _float fObjectScreenPosY = (1.0f - XMVectorGetY(vTargetPos)) * 0.5f * g_iWinSizeY;

    if (XMVectorGetX(vTargetPos) < -1.0f || XMVectorGetX(vTargetPos) > 1.0f ||
        XMVectorGetY(vTargetPos) < -1.0f || XMVectorGetY(vTargetPos) > 1.0f ||
        XMVectorGetZ(vTargetPos) < -1.0f || XMVectorGetZ(vTargetPos) > 1.0f)
    {
        TurnArmY(3.f * _fTimeDelta);
    }

    if (true == m_isTurnSubTargetDistance)
    {
        
        _vector vSubPos = m_pSubTarget->Get_Position();
        _vector vEyePos = Get_Position();
        _float fSubDistance = XMVectorGetX(XMVector3Length(XMVectorSetY(vSubPos, XMVectorGetY(vEyePos)) - vEyePos));
        if (fSubDistance >= m_fSubTargetStandardDistance)
        {
            m_isTurnSubTargetDistance = false;
        }
        else
        {
            /* 플레이어를 바라보는 방향벡터를 구한다. 그 후 외적하여 회전 방향을 결정. */
            _vector vTargetPos = m_pTarget->Get_Position();
            _vector vEyePos = Get_Position();
            _vector fTargetDir = XMVectorSetY(vTargetPos, XMVectorGetY(vEyePos)) - vEyePos;
            _vector fSubTargetDir = XMVectorSetY(vSubPos, XMVectorGetY(vEyePos)) - vEyePos;

            _float fCrossDirY = XMVectorGetY(XMVector3Cross(fTargetDir, fSubTargetDir));

            if (0 < fCrossDirY)
            {
                /* 서브타겟이 메인 타겟보다 우측 */
                TurnArmY(2.5f * _fTimeDelta);
            }
            else
            {
                TurnArmY(-2.5f * _fTimeDelta);
            }
        }
    }
    else if (fObjectScreenPosX < fMinX)
    {
        TurnArmY((fMinX - fObjectScreenPosX) * 0.007f * _fTimeDelta);
    }
    else if (fObjectScreenPosX > fMaxX)
    {
        TurnArmY((fMaxX - fObjectScreenPosX) * 0.007f * _fTimeDelta);
    }
    else if(false == m_isTurnSubTargetDistance)
    {      
        /* SubTarget과의 거리로 회전판단. */
        if (nullptr == m_pSubTarget)
            return;

        _vector vSubPos = m_pSubTarget->Get_Position();
        _vector vEyePos = Get_Position();
        _float fSubDistance = XMVectorGetX(XMVector3Length(XMVectorSetY(vSubPos, XMVectorGetY(vEyePos)) - vEyePos));
        if (m_fSubTargetMinDistance >= fSubDistance)
        {
            m_isTurnSubTargetDistance = true;
        }
    }

}

void CCamera_Target::MouseMove(_float _fTimeDelta)
{
    if (nullptr != m_pSubTarget)
        return;

    if (MOUSE_PRESSING(MOUSE_KEY::RB))
    {
        _long m_lMoveX = 0;
        _long m_lMoveY = 0;
        /* y축 회전은 서브 타겟이 존재한다면 제한한다. */
        if (m_lMoveX = MOUSE_MOVE(MOUSE_MOVE::X))
        {
            TurnArmY(m_lMoveX * _fTimeDelta *  m_fMouseSensor);
        }
        else if (m_lMoveY = MOUSE_MOVE(MOUSE_MOVE::Y))
        {
            TurnArmX(m_lMoveY * _fTimeDelta * m_fMouseSensor);
            //MakeArm();
        }

        
    }

}

void CCamera_Target::Action_Assassinate(_float _fTimeDelta)
{
    if (m_iAssassinate == 2 && true == m_isTurnFinishedY && true == m_isTurnFinishedX)
        m_iAssassinate = 0;

    Action_Zoom(_fTimeDelta);
    Turn_TargetY(_fTimeDelta);
    Turn_TargetX(_fTimeDelta);

    MoveToTarget(_fTimeDelta);
}

void CCamera_Target::Set_TargetOffset(const _float3& _vTargetOffset, _float _fTime)
{
    m_vTargetOffset = _vTargetOffset;
    m_vOffsetMoveTime.x = _fTime;
    m_vOffsetMoveTime.y = 0.0f;
    m_isOffsetMoveFinished = false;

    /* 이동 속도 계산 */
    _vector vSpeed = XMLoadFloat3(&m_vTargetOffset) - XMLoadFloat3(&m_vOffset);
    vSpeed /= m_vOffsetMoveTime.x;

    XMStoreFloat3(&m_vMoveSpeed, vSpeed);
}

void CCamera_Target::Action_OffsetMove(_float _fTimeDelta)
{
    if (true == m_isOffsetMoveFinished)
        return;

    m_vOffsetMoveTime.y += _fTimeDelta;
    if (m_vOffsetMoveTime.x <= m_vOffsetMoveTime.y)
    {
        m_isOffsetMoveFinished = true;
        m_vOffsetMoveTime.y = 0.0f;
    }
    else
    {
        XMStoreFloat3(&m_vOffset , XMLoadFloat3(&m_vOffset) + (XMLoadFloat3(&m_vMoveSpeed) * _fTimeDelta));
    }
}

void CCamera_Target::Start_Assassinate()
{
    m_iAssassinate = 1;
    Set_TurnTargetRadian_Y(XMConvertToRadians(45.f), 2.0f);
    Set_TurnTargetRadian_X(XMConvertToRadians(-10.f), 0.4f);
    _vector vLook = m_pControllerTransform->Get_State(CTransform::STATE_LOOK);
    vLook = XMVector3Normalize(XMVector3TransformNormal(vLook, XMMatrixRotationY(XMConvertToRadians(-45.f))));
    m_vOffset.x = vLook.m128_f32[0] * 0.5f;
    m_vOffset.z = vLook.m128_f32[2] * 0.5f;
    ZoomIn(0.5f, ZOOM_LEVEL::IN_LEVEL2);
}

void CCamera_Target::End_Assassinate()
{
    m_iAssassinate = 2;
    Set_TurnTargetRadian_Y(XMConvertToRadians(-45.f), 5.0f);
    Set_TurnTargetRadian_X(XMConvertToRadians(10.f), 1.0f);
    m_vOffset.x = 0.0f;
    m_vOffset.z = 0.0f;
    ZoomOut(0.1f, NORMAL);
}

void CCamera_Target::Compute_AtSpeed()
{
    m_fAtTimeAcc = 0.0f;

    _float fLength = XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_vTargetAt) - XMLoadFloat3(&m_vPrevAt)));
    m_fAtSpeed = fLength / m_fAtTime;
}

CCamera_Target* CCamera_Target::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CCamera_Target* pInstance = new CCamera_Target(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CCamera_Target");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CCamera_Target::Clone(void* _pArg)
{
    CCamera_Target* pInstance = new CCamera_Target(*this);

    // 단순히 복사만해서 주는 것이 아니라, Prototype을 통해 복사받은 후, 추가적으로 필요한 Initialize는 따로 수행한다.
    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CCamera_Free");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCamera_Target::Free()
{
    Safe_Release(m_pTarget);
    m_pTarget = nullptr;
    Safe_Release(m_pSubTarget);
    m_pSubTarget = nullptr;
    __super::Free();
}

HRESULT CCamera_Target::Cleanup_DeadReferences()
{
    if (nullptr == m_pTarget)
        return S_OK;

    if (nullptr == m_pSubTarget)
        return S_OK;

    if (m_pTarget->Is_Dead())
    {
        Safe_Release(m_pTarget);
        m_pTarget = nullptr;
    }

    if (m_pSubTarget->Is_Dead())
    {
        Safe_Release(m_pSubTarget);
        m_isTurnSubTargetDistance = false;
        m_pSubTarget = nullptr;
    }

    return S_OK;
}
