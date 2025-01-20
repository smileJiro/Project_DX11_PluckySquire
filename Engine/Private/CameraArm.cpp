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

#ifdef _DEBUG
    Set_PrimitiveBatch();
#endif // _DEBUG

    Turn_ArmX(m_vRotation.x);
    Turn_ArmY(m_vRotation.y);

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

_vector CCameraArm::Calculate_CameraPos(_float fTimeDelta)
{
    _vector vTargetPos;

    memcpy(&vTargetPos, m_pTargetWorldMatrix->m[3], sizeof(_float4));
    vTargetPos = vTargetPos; // +XMLoadFloat3(&m_vPosOffset);

    _vector vCameraPos = vTargetPos + (m_fLength * XMLoadFloat3(&m_vArm));

    return vCameraPos;
   // m_pGameInstance->Set_CameraPos(vCameraPos, vTargetPos);
}

void CCameraArm::Set_Rotation(_vector _vRotation)
{
    XMStoreFloat3(&m_vRotation, _vRotation);
    Turn_ArmX(m_vRotation.x);
    Turn_ArmY(m_vRotation.y);
}

void CCameraArm::Turn_ArmX(_float fAngle)
{
    // Right축
    _vector vCrossX = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMLoadFloat3(&m_vArm));
    _matrix RotationMatrix;

    if (0 < XMVectorGetY(vCrossX))
        RotationMatrix = XMMatrixRotationAxis(vCrossX, fAngle);
    else
        RotationMatrix = XMMatrixRotationAxis(vCrossX, -fAngle);

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

_float CCameraArm::Calculate_Ratio()
{
    return _float();
}

_bool CCameraArm::Move_To_NextArm_Cross(_float _fTimeDelta)
{
    _vector vArm = XMLoadFloat3(&m_vArm);
    _vector vNextArm = XMLoadFloat3(&m_pNextArmData->vArm);

    _vector vCross = XMVector3Cross(vArm, vNextArm);

    if (false == m_bSave) {
        _vector vDot = XMVector3Dot(XMVector3Normalize(vArm), XMVector3Normalize(vNextArm));
        m_fSavedAngle = acos(XMVectorGetX(vDot));
        m_bSave = true;
    }

    //_vector vResultArm;

    m_pNextArmData->fMoveTimeAxisY.y += _fTimeDelta;
    _float fRatio = m_pNextArmData->fMoveTimeAxisY.y / m_pNextArmData->fMoveTimeAxisY.x;

    if (fRatio > 1.f) {
        m_vArm = m_pNextArmData->vArm;
        m_pNextArmData->fMoveTimeAxisY.y = 0.f;
        m_bSave = false;
        m_fPreAngle = 0.f;
        return true;
    }

    _float fTotalAngle = m_pGameInstance->Lerp(0.f, m_fSavedAngle, fRatio);

    _float fResultAngle = fTotalAngle - m_fPreAngle;

    m_fPreAngle = fTotalAngle;

    _vector quat = {};

    if (XMVectorGetY(vCross) >= 0) {
        quat = XMQuaternionRotationAxis(XMVector3Normalize(vCross), fResultAngle);
    }
    else {
        quat = XMQuaternionRotationAxis(XMVector3Normalize(vCross), -fResultAngle);
    }

    _matrix RotationMatrix = XMMatrixRotationQuaternion(quat);

    vArm = XMVector3TransformNormal(vArm, RotationMatrix);

    XMStoreFloat3(&m_vArm, vArm);

    return false;
}

_bool CCameraArm::Move_To_NextArm_NotCross(_float _fTimeDelta)
{
   /* if (false == m_bSave) {
        m_SavedArm = m_vArm;
        m_bSave = true;
    }

    _vector vArm = XMLoadFloat3(&m_SavedArm);
    _vector vNextArm = XMLoadFloat3(&m_pNextArmData->vArm);

    m_pNextArmData->fMoveTime.y += _fTimeDelta;
    _float fRatio = m_pNextArmData->fMoveTime.y / m_pNextArmData->fMoveTime.x;

    if (fRatio > 1.f) {
        m_vArm = m_pNextArmData->vArm;
        m_pNextArmData->fMoveTime.y = 0.f;
        m_SavedArm = {};
        return true;
    }

    _vector vResultArm = XMVectorLerp(XMVector3Normalize(vArm), XMVector3Normalize(vNextArm), fRatio);

    XMStoreFloat3(&m_vArm, vResultArm);*/

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
    Safe_Release(m_pContext);
    Safe_Release(m_pDevice);
    Safe_Release(m_pGameInstance);

#ifdef _DEBUG
    Safe_Delete(m_pBatch);
    Safe_Delete(m_pEffect);

    Safe_Release(m_pInputLayout);
    Safe_Release(m_pDepthStencilState);
#endif

    __super::Free();
}
