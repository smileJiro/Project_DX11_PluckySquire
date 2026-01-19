#include "Light_Target.h"
#include "Light_Target.h"
#include "GameObject.h"

CLight_Target::CLight_Target(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, LIGHT_TYPE _eLightType)
    :CLight(_pDevice, _pContext, _eLightType)
{
    m_strName += "(Target)";
}

HRESULT CLight_Target::Initialize(const CONST_LIGHT& _LightDesc, CGameObject* _pTargetOwner, const _float3& _vOffsetPos, _bool _isNotClear)
{
    if (nullptr == _pTargetOwner)
        return E_FAIL;
    m_pTargetOwner = _pTargetOwner; // 순환참조 Ref 관리하지 않는다.
    m_vOffsetPostion = _vOffsetPos;

    if (FAILED(__super::Initialize(_LightDesc)))
        return E_FAIL;

    m_isNotClear = _isNotClear;

    Chase_Target();
    return S_OK;
}

void CLight_Target::Update(_float _fTimeDelta)
{
    if (true == m_pTargetOwner->Is_Dead())
        m_pTargetOwner = nullptr;

    if (nullptr == m_pTargetOwner)
        return;

    Chase_Target();
}


void CLight_Target::Chase_Target()
{
    if (nullptr == m_pTargetOwner)
        return;

    _vector vFinalPos = m_pTargetOwner->Get_FinalPosition() + XMLoadFloat3(&m_vOffsetPostion);
    XMStoreFloat3(&m_tLightConstData.vPosition, vFinalPos);

    Update_LightConstBuffer();
}

CLight_Target* CLight_Target::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const CONST_LIGHT& _LightDesc, LIGHT_TYPE _eLightType, CGameObject* _pTargetOwner, const _float3& _vOffsetPos, _bool _isNotClear)
{
    CLight_Target* pInstance = new CLight_Target(_pDevice, _pContext, _eLightType);

    if (FAILED(pInstance->Initialize(_LightDesc, _pTargetOwner, _vOffsetPos, _isNotClear)))
    {
        MSG_BOX("Failed to Created : CLight_Target");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLight_Target::Free()
{
    m_pTargetOwner = nullptr;

    __super::Free();
}
