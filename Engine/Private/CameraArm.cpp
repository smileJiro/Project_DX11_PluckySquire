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
    , m_pTargetWorldMatrix(Prototype.m_pTargetWorldMatrix)
    , m_eArmType(COPY)
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
    m_eArmType = pDesc->eArmType;
    
    m_pTargetWorldMatrix = pDesc->pTargetWorldMatrix;

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
    m_pEffect->SetWorld(XMMatrixIdentity());
    m_pEffect->SetView(m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));
    m_pEffect->SetProjection(m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ));

    m_pEffect->Apply(m_pContext);

    m_pContext->OMSetDepthStencilState(m_pDepthStencilState, 0);
    m_pContext->IASetInputLayout(m_pInputLayout);

    m_pBatch->Begin();

    _vector vTargetPos;

    memcpy(&vTargetPos, m_pTargetWorldMatrix->m[3], sizeof(_float4));
    vTargetPos = vTargetPos; // +XMLoadFloat3(&m_vPosOffset);

    _vector vCameraPos = vTargetPos + (m_fLength * XMLoadFloat3(&m_vArm));
    _vector vColor = {};

    switch (m_eArmType) {
    case DEFAULT:
        vColor = Colors::Red;
        break;
    case COPY:
        vColor = Colors::White;
        break;
    case OTHER:
        vColor = Colors::Yellow;
        break;
    }
    
    m_pBatch->DrawLine(
        VertexPositionColor(vTargetPos, vColor),  // 시작점, 빨간색
        VertexPositionColor(vCameraPos, vColor)     // 끝점, 빨간색
    );

    m_pBatch->End();
}
#endif

#ifdef _DEBUG
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

    // 초기 회전
    _vector vCrossX = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMLoadFloat3(&m_vArm));
    m_pTransform->TurnAngle(m_vRotation.x, vCrossX);
    m_pTransform->TurnAngle(m_vRotation.y);

    XMStoreFloat3(&m_vArm, m_pTransform->Get_State(CTransform::STATE_LOOK));
}

_vector CCameraArm::Calculate_CameraPos(_float fTimeDelta)
{
    _vector vTargetPos;

    memcpy(&vTargetPos, m_pTargetWorldMatrix->m[3], sizeof(_float4));
    vTargetPos = vTargetPos; // +XMLoadFloat3(&m_vPosOffset);

    _vector vCameraPos = vTargetPos + (m_fLength * XMLoadFloat3(&m_vArm));

    return vCameraPos;
   // m_pGameInstance->Set_CameraPos(vCameraPos, vTargetPos);
}

#ifdef _DEBUG
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

#endif // _DEBUG


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

_float CCameraArm::Calculate_Ratio(_uint _iTimeRate, _float2* _fTime, _float _fTimeDelta)
{
    _float fRatio = {};

    //_fTime->y += _fTimeDelta;
    //fRatio = _fTime->y / _fTime->x;

    //switch (_iTimeRate) {
    //case EASE_IN:
    //    fRatio = (fRatio + pow(fRatio, 2)) * 0.5f;
    //    break;
    //case EASE_OUT:
    //    fRatio = 1.0f - ((1.0f - fRatio) + pow(1.0f - fRatio, 2)) * 0.5f;
    //    break;
    //case LERP:
    //    break;
    //}

    return fRatio;
}

_bool CCameraArm::Move_To_NextArm(_float _fTimeDelta)
{
    // Y축 회전
    if (!(m_iRoateFlags & DONE_Y_ROTATE)) { // 안 끝났을 때
       
        m_pNextArmData->fMoveTimeAxisY.y += _fTimeDelta;
        _float fRatio = m_pNextArmData->fMoveTimeAxisY.y / m_pNextArmData->fMoveTimeAxisY.x;

        if (m_pNextArmData->fMoveTimeAxisY.y >= m_pNextArmData->fMoveTimeAxisY.x) {
            m_pNextArmData->fMoveTimeAxisY.y = 0.f;
            m_iRoateFlags |= DONE_Y_ROTATE;
        }
        else {
            _float fRotationPerSec = m_pGameInstance->Lerp(m_pNextArmData->fRotationPerSecAxisY.x, m_pNextArmData->fRotationPerSecAxisY.y, fRatio);
            m_pTransform->Set_RotationPerSec(fRotationPerSec);

            m_pTransform->Turn(_fTimeDelta, XMVectorSet(0.0f, 1.f, 0.f, 0.f));

            _vector vLook = m_pTransform->Get_State(CTransform::STATE_LOOK);
            XMStoreFloat3(&m_vArm, vLook);
        }
    }

    if (!(m_iRoateFlags & DONE_RIGHT_ROTATE)) {

        m_pNextArmData->fMoveTimeAxisRight.y += _fTimeDelta;
        _float fRatio = m_pNextArmData->fMoveTimeAxisRight.y / m_pNextArmData->fMoveTimeAxisRight.x;

        if (m_pNextArmData->fMoveTimeAxisRight.y >= m_pNextArmData->fMoveTimeAxisRight.x) {
            m_pNextArmData->fMoveTimeAxisRight.y = 0.f;
            m_iRoateFlags |= DONE_RIGHT_ROTATE;
        }
        else {
            _float fRotationPerSec = m_pGameInstance->Lerp(m_pNextArmData->fRotationPerSecAxisRight.x, m_pNextArmData->fRotationPerSecAxisRight.y, fRatio);
            m_pTransform->Set_RotationPerSec(fRotationPerSec);

            _vector vCross = XMVector3Cross(XMLoadFloat3(&m_vArm), XMVectorSet(0.f, 1.f, 0.f, 0.f));

            m_pTransform->Turn(_fTimeDelta, vCross);

            _vector vLook = m_pTransform->Get_State(CTransform::STATE_LOOK);
            XMStoreFloat3(&m_vArm, vLook);
        }
    }

    if (ALL_DONE_ROTATE == (m_iRoateFlags & ALL_DONE_ROTATE)) {
        m_iRoateFlags = RESET_FLAG;

        return true;
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
#ifdef _DEBUG
    Safe_Delete(m_pBatch);
    Safe_Delete(m_pEffect);

    Safe_Release(m_pInputLayout);
    Safe_Release(m_pDepthStencilState);
#endif

    __super::Free();
}
