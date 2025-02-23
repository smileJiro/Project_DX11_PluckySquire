#include "stdafx.h"
#include "GameInstance.h"
#include "PlayerBody.h"

CPlayerBody::CPlayerBody(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CModelObject(_pDevice, _pContext)
{
}


CPlayerBody::CPlayerBody(const CPlayerBody& _Prototype)
	: CModelObject(_Prototype)
{
}

void CPlayerBody::Update(_float _fTimeDelta)
{
    __super::Update(_fTimeDelta);
}

void CPlayerBody::Late_Update(_float _fTimeDelta)
{
    if (COORDINATE_3D == Get_CurCoord())
    {
        if (false == m_isFrustumCulling)
            m_pGameInstance->Add_RenderObject_New(m_iRenderGroupID_3D, PR3D_PLAYERDEPTH, this);
    }

    __super::Late_Update(_fTimeDelta);
}

HRESULT CPlayerBody::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;
    cout << "PlayerBOdyPos: " << m_WorldMatrices[COORDINATE_2D]._41 << ", " << m_WorldMatrices[COORDINATE_2D]._42 << ", " << m_WorldMatrices[COORDINATE_2D]._43 << endl;
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
