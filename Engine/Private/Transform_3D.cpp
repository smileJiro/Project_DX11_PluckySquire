#include "Transform_3D.h"
#include "Shader.h"
#include "GameInstance.h"

CTransform_3D::CTransform_3D(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CTransform(_pDevice, _pContext)
{
}

CTransform_3D::CTransform_3D(const CTransform_3D& _Prototype)
    :CTransform(_Prototype)
{
}

HRESULT CTransform_3D::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CTransform_3D::Initialize(void* _pArg)
{
    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;
	TRANSFORM_3D_DESC* pDesc = static_cast<TRANSFORM_3D_DESC*>(_pArg);
    if(!pDesc->isMatrix)
	    RotationQuaternion(pDesc->vInitialRotation);
    return S_OK;
}

_bool CTransform_3D::Check_Arrival(_fvector _vTargetPos, _float _fEpsilon)
{
    _float fLength = XMVectorGetX(XMVector3Length(_vTargetPos - Get_State(STATE_POSITION)));
    if (_fEpsilon >= fLength)
    {
        return true;
    }
    else
        return false;
}

_bool CTransform_3D::MoveToTarget(_fvector _vTargetPos, _float _fTimeDelta)
{
    static _float fEpsilon = 0.5f;
    _vector vPos = Get_State(STATE_POSITION);
    if (true == Check_Arrival(_vTargetPos, fEpsilon))
    {
        Set_State(CTransform_3D::STATE_POSITION, XMVectorSetW(_vTargetPos, 1.0f));
        return true;
    }
        
    _vector vDirection = XMVector3Normalize(XMVectorSetY(_vTargetPos - vPos, 0.0f));
    Set_AutoRotationYDirection(vDirection);
    Go_Straight(_fTimeDelta);

    return false;
}

_bool CTransform_3D::MoveTo(_fvector _vTargetPos, _float _fTimeDelta , _float _fEpsilon)
{
    //static _float fEpsilon = 0.5f;
    _vector vPos = Get_State(STATE_POSITION);
    if (true == Check_Arrival(_vTargetPos, _fEpsilon))
    {
        Set_State(CTransform_3D::STATE_POSITION, XMVectorSetW(_vTargetPos, 1.0f));
        return true;
    }

	//_vector vFinalPos = vPos + XMVectorSet(-1.f, 0.f, 0.f, 0.f) * m_fSpeedPerSec * _fTimeDelta;
    _vector vDir = XMVectorSetW(XMVector3Normalize(_vTargetPos - vPos), 0.f);
	_vector vFinalPos = vPos + vDir * m_fSpeedPerSec * _fTimeDelta;

    Set_State(STATE::STATE_POSITION, vFinalPos);

    return false;
}

void CTransform_3D::LookAt(_fvector _vAt, _fvector _vAxis, _float _fAngle)
{
    // 0. Scale 정보를 가져온다.
    _float3 vScale = Get_Scale();

    // 1. 타겟의 위치와 나의 위치를 기반으로 Look Vector를 구한다.
    _vector vPos = Get_State(STATE::STATE_POSITION);
    _vector vLook = _vAt - vPos;
    _vector vRight = XMVector3Cross(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), vLook);
    _vector vUp = XMVector3Cross(vLook, vRight);

    _matrix RotationMatrix = XMMatrixRotationAxis(_vAxis, _fAngle);

    vRight = XMVector3TransformNormal(XMVector3Normalize(vRight), RotationMatrix);
    vUp = XMVector3TransformNormal(XMVector3Normalize(vUp), RotationMatrix);
    vLook = XMVector3TransformNormal(XMVector3Normalize(vLook), RotationMatrix);

    // 2. 정규화 * Scale 후 값 세팅
    Set_State(STATE::STATE_RIGHT, XMVector3Normalize(vRight) * vScale.x);
    Set_State(STATE::STATE_UP, XMVector3Normalize(vUp) * vScale.y);
    Set_State(STATE::STATE_LOOK, XMVector3Normalize(vLook) * vScale.z);
}

void CTransform_3D::LookAt(_fvector _vAt, _float _fAngle)
{
    // 0. Scale 정보를 가져온다.
    _float3 vScale = Get_Scale();

    // 1. 타겟의 위치와 나의 위치를 기반으로 Look Vector를 구한다.
    _vector vPos = Get_State(STATE::STATE_POSITION);
    _vector vLook = _vAt - vPos;
    _vector vRight = XMVector3Cross(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), vLook);
    _vector vUp = XMVector3Cross(vLook, vRight);

    _matrix RotationMatrix = XMMatrixRotationAxis(vLook, _fAngle);

    vRight = XMVector3TransformNormal(XMVector3Normalize(vRight), RotationMatrix);
    vUp = XMVector3TransformNormal(XMVector3Normalize(vUp), RotationMatrix);
    //vLook = XMVector3TransformNormal(XMVector3Normalize(vLook), RotationMatrix);

    // 직교화
    vRight = XMVector3Normalize(XMVector3Cross(vUp, vLook));
    vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));

    // 2. 정규화 * Scale 후 값 세팅
    Set_State(STATE::STATE_RIGHT, XMVector3Normalize(vRight) * vScale.x);
    Set_State(STATE::STATE_UP, XMVector3Normalize(vUp) * vScale.y);
    Set_State(STATE::STATE_LOOK, XMVector3Normalize(vLook) * vScale.z);
}


void CTransform_3D::Update_AutoRotationY(_float _fTimeDelta)
{
    static const float fEpsilon = 0.01f;

    // 1. Target Direction과 현재 나의 Look 벡터의 외적의 결과가 0에 가깝다면.. return;
    _vector vCrossProduct = XMVector3Cross(XMVector3Normalize(Get_State(STATE::STATE_LOOK)), XMLoadFloat3(&m_vAutoRotationYDirection));
    _float fCrossProductLength = 0.0f;
    XMStoreFloat(&fCrossProductLength,XMVector3Length(vCrossProduct));
    if (fCrossProductLength < fEpsilon)
    {
        // 외적의 결과가 0에 근접한다면 판별해야할 조건들이 몇가지 있다.
        _float dotProduct = XMVectorGetX(XMVector3Dot(XMVector3Normalize(Get_State(STATE::STATE_LOOK)), XMLoadFloat3(&m_vAutoRotationYDirection)));
       
        if (dotProduct > 0)
        {
            return;
        }
    }
    
    
    // 2. 돌아야할 방향을 결정하자. (외적의 결과 중 Y 값이 음수라면 -방향; 양수라면 +방향)
    if (XMVectorGetY(vCrossProduct) < 0)
    {
        _fTimeDelta *= -1;
    }

    if (fCrossProductLength < 20.f * fEpsilon)
    {
        Turn(_fTimeDelta * 0.1f, XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
    }
    else
    {
        Turn(_fTimeDelta, XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
    }

}

_float CTransform_3D::Compute_Distance_Scaled(CTransform_3D* const _pTargetTransform) const
{
    _vector vPos = Get_State(STATE_POSITION);
    _vector vTargetPos = _pTargetTransform->Get_State(STATE_POSITION);
    _float3 vTargetScale = _pTargetTransform->Get_Scale();
    /* 현재 내가 존재하는 방향 벡터를 구하고 그방향쪽으로 타겟포스에 스케일값을 더해줘야함. */
    _vector vTargetHalfScale = XMVectorSet(vTargetScale.x * 0.5f, vTargetScale.y * 0.5f, vTargetScale.x * 0.5f, 0.0f);
    vTargetPos -= XMVector3Normalize(vPos - vTargetPos) * vTargetHalfScale;

    return XMVectorGetX(XMVector3Length(vTargetPos - vPos));
}

CTransform_3D* CTransform_3D::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CTransform_3D* pInstance = new CTransform_3D(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CTransform_3D");
        Safe_Release(pInstance);
    }

    return pInstance;
}


CComponent* CTransform_3D::Clone(void* pArg)
{
    CTransform_3D* pInstance = new CTransform_3D(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CTransform_3D");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CTransform_3D::Free()
{
    __super::Free();
}
