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
    , m_wszArmTag(TEXT("CopyArm"))
    , m_vRotation(Prototype.m_vRotation)
    , m_fLength(Prototype.m_fLength)
    //, m_pTargetWorldMatrix(Prototype.m_pTargetWorldMatrix)
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
    m_wszArmTag = pDesc->wszArmTag;

    //m_pTargetWorldMatrix = pDesc->pTargetWorldMatrix;

    m_pTransform = CTransform_3D::Create(m_pDevice, m_pContext);
    if (nullptr == m_pTransform)
        return E_FAIL;

    if (FAILED(m_pTransform->Initialize(&pDesc)))
        return E_FAIL;

    Set_WorldMatrix();

#ifdef _DEBUG
    Set_PrimitiveBatch();
#endif // _DEBUG

  /*  Turn_ArmX(m_vRotation.x);
    Turn_ArmY(m_vRotation.y);*/

    return S_OK;
}

HRESULT CCameraArm::Initialize_Clone()
{
#ifdef _DEBUG
    Set_PrimitiveBatch();
#endif // _DEBUG

    return S_OK;
}

void CCameraArm::Priority_Update(_float fTimeDelta)
{
}

void CCameraArm::Update(_float fTimeDelta)
{
    //Set_CameraPos(fTimeDelta);
}

void CCameraArm::Late_Update(_float fTimeDelta)
{
}

#ifdef _DEBUG
void CCameraArm::Render_Arm()
{
  //  m_pEffect->SetWorld(XMMatrixIdentity());
  //  m_pEffect->SetView(m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));
  //  m_pEffect->SetProjection(m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ));

  //  m_pEffect->Apply(m_pContext);

  //  m_pContext->OMSetDepthStencilState(m_pDepthStencilState, 0);
  //  m_pContext->IASetInputLayout(m_pInputLayout);

  //  m_pBatch->Begin();

  //  _vector vTargetPos;

  ////  memcpy(&vTargetPos, m_pTargetWorldMatrix->m[3], sizeof(_float4));
  //  vTargetPos = vTargetPos; // +XMLoadFloat3(&m_vPosOffset);

  //  _vector vCameraPos = vTargetPos + (m_fLength * XMLoadFloat3(&m_vArm));
  //  _vector vColor = {};

  //  m_pBatch->DrawLine(
  //      VertexPositionColor(vTargetPos, vColor),  // 시작점, 빨간색
  //      VertexPositionColor(vCameraPos, vColor)     // 끝점, 빨간색
  //  );

  //  m_pBatch->End();
}

HRESULT CCameraArm::Set_PrimitiveBatch()
{
    m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pContext);
    m_pEffect = new BasicEffect(m_pDevice);

    // DepthStencilState 생성
    D3D11_DEPTH_STENCIL_DESC depthDesc;
    ZeroMemory(&depthDesc, sizeof(depthDesc));
    depthDesc.DepthEnable = TRUE;
    depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthDesc.DepthFunc = D3D11_COMPARISON_LESS;

    if (FAILED(m_pDevice->CreateDepthStencilState(&depthDesc, &m_pDepthStencilState)))
        return E_FAIL;

    const void* pShaderByteCode = nullptr;
    size_t iShaderByteCodeLength = 0;

    /* m_pEffect 쉐이더의 옵션 자체를 Color 타입으로 변경해주어야 정상적인 동작을 한다. (기본 값은 Texture )*/
    m_pEffect->SetVertexColorEnabled(true);
    m_pEffect->GetVertexShaderBytecode(&pShaderByteCode, &iShaderByteCodeLength);

    if (FAILED(m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, pShaderByteCode, iShaderByteCodeLength, &m_pInputLayout)))
        return E_FAIL;

    return S_OK;
}
#endif

void CCameraArm::Set_WorldMatrix()
{
    _vector vLook = XMLoadFloat3(&m_vArm);
    _vector vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
    _vector vUp = XMVector3Cross(vLook, vRight);

    m_pTransform->Set_State(CTransform::STATE_RIGHT, XMVector3Normalize(vRight));
    m_pTransform->Set_State(CTransform::STATE_UP, XMVector3Normalize(vUp));
    m_pTransform->Set_State(CTransform::STATE_LOOK, XMVector3Normalize(vLook));

    //// 초기 회전
    _vector vCrossX = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMLoadFloat3(&m_vArm));
    m_pTransform->TurnAngle(m_vRotation.x, vCrossX);
    m_pTransform->TurnAngle(m_vRotation.y);

    XMStoreFloat3(&m_vArm, m_pTransform->Get_State(CTransform::STATE_LOOK));
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
    m_fRotationValue = 1.f;
    m_iMovementFlags = RESET_FLAG;

    m_pNextArmData = _pData;
    m_fStartLength = m_fLength;
    m_vStartArm = m_vArm;

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

//_vector CCameraArm::Calculate_CameraPos(_float fTimeDelta)
//{
//    _vector vTargetPos;
//
//    memcpy(&vTargetPos, m_pTargetWorldMatrix->m[3], sizeof(_float4));
//    vTargetPos = vTargetPos; // +XMLoadFloat3(&m_vPosOffset);
//
//    _vector vCameraPos = vTargetPos + (m_fLength * XMLoadFloat3(&m_vArm));
//
//    return vCameraPos;
//   // m_pGameInstance->Set_CameraPos(vCameraPos, vTargetPos);
//}

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
    XMStoreFloat3(&m_vArm, _vArm);

    _vector vLook = XMLoadFloat3(&m_vArm);
    _vector vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
    _vector vUp = XMVector3Cross(vLook, vRight);

    m_pTransform->Set_State(CTransform::STATE_RIGHT, XMVector3Normalize(vRight));
    m_pTransform->Set_State(CTransform::STATE_UP, XMVector3Normalize(vUp));
    m_pTransform->Set_State(CTransform::STATE_LOOK, XMVector3Normalize(vLook));
}

void CCameraArm::Set_StartInfo(_vector _vArm, _float _fLength)
{
    XMStoreFloat3(&m_vStartArm, _vArm);
    m_fStartLength = _fLength;
}

void CCameraArm::Turn_ArmX(_float fAngle)
{
    // Right축
    _vector vCrossX = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMLoadFloat3(&m_vArm));
    _matrix RotationMatrix;

    if (0 < XMVectorGetY(vCrossX)) {
        RotationMatrix = XMMatrixRotationAxis(vCrossX, -fAngle);
        cout << "fAngle+++++++++++++++" << endl;
    }
    else {
        RotationMatrix = XMMatrixRotationAxis(vCrossX, +fAngle);
        cout << "fAngle----------------" << endl;
    }

    _vector vRoatateArm = XMVector3TransformNormal(XMLoadFloat3(&m_vArm), RotationMatrix);
    XMStoreFloat3(&m_vArm, vRoatateArm);


}

void CCameraArm::Turn_ArmY(_float fAngle)
{
    // Y축
    _matrix	RotationMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), fAngle);

    _vector vRoatateArm = XMVector3TransformNormal(XMLoadFloat3(&m_vArm), RotationMatrix);
    XMStoreFloat3(&m_vArm, vRoatateArm);
}

_float CCameraArm::Calculate_Ratio(_float2* _fTime, _float _fTimeDelta, _uint _iRatioType)
{
    _float fRatio = {};

    _fTime->y += _fTimeDelta;
    fRatio = _fTime->y / _fTime->x;
    fRatio = clamp(fRatio, 0.f, 1.f);

    switch (_iRatioType) {
    case (_uint)RATIO_TYPE::EASE_IN:
        fRatio = (fRatio + (_float)pow((_double)fRatio, (_double)2.f)) * 0.5f;
        //fRatio = fRatio * fRatio;
        break;
    case (_uint)RATIO_TYPE::EASE_OUT:
        //fRatio = 1.0f - ((1.0f - fRatio) + (_float)pow((_double)(1.0f - fRatio), 2.f)) * 0.5f;
        fRatio = 1.0f - (1.0f - fRatio) * (1.0f - fRatio);
        break;
    case (_uint)RATIO_TYPE::LERP:
        break;
    case (_uint)RATIO_TYPE::EASE_IN_OUT:
        fRatio = fRatio * fRatio * (3 - 2 * fRatio);
        break;
    }

    return fRatio;
}

_bool CCameraArm::Check_IsNear_ToDesireArm(_float _fTimeDelta)
{
    _float fDot = XMVectorGetX(XMVector3Dot(XMVector3Normalize(XMLoadFloat3(&m_vArm)), XMVector3Normalize(XMLoadFloat3(&m_pNextArmData->vDesireArm))));
    _float fAngle = acos(fDot);
    _float f = XMConvertToDegrees(fAngle);
 
    if (XMConvertToDegrees(fAngle) < 10.f) {
        _vector vArm = XMVector3Normalize(XMVectorLerp(XMLoadFloat3(&m_vArm), XMLoadFloat3(&m_pNextArmData->vDesireArm), 0.05f));
        XMStoreFloat3(&m_vArm, (vArm));
        m_pTransform->Set_Look(vArm);

        return true;
    }
    //else if(XMConvertToDegrees(fAngle) < 10.f) {

    //    if()

    //    m_fRotationValue = m_pGameInstance->Lerp(m_fRotationValue, 0.1f, 0.1f);
  
    //    return false;
    //}

    return false;
}

_bool CCameraArm::Move_To_NextArm(_float _fTimeDelta)
{
    _bool isNear = Check_IsNear_ToDesireArm(_fTimeDelta);

    if (true == isNear) {
        m_iMovementFlags |= DONE_Y_ROTATE;
        m_iMovementFlags |= DONE_RIGHT_ROTATE;
        m_pNextArmData->fMoveTimeAxisY.y = 0.f;
        m_pNextArmData->fMoveTimeAxisRight.y = 0.f;
        m_fRotationValue = 1.f;
    }

    // Y축 회전
    if (!(m_iMovementFlags & DONE_Y_ROTATE)) { // 안 끝났을 때
       
        m_pNextArmData->fMoveTimeAxisY.y += _fTimeDelta;
        _float fRatio = m_pNextArmData->fMoveTimeAxisY.y / m_pNextArmData->fMoveTimeAxisY.x;

        if (m_pNextArmData->fMoveTimeAxisY.y >= m_pNextArmData->fMoveTimeAxisY.x) {
            m_pNextArmData->fMoveTimeAxisY.y = 0.f;
            m_iMovementFlags |= DONE_Y_ROTATE;
        }
        else {
            _float fRotationPerSec = m_pGameInstance->Lerp(m_pNextArmData->fRotationPerSecAxisY.x, m_pNextArmData->fRotationPerSecAxisY.y, fRatio);
            fRotationPerSec *= m_fRotationValue;
            m_pTransform->Set_RotationPerSec(fRotationPerSec);

            m_pTransform->Turn(_fTimeDelta, XMVectorSet(0.0f, 1.f, 0.f, 0.f));

            _vector vLook = m_pTransform->Get_State(CTransform::STATE_LOOK);
            XMStoreFloat3(&m_vArm, vLook);
        }
    }

    // Right 축 회전
    if (!(m_iMovementFlags & DONE_RIGHT_ROTATE)) {

        m_pNextArmData->fMoveTimeAxisRight.y += _fTimeDelta;
        _float fRatio = m_pNextArmData->fMoveTimeAxisRight.y / m_pNextArmData->fMoveTimeAxisRight.x;

        if (m_pNextArmData->fMoveTimeAxisRight.y >= m_pNextArmData->fMoveTimeAxisRight.x) {
            m_pNextArmData->fMoveTimeAxisRight.y = 0.f;
            m_iMovementFlags |= DONE_RIGHT_ROTATE;
        }
        else {
            _float fRotationPerSec = m_pGameInstance->Lerp(m_pNextArmData->fRotationPerSecAxisRight.x, m_pNextArmData->fRotationPerSecAxisRight.y, fRatio);
            fRotationPerSec *= m_fRotationValue;
            m_pTransform->Set_RotationPerSec(fRotationPerSec);

            _vector vCross = XMVector3Cross(XMLoadFloat3(&m_vArm), XMVectorSet(0.f, 1.f, 0.f, 0.f));

            m_pTransform->Turn(_fTimeDelta, vCross);

            _vector vLook = m_pTransform->Get_State(CTransform::STATE_LOOK);
            XMStoreFloat3(&m_vArm, vLook);
        }
    }
    
    // Length 이동
    if (!(m_iMovementFlags & DONE_LENGTH_MOVE)) {

        _float fRatio = Calculate_Ratio(&m_pNextArmData->fLengthTime, _fTimeDelta, m_pNextArmData->iLengthRatioType);

        if (m_pNextArmData->fLengthTime.y >= m_pNextArmData->fLengthTime.x) {
            m_pNextArmData->fLengthTime.y = 0.f;
            m_iMovementFlags |= DONE_LENGTH_MOVE;
        }
        else {
            m_fLength = m_pGameInstance->Lerp(m_fStartLength, m_pNextArmData->fLength, fRatio);
        }
    }

    if ((ALL_DONE_MOVEMENT == (m_iMovementFlags & ALL_DONE_MOVEMENT))) {
        m_iMovementFlags = RESET_FLAG;
        m_fRotationValue = 1.f;
        m_pNextArmData->fMoveTimeAxisY.y = 0.f;
        m_pNextArmData->fMoveTimeAxisRight.y = 0.f;

        return true;
    }

    return false;
}

_bool CCameraArm::Move_To_NextArm_ByVector(_float _fTimeDelta)
{
    _vector vDot = XMVector3Dot(XMLoadFloat3(&m_vArm), XMLoadFloat3(&m_pNextArmData->vDesireArm));
    _float fAngle = acos(XMVectorGetX(vDot));
    _float fDegree = XMConvertToDegrees(fAngle);
    //m_PreArms.pop_back();
    if (fDegree < 3.f) {
        m_iMovementFlags |= DONE_Y_ROTATE;
        m_iMovementFlags |= DONE_LENGTH_MOVE;
        m_pNextArmData->fMoveTimeAxisY.y = 0.f;
        m_pNextArmData->fLengthTime.y = 0.f;
        //if()
        return true;
    }

    // 일단 Y축 회전 시간, EASE_IN으로 설정해서 하기
    if (!(m_iMovementFlags & DONE_Y_ROTATE)) {
        _float fRatio = m_pGameInstance->Calculate_Ratio(&m_pNextArmData->fMoveTimeAxisY, _fTimeDelta, m_pNextArmData->iRotationRatioType);

        if (fRatio >= 1.f) {

            m_vArm = m_pNextArmData->vDesireArm;
            m_pTransform->Set_Look(XMLoadFloat3(&m_vArm));
            m_pNextArmData->fMoveTimeAxisY.y = 0.f;
            m_iMovementFlags |= DONE_Y_ROTATE;
        }
        else {
            _vector vArm = XMVectorLerp(XMLoadFloat3(&m_vStartArm), XMLoadFloat3(&m_pNextArmData->vDesireArm), fRatio);

            XMStoreFloat3(&m_vArm, vArm);
            m_pTransform->Set_Look(vArm);
        }
    }

    // Length 이동
    if (!(m_iMovementFlags & DONE_LENGTH_MOVE)) {

        _float fRatio = Calculate_Ratio(&m_pNextArmData->fLengthTime, _fTimeDelta, m_pNextArmData->iLengthRatioType);

        if (m_pNextArmData->fLengthTime.y >= m_pNextArmData->fLengthTime.x) {
            m_pNextArmData->fLengthTime.y = 0.f;
            m_iMovementFlags |= DONE_LENGTH_MOVE;
        }
        else {
            m_fLength = m_pGameInstance->Lerp(m_fStartLength, m_pNextArmData->fLength, fRatio);
        }
    }

    return false;
}

_bool CCameraArm::Move_To_PreArm(_float _fTimeDelta)
{
    _float fRatio = Calculate_Ratio(&m_fReturnTime, _fTimeDelta, RATIO_TYPE::EASE_IN);

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

_bool CCameraArm::Move_To_FreezeExitArm(_float _fRatio, _fvector _vFreezeExitArm)
{
    if (_fRatio >= (1.f - EPSILON)) {
        return true;
    }

    _vector vArm = XMVectorLerp(XMLoadFloat3(&m_vArm), _vFreezeExitArm, _fRatio);

    XMStoreFloat3(&m_vArm, vArm);

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
#ifdef _DEBUG
    Safe_Delete(m_pBatch);
    Safe_Delete(m_pEffect);

    Safe_Release(m_pInputLayout);
    Safe_Release(m_pDepthStencilState);
#endif

    __super::Free();
}
