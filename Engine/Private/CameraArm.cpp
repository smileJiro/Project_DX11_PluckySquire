#include "CameraArm.h"
#include "GameInstance.h"

CCameraArm::CCameraArm(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : m_pDevice(_pDevice)
    , m_pContext(_pContext)
    , m_pGameInstance(CGameInstance::GetInstance())
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
    Safe_AddRef(m_pGameInstance);
}

CCameraArm::CCameraArm(const CCameraArm& Prototype)
    : m_pDevice(Prototype.m_pDevice)
    , m_pContext(Prototype.m_pContext)
    , m_pGameInstance(Prototype.m_pGameInstance)
    , m_vArm(Prototype.m_vArm)
    , m_vRotation(Prototype.m_vRotation)
    , m_fLength(Prototype.m_fLength)
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
    Safe_AddRef(m_pGameInstance);
}

HRESULT CCameraArm::Initialize(void* pArg)
{
    CAMERA_ARM_DESC* pDesc = static_cast<CAMERA_ARM_DESC*>(pArg);

    m_vArm = pDesc->vArm;
    m_vRotation = pDesc->vRotation;
    m_fLength = pDesc->fLength;

    m_pTransform = CTransform_3D::Create(m_pDevice, m_pContext);
    if (nullptr == m_pTransform)
        return E_FAIL;

    if (FAILED(m_pTransform->Initialize(pDesc)))
        return E_FAIL;

    m_pTransform->Set_Look(XMLoadFloat3(&m_vArm));
    //Set_ArmVector(XMLoadFloat3(&m_vArm));

    return S_OK;
}

HRESULT CCameraArm::Initialize_Clone()
{
    return S_OK;
}

void CCameraArm::Priority_Update(_float fTimeDelta)
{
}

void CCameraArm::Update(_float fTimeDelta)
{
}

void CCameraArm::Late_Update(_float fTimeDelta)
{
}

_bool CCameraArm::Get_IsInPreArmData(_int _iTriggerID, pair<RETURN_ARMDATA, _bool>* _pPreArmData)
{
    for (auto& PreArm : m_PreArms) {
        if (_iTriggerID == PreArm.first.iTriggerID) {
            _pPreArmData = &PreArm;
            return true;
        }
    }

    return false;
}

void CCameraArm::Set_NextArmData(ARM_DATA* _pData, _int _iTriggerID)
{
    // 초기화
    if (nullptr != m_pNextArmData) {
        m_pNextArmData->fMoveTimeAxisY.y = 0.f;
        m_pNextArmData->fMoveTimeAxisRight.y = 0.f;
        m_pNextArmData->fLengthTime.y = 0.f;
    }
    m_iMovementFlags = RESET_FLAG;

    m_pNextArmData = _pData;
    m_fStartLength = m_fLength;
    m_vStartArm = m_vArm;
    m_fReturnTime.y = 0.f;

    for (auto& PreArm : m_PreArms) {
        if (_iTriggerID == PreArm.first.iTriggerID) {
            if (true == PreArm.second) {    // Return 중이라면?
                PreArm.second = false;
                m_fReturnTime.y = 0.f;
                
                return;
            }

            return;
        }
    }

    RETURN_ARMDATA tData;

    tData.vPreArm = m_vArm;
    tData.fPreLength = m_fLength;
    tData.iTriggerID = _iTriggerID;

    m_PreArms.push_back(make_pair(tData, false));
}

void CCameraArm::Set_PreArmDataState(_int _iTriggerID, _bool _isReturn)
{
    if (true == _isReturn) {
        for (auto iterator = m_PreArms.rbegin(); iterator != m_PreArms.rend(); ++iterator) {
            // Trigger ID와 Exit 조건 일치 확인
            if (iterator->first.iTriggerID == _iTriggerID) {
                
                m_vStartArm = m_vArm;
                m_fStartLength = m_fLength;
                m_iCurTriggerID = _iTriggerID;
                iterator->second = true;        // Return 중이란 의미

                m_fReturnTime.y = 0.f;
                return;
            }
        }

    }
    else {
        for (auto iterator = m_PreArms.rbegin(); iterator != m_PreArms.rend(); ++iterator) {
            // Trigger ID와 Exit 조건 일치 확인
            if (iterator->first.iTriggerID == _iTriggerID) {

                // deque에서 해당 상태 제거
                //m_PreArms.erase((iterator + 1).base());
                return;
            }
        }
    }
}

void CCameraArm::Set_Rotation(_vector _vRotation)
{
    XMStoreFloat3(&m_vRotation, _vRotation);

    _vector vCrossX = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMLoadFloat3(&m_vArm));
    m_pTransform->TurnAngle(m_vRotation.x, vCrossX);
    m_pTransform->TurnAngle(m_vRotation.y);

    XMStoreFloat3(&m_vArm, m_pTransform->Get_State(CTransform::STATE_LOOK));
}

void CCameraArm::Set_ArmVector(_vector _vArm)
{
    XMStoreFloat3(&m_vArm, XMVector3Normalize(_vArm));

    _vector vLook = XMLoadFloat3(&m_vArm);
    _vector vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
    _vector vUp = XMVector3Cross(vLook, vRight);

    m_pTransform->Set_State(CTransform::STATE_RIGHT, XMVector3Normalize(vRight));
    m_pTransform->Set_State(CTransform::STATE_UP, XMVector3Normalize(vUp));
    m_pTransform->Set_State(CTransform::STATE_LOOK, XMVector3Normalize(vLook));
}

void CCameraArm::Set_StartInfo()
{
    m_vStartArm = m_vArm;
    m_fStartLength = m_fLength;
}

_bool CCameraArm::Move_To_NextArm_ByVector(_float _fTimeDelta, _bool _isBook)
{
    if (nullptr == m_pNextArmData)
        return false;
    _vector vDot = XMVector3Dot(XMLoadFloat3(&m_vArm), XMLoadFloat3(&m_pNextArmData->vDesireArm));
    _float fAngle = acos(XMVectorGetX(vDot));
    _float fDegree = XMConvertToDegrees(fAngle);

    // 일단 Y축 회전 시간, EASE_IN으로 설정해서 하기
    if (!(m_iMovementFlags & DONE_Y_ROTATE)) {
        _float fRatio = m_pGameInstance->Calculate_Ratio(&m_pNextArmData->fMoveTimeAxisY, _fTimeDelta, LERP);

        if (fRatio >= (1.f - EPSILON)) {

            m_vArm = m_pNextArmData->vDesireArm;
            m_pTransform->Set_Look(XMLoadFloat3(&m_vArm));
            m_pNextArmData->fMoveTimeAxisY.y = 0.f;
            m_iMovementFlags |= DONE_Y_ROTATE;
        }
        else {

            if (true == XMVector3Equal(XMLoadFloat3(&m_vArm), XMLoadFloat3(&m_pNextArmData->vDesireArm))) {
                m_vArm = m_pNextArmData->vDesireArm;
                m_pTransform->Set_Look(XMLoadFloat3(&m_vArm));
                m_pNextArmData->fMoveTimeAxisY.y = 0.f;
                m_iMovementFlags |= DONE_Y_ROTATE;
            }

            _vector vArm = XMVector3Normalize(XMVectorLerp(XMLoadFloat3(&m_vStartArm), XMLoadFloat3(&m_pNextArmData->vDesireArm), fRatio));

            XMStoreFloat3(&m_vArm, vArm);
            m_pTransform->Set_Look(vArm);
        }
    }

    // Length 이동
    if (!(m_iMovementFlags & DONE_LENGTH_MOVE)) {
        
        _float fRatio = m_pGameInstance->Calculate_Ratio(&m_pNextArmData->fLengthTime, _fTimeDelta, m_pNextArmData->iLengthRatioType);

        if (m_pNextArmData->fLengthTime.y >= m_pNextArmData->fLengthTime.x) {
            m_pNextArmData->fLengthTime.y = 0.f;
            m_fLength = m_pNextArmData->fLength;
            m_iMovementFlags |= DONE_LENGTH_MOVE;
        }
        else {

            if (abs(m_fLength - m_pNextArmData->fLength) < EPSILON) {
                m_pNextArmData->fLengthTime.y = 0.f;
                m_fLength = m_pNextArmData->fLength;
                m_iMovementFlags |= DONE_LENGTH_MOVE;
            }
            
            m_fLength = m_pGameInstance->Lerp(m_fStartLength, m_pNextArmData->fLength, fRatio);
        }
    }

    if ((ALL_DONE_MOVEMENT_VECTOR == (m_iMovementFlags & ALL_DONE_MOVEMENT_VECTOR))) {
        m_iMovementFlags = RESET_FLAG;
        m_pNextArmData->fMoveTimeAxisY.y = 0.f;
        m_pNextArmData->fLengthTime.y = 0.f;

        return true;
    }

    return false;
}

_bool CCameraArm::Move_To_CustomArm(ARM_DATA* _pCustomData, _float _fTimeDelta, _bool _isUsingVector)
{
    // Y축 회전
    if (false == _isUsingVector) {
        if (!(m_iMovementFlags & DONE_Y_ROTATE)) { // 안 끝났을 때

            _pCustomData->fMoveTimeAxisY.y += _fTimeDelta;
            _float fRatio = _pCustomData->fMoveTimeAxisY.y / _pCustomData->fMoveTimeAxisY.x;

            if (_pCustomData->fMoveTimeAxisY.y >= _pCustomData->fMoveTimeAxisY.x) {
                _pCustomData->fMoveTimeAxisY.y = 0.f;
                m_iMovementFlags |= DONE_Y_ROTATE;
            }
            else {
                _float fRotationPerSec = m_pGameInstance->Lerp(_pCustomData->fRotationPerSecAxisY.x, _pCustomData->fRotationPerSecAxisY.y, fRatio);
                m_pTransform->Set_RotationPerSec(fRotationPerSec);

                m_pTransform->Turn(_fTimeDelta, XMVectorSet(0.0f, 1.f, 0.f, 0.f));

                _vector vLook = m_pTransform->Get_State(CTransform::STATE_LOOK);
                XMStoreFloat3(&m_vArm, vLook);
            }
        }

        // Right 축 회전
        if (!(m_iMovementFlags & DONE_RIGHT_ROTATE)) {

            _pCustomData->fMoveTimeAxisRight.y += _fTimeDelta;
            _float fRatio = _pCustomData->fMoveTimeAxisRight.y / _pCustomData->fMoveTimeAxisRight.x;

            if (_pCustomData->fMoveTimeAxisRight.y >= _pCustomData->fMoveTimeAxisRight.x) {
                _pCustomData->fMoveTimeAxisRight.y = 0.f;
                m_iMovementFlags |= DONE_RIGHT_ROTATE;
            }
            else {
                _float fRotationPerSec = m_pGameInstance->Lerp(_pCustomData->fRotationPerSecAxisRight.x, _pCustomData->fRotationPerSecAxisRight.y, fRatio);
                m_pTransform->Set_RotationPerSec(fRotationPerSec);

                _vector vCross = XMVector3Cross(XMLoadFloat3(&m_vArm), XMVectorSet(0.f, 1.f, 0.f, 0.f));

                m_pTransform->Turn(_fTimeDelta, vCross);

                _vector vLook = m_pTransform->Get_State(CTransform::STATE_LOOK);
                XMStoreFloat3(&m_vArm, vLook);
            }
        }
    }
    else {
        _float fRatio = m_pGameInstance->Calculate_Ratio(&m_pNextArmData->fMoveTimeAxisY, _fTimeDelta, LERP);

        if (fRatio >= (1.f - EPSILON)) {

            m_vArm = m_pNextArmData->vDesireArm;
            m_pTransform->Set_Look(XMLoadFloat3(&m_vArm));
            m_pNextArmData->fMoveTimeAxisY.y = 0.f;
            m_iMovementFlags |= DONE_Y_ROTATE;
        }
        else {

            if (true == XMVector3Equal(XMLoadFloat3(&m_vArm), XMLoadFloat3(&m_pNextArmData->vDesireArm))) {
                m_vArm = m_pNextArmData->vDesireArm;
                m_pTransform->Set_Look(XMLoadFloat3(&m_vArm));
                m_pNextArmData->fMoveTimeAxisY.y = 0.f;
                m_iMovementFlags |= DONE_Y_ROTATE;
            }

            _vector vArm = XMVector3Normalize(XMVectorLerp(XMLoadFloat3(&m_vStartArm), XMLoadFloat3(&m_pNextArmData->vDesireArm), fRatio));

            XMStoreFloat3(&m_vArm, vArm);
            m_pTransform->Set_Look(vArm);
        }
    }

    // Length 이동
    if (!(m_iMovementFlags & DONE_LENGTH_MOVE)) {

        _float fRatio = m_pGameInstance->Calculate_Ratio(&_pCustomData->fLengthTime, _fTimeDelta, _pCustomData->iLengthRatioType);

        if (_pCustomData->fLengthTime.y >= _pCustomData->fLengthTime.x) {
            _pCustomData->fLengthTime.y = 0.f;
            m_iMovementFlags |= DONE_LENGTH_MOVE;
        }
        else {
            m_fLength = m_pGameInstance->Lerp(m_fStartLength, _pCustomData->fLength, fRatio);
        }
    }

    if ((ALL_DONE_MOVEMENT == (m_iMovementFlags & ALL_DONE_MOVEMENT))) {
        m_iMovementFlags = RESET_FLAG;
        _pCustomData->fMoveTimeAxisY.y = 0.f;
        _pCustomData->fMoveTimeAxisRight.y = 0.f;
        _pCustomData->fLengthTime.y = 0.f;

        return true;
    }

    return false;
}

_bool CCameraArm::Move_To_PreArm(_float _fTimeDelta)
{
    _float fRatio = m_pGameInstance->Calculate_Ratio(&m_fReturnTime, _fTimeDelta, RATIO_TYPE::EASE_IN);

    if (fRatio >= (1.f - EPSILON)) {
        m_fReturnTime.y = 0.f;

        m_vArm = m_PreArms.back().first.vPreArm;
        m_fLength = m_PreArms.back().first.fPreLength;
        m_PreArms.back().second = false;
        m_pTransform->Set_Look(XMVector3Normalize(XMLoadFloat3(&m_vArm)));
        m_PreArms.pop_back();

        return true;
    }

    _vector vArm = XMVector3Normalize(XMVectorLerp(XMLoadFloat3(&m_vStartArm), XMLoadFloat3(&m_PreArms.back().first.vPreArm), fRatio));
    m_fLength = m_pGameInstance->Lerp(m_fStartLength, m_PreArms.back().first.fPreLength, fRatio);

    XMStoreFloat3(&m_vArm, XMVector3Normalize(vArm));
    m_pTransform->Set_Look(vArm);

    return false;
}

_bool CCameraArm::Move_To_FreezeExitArm(_float _fRatio, _fvector _vFreezeExitArm, _float _fFreezeExitLength)
{
    if (_fRatio >= (1.f - EPSILON)) {
        XMStoreFloat3(&m_vArm, XMVector3Normalize(_vFreezeExitArm));
        m_pTransform->Set_Look(XMLoadFloat3(&m_vArm));
        m_vStartArm = { 0.f, 0.f, 0.f };
       // m_fLength = _fFreezeExitLength;
        return true;
    }

    _vector vArm = XMVectorLerp(XMLoadFloat3(&m_vStartArm), _vFreezeExitArm, _fRatio);
    // m_fLength = m_pGameInstance->Lerp(m_fLength, _fFreezeExitLength, _fRatio);
    // Length는 아직 하지 말아 보기
    XMStoreFloat3(&m_vArm, XMVector3Normalize(vArm));
    m_pTransform->Set_Look(vArm);

    return false;
}

_bool CCameraArm::Reset_To_SettingPoint(_float _fRatio, _fvector _vSettingPoint, _float _fSettingLength)
{
    if (_fRatio >= (1.f - EPSILON)) {
        XMStoreFloat3(&m_vArm, XMVector3Normalize(_vSettingPoint));
        m_pTransform->Set_Look(XMLoadFloat3(&m_vArm));
        m_vStartArm = { 0.f, 0.f, 0.f };
        m_fLength = _fSettingLength;
        return true;
    }

    _vector vArm = XMVectorLerp(XMLoadFloat3(&m_vStartArm), _vSettingPoint, _fRatio);
    m_fLength = m_pGameInstance->Lerp(m_fStartLength, _fSettingLength, _fRatio);
    XMStoreFloat3(&m_vArm, XMVector3Normalize(vArm));
    m_pTransform->Set_Look(vArm);

    return false;
}

_bool CCameraArm::Turn_Vector(ARM_DATA* _pCustomData, _float _fTimeDelta)
{
    _float fRatio = m_pGameInstance->Calculate_Ratio(&_pCustomData->fMoveTimeAxisY, _fTimeDelta, _pCustomData->iRotationRatioType);

    if (fRatio >= (1.f - EPSILON)) {

        XMStoreFloat3(&m_vArm, m_pTransform->Get_State(CTransform::STATE_LOOK));
        _pCustomData->fMoveTimeAxisY.y = 0.f;
        return true;
    }

    _vector vArm = XMVector3Normalize(XMVectorLerp(XMLoadFloat3(&m_vStartArm), XMLoadFloat3(&_pCustomData->vDesireArm), fRatio));

    XMStoreFloat3(&m_vArm, vArm);
    m_pTransform->Set_Look(vArm);

    return false;
}

_bool CCameraArm::Turn_AxisY(ARM_DATA* _pCustomData, _float _fTimeDelta)
{
    _float fRatio = m_pGameInstance->Calculate_Ratio(&_pCustomData->fMoveTimeAxisY, _fTimeDelta, LERP);

    if (fRatio >= (1.f - EPSILON)) {
        _pCustomData->fMoveTimeAxisY.y = 0.f;
        return true;
    }
    else {
        _float fRotationPerSec = m_pGameInstance->Lerp(_pCustomData->fRotationPerSecAxisY.x, _pCustomData->fRotationPerSecAxisY.y, fRatio);
        m_pTransform->Set_RotationPerSec(fRotationPerSec);

        m_pTransform->Turn(_fTimeDelta, XMVectorSet(0.0f, 1.f, 0.f, 0.f));

        _vector vLook = m_pTransform->Get_State(CTransform::STATE_LOOK);
        XMStoreFloat3(&m_vArm, vLook);
    }

    return false;
}

_bool CCameraArm::Turn_AxisRight(ARM_DATA* _pCustomData, _float _fTimeDelta)
{
    _float fRatio = m_pGameInstance->Calculate_Ratio(&_pCustomData->fMoveTimeAxisRight, _fTimeDelta, LERP);

    if (fRatio >= (1.f - EPSILON)) {
        _pCustomData->fMoveTimeAxisRight.y = 0.f;
        return true;
    }
    else {
        _float fRotationPerSec = m_pGameInstance->Lerp(_pCustomData->fRotationPerSecAxisRight.x, _pCustomData->fRotationPerSecAxisRight.y, fRatio);
        m_pTransform->Set_RotationPerSec(fRotationPerSec);

        _vector vCross = XMVector3Cross(XMLoadFloat3(&m_vArm), XMVectorSet(0.f, 1.f, 0.f, 0.f));

        m_pTransform->Turn(_fTimeDelta, vCross);

        _vector vLook = m_pTransform->Get_State(CTransform::STATE_LOOK);
        XMStoreFloat3(&m_vArm, vLook);
    }

    return false;
}

_bool CCameraArm::Turn_AxisY(_float _fAngle, _float _fTimeDelta, _uint _iRatioType)
{
    _float fRatio = m_pGameInstance->Calculate_Ratio(&m_fArmTurnTime, _fTimeDelta, _iRatioType);

    if (fRatio >= (1.f - EPSILON)) {
        m_pTransform->TurnAngle(_fAngle - m_fPreArmAngle);
        XMStoreFloat3(&m_vArm, m_pTransform->Get_State(CTransform::STATE_LOOK));
        
        m_fArmTurnTime.y = 0.f;
        m_fPreArmAngle = 0.f;

        return true;
    }

    _float fAngle = m_pGameInstance->Lerp(0.f, _fAngle, fRatio);
    _float fResultAngle = fAngle - m_fPreArmAngle;
    m_fPreArmAngle = fAngle;

    // Y축
    m_pTransform->TurnAngle(fResultAngle);
    XMStoreFloat3(&m_vArm, m_pTransform->Get_State(CTransform::STATE_LOOK));

    return false;
}

_bool CCameraArm::Turn_AxisRight(_float _fAngle, _float _fTimeDelta, _uint _iRatioType)
{
    _float fRatio = m_pGameInstance->Calculate_Ratio(&m_fArmTurnTime, _fTimeDelta, _iRatioType);

    if (fRatio >= (1.f - EPSILON)) {
        m_pTransform->TurnAngle(_fAngle - m_fPreArmAngle);
        XMStoreFloat3(&m_vArm, m_pTransform->Get_State(CTransform::STATE_LOOK));

        m_fArmTurnTime.y = 0.f;
        m_fPreArmAngle = 0.f;

        return true;
    }

    _float fAngle = m_pGameInstance->Lerp(0.f, _fAngle, fRatio);
    _float fResultAngle = fAngle - m_fPreArmAngle;
    m_fPreArmAngle = fAngle;

    // Right
    _vector vCrossX = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMLoadFloat3(&m_vArm));
    m_pTransform->TurnAngle(fResultAngle, vCrossX);
    XMStoreFloat3(&m_vArm, m_pTransform->Get_State(CTransform::STATE_LOOK));

    return false;
}

_bool CCameraArm::Change_Length(ARM_DATA* _pCustomData, _float _fTimeDelta)
{
    _float fRatio = m_pGameInstance->Calculate_Ratio(&_pCustomData->fLengthTime, _fTimeDelta, _pCustomData->iLengthRatioType);

    if (fRatio >= (1.f - EPSILON)) {
        _pCustomData->fLengthTime.y = 0.f;
        m_fLength = _pCustomData->fLength;
        return true;
    }
    else {
        m_fLength = m_pGameInstance->Lerp(m_fStartLength, _pCustomData->fLength, fRatio);
    }

    return false;
}

CCameraArm* CCameraArm::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* pArg)
{
    CCameraArm* pInstance = new CCameraArm(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CCameraArm");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CCameraArm* CCameraArm::Clone()
{
    CCameraArm* pInstance = new CCameraArm(*this);

    if (FAILED(pInstance->Initialize_Clone()))
    {
        MSG_BOX("Failed to Cloned : CCameraArm");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCameraArm::Free()
{
    Safe_Release(m_pTransform);
    Safe_Release(m_pGameInstance);
    Safe_Release(m_pContext);
    Safe_Release(m_pDevice);

    __super::Free();
}
