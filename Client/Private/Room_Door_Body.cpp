#include "stdafx.h"
#include "Room_Door_Body.h"

CRoom_Door_Body::CRoom_Door_Body(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CModelObject(_pDevice, _pContext)
{
}

CRoom_Door_Body::CRoom_Door_Body(const CRoom_Door_Body& _Prototype)
    : CModelObject(_Prototype)
{
}

HRESULT CRoom_Door_Body::Initialize(void* _pArg)
{
    ROOM_DOOR_BODY_DESC* pBodyDesc = static_cast<ROOM_DOOR_BODY_DESC*>(_pArg);
    m_pRoomDoor = pBodyDesc->pRoomDoor;
    return __super::Initialize(_pArg);
}

void CRoom_Door_Body::Update(_float _fTimeDelta)
{
    __super::Update(_fTimeDelta);
}


void CRoom_Door_Body::Late_Update(_float _fTimeDelta)
{
    if (COORDINATE_3D == Get_CurCoord())
    {
        if (false == m_isFrustumCulling)
            Register_RenderGroup(m_iRenderGroupID_3D, m_iPriorityID_3D);
    }

    __super::Late_Update(_fTimeDelta);
}

HRESULT CRoom_Door_Body::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

CModelObject* CRoom_Door_Body::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CRoom_Door_Body* pInstance = new CRoom_Door_Body(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CModelObject");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CRoom_Door_Body::Clone(void* _pArg)
{
    CRoom_Door_Body* pInstance = new CRoom_Door_Body(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CRoom_Door_Body");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CRoom_Door_Body::Free()
{
    __super::Free();
}
