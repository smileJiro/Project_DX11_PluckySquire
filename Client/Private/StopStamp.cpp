#include "stdafx.h"
#include "StopStamp.h"
#include "GameInstance.h"
#include "PlayerBody.h"
#include "Player.h"


CStopStamp::CStopStamp(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CModelObject(_pDevice, _pContext)
{
}

CStopStamp::CStopStamp(const CStopStamp& _Prototype)
	: CModelObject(_Prototype)
{
}

HRESULT CStopStamp::Initialize(void* _pArg)
{
    STOP_STAMP_DESC* pBodyDesc = static_cast<STOP_STAMP_DESC*>(_pArg);

    return __super::Initialize(_pArg);
}

void CStopStamp::Update(_float _fTimeDelta)
{
    __super::Update(_fTimeDelta);
}

void CStopStamp::Late_Update(_float _fTimeDelta)
{
    if (COORDINATE_3D == Get_CurCoord())
    {
        if (false == m_isFrustumCulling)
            m_pGameInstance->Add_RenderObject_New(m_iRenderGroupID_3D, PR3D_PLAYERDEPTH, this);
    }

    __super::Late_Update(_fTimeDelta);
}

HRESULT CStopStamp::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;


    //cout << "PlayerBOdyPos: " << m_WorldMatrices[COORDINATE_3D]._41 << ", " << m_WorldMatrices[COORDINATE_3D]._42 << ", " << m_WorldMatrices[COORDINATE_3D]._43 << endl;
    return S_OK;
}

CStopStamp* CStopStamp::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CStopStamp* pInstance = new CStopStamp(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : StopStamp");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CStopStamp::Clone(void* _pArg)
{
    CStopStamp* pInstance = new CStopStamp(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : StopStamp");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CStopStamp::Free()
{
    __super::Free();
}
