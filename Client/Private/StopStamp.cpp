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

    pBodyDesc->eStartCoord = COORDINATE_3D;

    pBodyDesc->strShaderPrototypeTag_3D = TEXT("Prototype_Component_Shader_VtxMesh");
    pBodyDesc->iShaderPass_3D = (_uint)PASS_VTXMESH::DEFAULT;
    pBodyDesc->tTransform2DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);
    pBodyDesc->tTransform2DDesc.vInitialScaling = _float3(1, 1, 1);
    pBodyDesc->iRenderGroupID_3D = RG_3D;
    pBodyDesc->iPriorityID_3D = PR3D_GEOMETRY;
    pBodyDesc->iModelPrototypeLevelID_2D = LEVEL_STATIC;
    pBodyDesc->iModelPrototypeLevelID_3D = LEVEL_STATIC;

    pBodyDesc->eActorType = ACTOR_TYPE::LAST;
    pBodyDesc->pActorDesc = nullptr;
    pBodyDesc->isCoordChangeEnable = false;
    pBodyDesc->strModelPrototypeTag_3D = TEXT("Stop_Stamp");
    return __super::Initialize(_pArg);
}

void CStopStamp::Update(_float _fTimeDelta)
{
    __super::Update(_fTimeDelta);
}

void CStopStamp::Late_Update(_float _fTimeDelta)
{
    __super::Late_Update(_fTimeDelta);
}

HRESULT CStopStamp::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;


    //cout << "PlayerBOdyPos: " << m_WorldMatrices[COORDINATE_3D]._41 << ", " << m_WorldMatrices[COORDINATE_3D]._42 << ", " << m_WorldMatrices[COORDINATE_3D]._43 << endl;
    return S_OK;
}

void CStopStamp::Place_Stopper(_fvector v2DPosition)
{

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
