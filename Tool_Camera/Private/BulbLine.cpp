#include "stdafx.h"
#include "BulbLine.h"

#include "GameInstance.h"
#include "Bulb.h"

CBulbLine::CBulbLine(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : m_pDevice(_pDevice)
    , m_pContext(_pContext)
    , m_pGameInstance(CGameInstance::GetInstance())
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
    Safe_AddRef(m_pGameInstance);
}

CBulbLine::CBulbLine(const CBulbLine& _Prototype)
    : m_pDevice(_Prototype.m_pDevice)
    , m_pContext(_Prototype.m_pContext)
    , m_pGameInstance(_Prototype.m_pGameInstance)
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
    Safe_AddRef(m_pGameInstance);
}

HRESULT CBulbLine::Initialize(void* pArg)
{
    CBulbLine::BULBLINE_DESC* pDesc = static_cast<CBulbLine::BULBLINE_DESC*>(pArg);

    m_fBulbPosOffset = pDesc->fBulbPosOffset;

    m_Line.first = nullptr;
    m_Line.second = nullptr;

    return S_OK;
}

HRESULT CBulbLine::Initialize_Clone()
{
    return S_OK;
}

void CBulbLine::Priority_Update(_float fTimeDelta)
{
}

void CBulbLine::Update(_float fTimeDelta)
{
}

void CBulbLine::Late_Update(_float fTimeDelta)
{
}

void CBulbLine::Add_Point(CModelObject* _pPoint)
{
    if (nullptr == _pPoint)
        return;

    if (nullptr == m_Line.first)
        m_Line.first = _pPoint;
    else if (nullptr == m_Line.second) {
        m_Line.second = _pPoint;
        Create_Bulbs();
    }
}

void CBulbLine::Create_Bulbs()
{
    _vector vFirstPos = m_Line.first->Get_ControllerTransform()->Get_State(CTransform::STATE_POSITION);
    _vector vSecondPos = m_Line.second->Get_ControllerTransform()->Get_State(CTransform::STATE_POSITION);
    _vector vDir = vSecondPos - vFirstPos;

    _float fLength = XMVectorGetX(XMVector3Length(vDir));
    vDir = XMVector3Normalize(vDir);

    _uint iBulbCount = (_uint)(fLength / m_fBulbPosOffset);

    for (_uint i = 0; i < iBulbCount; ++i) {
        _vector vCreatePos = vFirstPos + (vDir * m_fBulbPosOffset * i);

       // Create
    }
}

void CBulbLine::Create_Bulb()
{
}

CBulbLine* CBulbLine::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* pArg)
{
    CBulbLine* pInstance = new CBulbLine(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CBulbLine");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CBulbLine* CBulbLine::Clone()
{
    CBulbLine* pInstance = new CBulbLine(*this);

    if (FAILED(pInstance->Initialize_Clone()))
    {
        MSG_BOX("Failed to Cloned : CBulbLine");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBulbLine::Free()
{
    Safe_Release(m_pGameInstance);
    Safe_Release(m_pContext);
    Safe_Release(m_pDevice);

    Safe_Release(m_Line.first);
    Safe_Release(m_Line.second);

    for (auto& Bulb : m_Bulbs) {
        Safe_Release(Bulb);
    }
	
    __super::Free();
}
