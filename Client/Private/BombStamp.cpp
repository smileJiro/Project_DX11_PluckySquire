#include "stdafx.h"
#include "BombStamp.h"
#include "GameInstance.h"
#include "PlayerBody.h"
#include "Player.h"
#include "Section_Manager.h"
#include "PlayerBomb.h"


CBombStamp::CBombStamp(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CModelObject(_pDevice, _pContext)
{
}

CBombStamp::CBombStamp(const CBombStamp& _Prototype)
	: CModelObject(_Prototype)
{
}

HRESULT CBombStamp::Initialize(void* _pArg)
{
    BOMB_STAMP_DESC* pBodyDesc = static_cast<BOMB_STAMP_DESC*>(_pArg);

    pBodyDesc->eStartCoord =COORDINATE_3D;

    pBodyDesc->strShaderPrototypeTag_3D = TEXT("Prototype_Component_Shader_VtxMesh");
    pBodyDesc->iShaderPass_3D = (_uint)PASS_VTXMESH::DEFAULT;
    pBodyDesc->tTransform2DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);
    pBodyDesc->tTransform2DDesc.vInitialScaling = _float3(1, 1, 1);
    pBodyDesc->iRenderGroupID_3D = RG_3D;
    pBodyDesc->iPriorityID_3D = PR3D_GEOMETRY;
    pBodyDesc->iModelPrototypeLevelID_2D = LEVEL_STATIC;
    pBodyDesc->iModelPrototypeLevelID_3D = LEVEL_STATIC;
    pBodyDesc->strModelPrototypeTag_3D = TEXT("Bomb_Stamp");
    
    pBodyDesc->eActorType = ACTOR_TYPE::LAST;
    pBodyDesc->pActorDesc = nullptr;
    pBodyDesc->isCoordChangeEnable = false;
    return __super::Initialize(_pArg);
}

void CBombStamp::Update(_float _fTimeDelta)
{
    __super::Update(_fTimeDelta);
}

void CBombStamp::Late_Update(_float _fTimeDelta)
{
    __super::Late_Update(_fTimeDelta);
}

HRESULT CBombStamp::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;


    //cout << "PlayerBOdyPos: " << m_WorldMatrices[COORDINATE_3D]._41 << ", " << m_WorldMatrices[COORDINATE_3D]._42 << ", " << m_WorldMatrices[COORDINATE_3D]._43 << endl;
    return S_OK;
}

IBombable* CBombStamp::Place_Bomb(_fvector v2DPosition)
{
	CModelObject::MODELOBJECT_DESC tBombDesc{};
    tBombDesc.tTransform2DDesc.vInitialPosition = _float3(XMVectorGetX(v2DPosition), XMVectorGetY(v2DPosition), 0.0f);
	tBombDesc.iCurLevelID = m_iCurLevelID;
    CPlayerBomb* pPlayerBomb = static_cast<CPlayerBomb*>( m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_PlayerBomb"), &tBombDesc));
	m_pGameInstance->Add_GameObject_ToLayer(m_iCurLevelID,TEXT("Layer_Player"), pPlayerBomb);
    CSection_Manager* pSectionMgr = CSection_Manager::GetInstance();
    pSectionMgr->Add_GameObject_ToSectionLayer(pSectionMgr->Get_Cur_Section_Key(), pPlayerBomb, SECTION_2D_PLAYMAP_OBJECT);

    return pPlayerBomb;
}

CBombStamp* CBombStamp::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CBombStamp* pInstance = new CBombStamp(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : BombStamp");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBombStamp::Clone(void* _pArg)
{
    CBombStamp* pInstance = new CBombStamp(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : BombStamp");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBombStamp::Free()
{
    __super::Free();
}
