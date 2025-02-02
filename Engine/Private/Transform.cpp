#include "Transform.h"
#include "GameInstance.h"

CTransform::CTransform(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CComponent(_pDevice, _pContext)
{
}

CTransform::CTransform(const CTransform& _Prototype)
    :CComponent(_Prototype)
{
}

HRESULT CTransform::Initialize_Prototype()
{
    XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

    return S_OK;
}

HRESULT CTransform::Initialize(void* _pArg)
{
    if (nullptr == _pArg)
        return S_OK; // nullptr이어도 문제 삼지 않는다. 다만 return 하는 이유는, 아래의 코드 자체가 의미가 없기때문, 하지만 의미가 생긴다면?

    TRANSFORM_DESC* pDesc = static_cast<TRANSFORM_DESC*>(_pArg);
    if (pDesc->isMatrix) 
    {
        Set_WorldMatrix(pDesc->matWorld);
    }
    else 
    {
        m_fSpeedPerSec = pDesc->fSpeedPerSec;
        m_fRotationPerSec = pDesc->fRotationPerSec;

        Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&pDesc->vInitialPosition), 1.0f));
        Set_Scale(pDesc->vInitialScaling.x, pDesc->vInitialScaling.y, pDesc->vInitialScaling.z);
    }
    return S_OK;
}


_bool CTransform::Go_Straight(_float _fTimeDelta)
{
    _vector vPos = Get_State(STATE::STATE_POSITION);
    _vector vLook = Get_State(STATE::STATE_LOOK);
    _vector vFinalPos = vPos + XMVector3Normalize(vLook) * m_fSpeedPerSec * _fTimeDelta;

    Set_State(STATE::STATE_POSITION, vFinalPos);
    return true;
}

_bool CTransform::Go_Backward(_float _fTimeDelta)
{
    _vector vPos = Get_State(STATE::STATE_POSITION);
    _vector vLook = Get_State(STATE::STATE_LOOK);
    _vector vFinalPos = vPos - XMVector3Normalize(vLook) * m_fSpeedPerSec * _fTimeDelta;

    Set_State(STATE::STATE_POSITION, vFinalPos);
    return true;
}

_bool CTransform::Go_Left(_float _fTimeDelta)
{
    _vector vPos = Get_State(STATE::STATE_POSITION);
    _vector vRight = Get_State(STATE::STATE_RIGHT);

    _vector vFinalPos = vPos - XMVector3Normalize(vRight) * m_fSpeedPerSec * _fTimeDelta;

    Set_State(STATE::STATE_POSITION, vFinalPos);
    return true;
}

_bool CTransform::Go_Right(_float _fTimeDelta)
{
    _vector vPos = Get_State(STATE::STATE_POSITION);
    _vector vRight = Get_State(STATE::STATE_RIGHT);
    _vector vFinalPos = vPos + XMVector3Normalize(vRight) * m_fSpeedPerSec * _fTimeDelta;

    Set_State(STATE::STATE_POSITION, vFinalPos);

    return true;
}

_bool CTransform::Go_Up(_float _fTimeDelta)
{
    _vector vPos = Get_State(STATE::STATE_POSITION);
    _vector vUp = Get_State(STATE::STATE_UP);
    _vector vFinalPos = vPos + XMVector3Normalize(vUp) * m_fSpeedPerSec * _fTimeDelta;

    Set_State(STATE::STATE_POSITION, vFinalPos);

    return true;
}

_bool CTransform::Go_Down(_float _fTimeDelta)
{
    _vector vPos = Get_State(STATE::STATE_POSITION);
    _vector vUp = Get_State(STATE::STATE_UP);
    _vector vFinalPos = vPos - XMVector3Normalize(vUp) * m_fSpeedPerSec * _fTimeDelta;

    Set_State(STATE::STATE_POSITION, vFinalPos);

    return true;
}

void CTransform::Go_Direction(_vector _vDirection, _float _fTimeDelta)
{
	_vector vPos = Get_State(STATE::STATE_POSITION);
	_vector vFinalPos = vPos + XMVector3Normalize(_vDirection) * m_fSpeedPerSec * _fTimeDelta;
	Set_State(STATE::STATE_POSITION, vFinalPos);
}

void CTransform::Go_Direction(_vector _vDirection, _float _fSpeed, _float _fTimeDelta)
{
    _vector vPos = Get_State(STATE::STATE_POSITION);
    _vector vFinalPos = vPos + XMVector3Normalize(_vDirection) * _fSpeed * _fTimeDelta;
    Set_State(STATE::STATE_POSITION, vFinalPos);
}

void CTransform::Turn(_float _fTimeDelta, _fvector _vAxis)
{
    // 지속적인 회전.
    _vector vRight = Get_State(STATE::STATE_RIGHT);
    _vector vUp = Get_State(STATE::STATE_UP);
    _vector vLook = Get_State(STATE::STATE_LOOK);

    // 회전 행렬 만들기.
    _matrix RotationMatrix = XMMatrixRotationAxis(XMVector3Normalize(_vAxis), m_fRotationPerSec * _fTimeDelta);

    // vector * matrix
    // set_state
    Set_State(STATE::STATE_RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
    Set_State(STATE::STATE_UP, XMVector3TransformNormal(vUp, RotationMatrix));
    Set_State(STATE::STATE_LOOK, XMVector3TransformNormal(vLook, RotationMatrix));
}

_bool CTransform::Turn_To_DesireDir(_fvector _vStartDir, _fvector _vDesireDir, _float _fRatio)
{
    _vector vCross = XMVector3Cross(Get_State(CTransform::STATE_LOOK), _vDesireDir);
    _vector vDot = XMVector3Dot(XMVector3Normalize(Get_State(CTransform::STATE_LOOK)), XMVector3Normalize(_vDesireDir));
    _float fCrossLength = XMVectorGetX(XMVector3Length(vCross));

    _float fRotationValue = 1.f;

    if (fCrossLength < 0.1f && XMVectorGetX(vDot) > 0) {
        fRotationValue = 0.05f;
        //m_fRotationPerSec = m_fPreRotationPerSec;
    }

    if (fCrossLength < 0.0001f) {
        if (XMVectorGetX(vDot) > 0) {
            Set_Look(_vDesireDir);
            return true;
        }
    }

    _float3		vScale = Get_Scale();

    //		vLook = Get_State(STATE_LOOK);

    _vector     vLook = XMVectorLerp(_vStartDir, _vDesireDir, _fRatio);

    _vector		vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
    _vector		vUp = XMVector3Cross(vLook, vRight);

    Set_State(STATE_RIGHT, XMVector3Normalize(vRight) * vScale.x);
    Set_State(STATE_UP, XMVector3Normalize(vUp) * vScale.y);
    Set_State(STATE_LOOK, XMVector3Normalize(vLook) * vScale.z);
}

void CTransform::TurnAngle(_float _fRadian, _fvector _vAxis)
{
    // 지속적인 회전.
    _vector vRight = Get_State(STATE::STATE_RIGHT);
    _vector vUp = Get_State(STATE::STATE_UP);
    _vector vLook = Get_State(STATE::STATE_LOOK);

    // 회전 행렬 만들기.
    _matrix RotationMatrix = XMMatrixRotationAxis(XMVector3Normalize(_vAxis), _fRadian);

   // vector * matrix
   // set_state
    Set_State(STATE::STATE_RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
    Set_State(STATE::STATE_UP, XMVector3TransformNormal(vUp, RotationMatrix));
    Set_State(STATE::STATE_LOOK, XMVector3TransformNormal(vLook, RotationMatrix));
}

void CTransform::TurnZ(_float _fTimeDelta)
{
    _vector vRight = Get_State(STATE::STATE_RIGHT);
    _vector vUp = Get_State(STATE::STATE_UP);
    _vector vLook = Get_State(STATE::STATE_LOOK);

    _vector AxisZ = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
    _matrix RotationMatrix = XMMatrixRotationAxis(AxisZ, m_fRotationPerSec * _fTimeDelta);

    Set_State(STATE::STATE_RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
    Set_State(STATE::STATE_UP, XMVector3TransformNormal(vUp, RotationMatrix));
    Set_State(STATE::STATE_LOOK, XMVector3TransformNormal(vLook, RotationMatrix));
}

void CTransform::Rotation(_float _fRadian, _fvector _vAxis)
{
    // 항등행렬 기준 회전 값이기때문에 Scale에 대한 값을 적용시켜야함.
    _float3 vScale = Get_Scale();

    _vector vRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f) * vScale.x;
    _vector vUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f) * vScale.y;
    _vector vLook = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f) * vScale.z;

    _matrix RotationMatrix = XMMatrixRotationAxis(_vAxis, _fRadian);

    Set_State(STATE::STATE_RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
    Set_State(STATE::STATE_UP, XMVector3TransformNormal(vUp, RotationMatrix));
    Set_State(STATE::STATE_LOOK, XMVector3TransformNormal(vLook, RotationMatrix));
}

void CTransform::RotationZ(_float _fRadianZ)
{
    // 항등행렬 기준 회전 값이기때문에 Scale에 대한 값을 적용시켜야함.
    _float3 vScale = Get_Scale();

    _vector vRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f) * vScale.x;
    _vector vUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f) * vScale.y;
    _vector vLook = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f) * vScale.z;

    _matrix RotationMatrixZ = XMMatrixRotationAxis(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), _fRadianZ);

    Set_State(STATE::STATE_RIGHT, XMVector3TransformNormal(vRight, RotationMatrixZ));
    Set_State(STATE::STATE_UP, XMVector3TransformNormal(vUp, RotationMatrixZ));
    Set_State(STATE::STATE_LOOK, XMVector3TransformNormal(vLook, RotationMatrixZ));
}

void CTransform::RotationXYZ(const _float3& _vRadianXYZ)
{
    // 항등행렬 기준 회전 값이기때문에 Scale에 대한 값을 적용시켜야함.
    _float3 vScale = Get_Scale();

    _vector vRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f) * vScale.x;
    _vector vUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f) * vScale.y;
    _vector vLook = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f) * vScale.z;

    _matrix RotationMatrixX = XMMatrixRotationAxis(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), _vRadianXYZ.x);
    _matrix RotationMatrixY = XMMatrixRotationAxis(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), _vRadianXYZ.y);
    _matrix RotationMatrixZ = XMMatrixRotationAxis(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), _vRadianXYZ.z);


    Set_State(STATE::STATE_RIGHT, XMVector3TransformNormal(vRight, RotationMatrixX * RotationMatrixY * RotationMatrixZ));
    Set_State(STATE::STATE_UP, XMVector3TransformNormal(vUp, RotationMatrixX * RotationMatrixY * RotationMatrixZ));
    Set_State(STATE::STATE_LOOK, XMVector3TransformNormal(vLook, RotationMatrixX * RotationMatrixY * RotationMatrixZ));
}

void CTransform::RotationQuaternion(const _float3& _vRadianXYZ)
{
    _float3 vScale = Get_Scale();
    _matrix matQuaternion = XMMatrixRotationRollPitchYaw(_vRadianXYZ.x, _vRadianXYZ.y, _vRadianXYZ.z);

    _vector vRight = matQuaternion.r[0] * vScale.x;
    _vector vUp = matQuaternion.r[1] * vScale.y;
    _vector vLook = matQuaternion.r[2] * vScale.z;

    Set_State(STATE::STATE_RIGHT, vRight);
    Set_State(STATE::STATE_UP, vUp);
    Set_State(STATE::STATE_LOOK, vLook);
}

void CTransform::RotationQuaternionW(const _float4& _vQuaternion)
{
    _matrix WorldMatrix = XMMatrixIdentity();
    _vector vQuaternion = XMLoadFloat4(&_vQuaternion);
	WorldMatrix = Get_WorldMatrix() * XMMatrixRotationQuaternion(vQuaternion);

    Set_WorldMatrix(WorldMatrix);
}


_float3 CTransform::Get_Scale()
{
    return _float3(XMVectorGetX(XMVector3Length(Get_State(STATE::STATE_RIGHT))),
                   XMVectorGetX(XMVector3Length(Get_State(STATE::STATE_UP))),
                   XMVectorGetX(XMVector3Length(Get_State(STATE::STATE_LOOK))));
}

void CTransform::Set_Scale(_float _fX, _float _fY, _float _fZ)
{
    // WorldMatrix의 각 행의 Length를 fX,fY,fZ 의 값으로 바꿔주면 된다.

    Set_State(STATE::STATE_RIGHT, XMVector3Normalize(Get_State(STATE::STATE_RIGHT)) * _fX);
    Set_State(STATE::STATE_UP, XMVector3Normalize(Get_State(STATE::STATE_UP)) * _fY);
    Set_State(STATE::STATE_LOOK, XMVector3Normalize(Get_State(STATE::STATE_LOOK)) * _fZ);
}

void CTransform::Set_Scale(const _float3& _vScale)
{
    // WorldMatrix의 각 행의 Length를 fX,fY,fZ 의 값으로 바꿔주면 된다.

    Set_State(STATE::STATE_RIGHT, XMVector3Normalize(Get_State(STATE::STATE_RIGHT)) * _vScale.x);
    Set_State(STATE::STATE_UP, XMVector3Normalize(Get_State(STATE::STATE_UP)) * _vScale.y);
    Set_State(STATE::STATE_LOOK, XMVector3Normalize(Get_State(STATE::STATE_LOOK)) * _vScale.z);
}


_float CTransform::Compute_Distance(_fvector _vTargetPos) const
{
    _vector vPos = Get_State(STATE_POSITION);

    return XMVectorGetX(XMVector3Length(_vTargetPos - vPos));
}

void CTransform::Set_Look(_fvector _vDir)
{
    _float3		vScale = Get_Scale();

    _vector		vPosition = Get_State(STATE_POSITION);
    _vector		vLook = _vDir;
    _vector		vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
    _vector		vUp = XMVector3Cross(vLook, vRight);

    Set_State(STATE_RIGHT, XMVector3Normalize(vRight) * vScale.x);
    Set_State(STATE_UP, XMVector3Normalize(vUp) * vScale.y);
    Set_State(STATE_LOOK, XMVector3Normalize(vLook) * vScale.z);
}

HRESULT CTransform::Bind_ShaderResource(CShader* pShader, const _char* pConstantName) const
{
    return pShader->Bind_Matrix(pConstantName, &m_WorldMatrix);
}

void CTransform::Free()
{
    __super::Free();
}
