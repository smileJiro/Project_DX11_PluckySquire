#include "stdafx.h"
#include "StopStamp.h"
#include "GameInstance.h"
#include "PlayerBody.h"
#include "Player.h"
#include "Section_Manager.h"
#include "PalmMarker.h"
#include "PalmDecal.h"


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
    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;


    //도장 손바닥 자국 만들기
    CModelObject::MODELOBJECT_DESC tDecalDesc{};
    tDecalDesc.iCurLevelID = m_iCurLevelID;
    m_pPalmDecal = static_cast<CPalmDecal*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_PalmDecal"), &tDecalDesc));
	if (nullptr == m_pPalmDecal)
	{
		MSG_BOX("CPlayer PalmDecal Creation Failed");
		return E_FAIL;
	}
    if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(m_iCurLevelID, TEXT("Layer_PlayerSubs"), m_pPalmDecal)))
        return E_FAIL;  
    CSection_Manager* pSectionMgr = CSection_Manager::GetInstance();
    if (FAILED(pSectionMgr->Add_GameObject_ToSectionLayer(pSectionMgr->Get_Cur_Section_Key(), m_pPalmDecal, SECTION_2D_PLAYMAP_STAMP)))
        return E_FAIL;
	m_pPalmDecal->Set_Active(false);
    Safe_AddRef(m_pPalmDecal);
    return S_OK;
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

void CStopStamp::Place_PalmDecal(_fvector v2DPosition, _fvector _v2DDirection)
{
    m_pPalmDecal->Set_Active(true);
	m_pPalmDecal->Place(v2DPosition, _v2DDirection);
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
	Safe_Release(m_pPalmDecal);
    __super::Free();
}
