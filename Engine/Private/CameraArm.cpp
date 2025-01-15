#include "CameraArm.h"

CCameraArm::CCameraArm(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
}

CCameraArm::CCameraArm(const CCameraArm& Prototype)
{
}

HRESULT CCameraArm::Initialize(void* pArg)
{
    CAMERA_ARM_DESC* pDesc = static_cast<CAMERA_ARM_DESC*>(pArg);

    m_vArm = pDesc->vArm;
    m_vPosOffset = pDesc->vPosOffset;
    m_vRotation = pDesc->vRotation;
    m_fLength = pDesc->fLength;
    m_wszArmTag = pDesc->wszArmTag;

    m_pTargetWorldMatrix = pDesc->pTargetWorldMatrix;


    return S_OK;
}

void CCameraArm::Priority_Update(_float fTimeDelta)
{
}

void CCameraArm::Update(_float fTimeDelta)
{
    Set_CameraPos(fTimeDelta);
}

void CCameraArm::Late_Update(_float fTimeDelta)
{
}

void CCameraArm::Set_CameraPos(_float fTimeDelta)
{
    _vector vTargetPos;

    memcpy(&vTargetPos, m_pTargetWorldMatrix->m[3], sizeof(_float4));

    _vector vCameraPos = vTargetPos + (m_fLength * XMLoadFloat3(&m_vArm)) + XMLoadFloat3(&m_vPosOffset);
}

void CCameraArm::Turn_Arm(_float fTimeDelta)
{
}

CCameraArm* CCameraArm::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
    CCameraArm* pInstance = new CCameraArm(pDevice, pContext);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CCameraArm");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCameraArm::Free()
{
    __super::Free();
}
