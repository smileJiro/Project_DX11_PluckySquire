#include "stdafx.h"
#include "GameInstance.h"
#include "PlayerBody.h"
#include "Player.h"
#include "CarriableObject.h"

CPlayerBody::CPlayerBody(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CModelObject(_pDevice, _pContext)
{
}


CPlayerBody::CPlayerBody(const CPlayerBody& _Prototype)
	: CModelObject(_Prototype)
{
}

HRESULT CPlayerBody::Initialize(void* _pArg)
{
    PLAYER_BODY_DESC* pBodyDesc = static_cast<PLAYER_BODY_DESC*>(_pArg);
    m_pPlayer = pBodyDesc->pPlayer;
    return __super::Initialize(_pArg);
}

void CPlayerBody::Update(_float _fTimeDelta)
{
    __super::Update(_fTimeDelta);
}

void CPlayerBody::Late_Update(_float _fTimeDelta)
{
    __super::Late_Update(_fTimeDelta);
}

HRESULT CPlayerBody::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;
    if (COORDINATE_2D == Get_CurCoord())
    {
        CCarriableObject* pCarriable = m_pPlayer->Get_CarryingObject();
        if (pCarriable)
            pCarriable->Render();
    }

    //cout << "PlayerBOdyPos: " << m_WorldMatrices[COORDINATE_3D]._41 << ", " << m_WorldMatrices[COORDINATE_3D]._42 << ", " << m_WorldMatrices[COORDINATE_3D]._43 << endl;
    return S_OK;
}

CModelObject* CPlayerBody::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CPlayerBody* pInstance = new CPlayerBody(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : PlayerBody");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CPlayerBody::Clone(void* _pArg)
{
    CPlayerBody* pInstance = new CPlayerBody(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : PlayerBody");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPlayerBody::Free()
{
	__super::Free();
}
