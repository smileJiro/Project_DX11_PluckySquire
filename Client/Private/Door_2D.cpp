#include "stdafx.h"
#include "Door_2D.h"
#include "GameInstance.h"
#include "Section_Manager.h"

CDoor_2D::CDoor_2D(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CModelObject(_pDevice, _pContext)
{
}

CDoor_2D::CDoor_2D(const CDoor_2D& _Prototype)
    : CModelObject(_Prototype)
{
}

HRESULT CDoor_2D::Initialize(void* _pArg)
{
    if (nullptr == _pArg)
        return E_FAIL;

    DOOR_2D_DESC* pBodyDesc = static_cast<DOOR_2D_DESC*>(_pArg);

    pBodyDesc->eStartCoord = COORDINATE_2D;
    pBodyDesc->isCoordChangeEnable = false;
    pBodyDesc->strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
    pBodyDesc->iShaderPass_2D = (_uint)PASS_VTXPOSTEX::SPRITE2D;

    pBodyDesc->tTransform2DDesc.vInitialScaling = _float3(1.f, 1.f, 1.f);

    pBodyDesc->eActorType = ACTOR_TYPE::LAST;
    pBodyDesc->pActorDesc = nullptr;
    pBodyDesc->iModelPrototypeLevelID_2D = LEVEL_STATIC;
    pBodyDesc->strModelPrototypeTag_2D = TEXT("Door2D");

    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    m_eDoorSize = pBodyDesc->eSize;
    m_isHorizontal = pBodyDesc->isHorizontal;
    m_eDoorState = pBodyDesc->eInitialState;

    m_p2DColliderComs.resize(1);
    ///* Test 2D Collider */
    CCollider_AABB::COLLIDER_AABB_DESC AABBDESC = {};

    AABBDESC.pOwner = this;
    AABBDESC.vScale = { 1.0f, 1.0f };
    AABBDESC.isBlock = true;
    AABBDESC.isTrigger = false;
    AABBDESC.iCollisionGroupID = OBJECT_GROUP::DOOR;
  
    if (m_isHorizontal)
    {
        if (LARGE == m_eDoorSize)
        {
            AABBDESC.vExtents = _float2(150.f, 40.f);
            AABBDESC.vOffsetPosition = { 0.f, 40.f };
        }
        else if (MED == m_eDoorSize)
        {
            AABBDESC.vExtents = _float2(115.f, 40.f);
            AABBDESC.vOffsetPosition = { 0.f, 40.f };
        }
        else
        {
            AABBDESC.vExtents = _float2(80.f, 40.f);
            AABBDESC.vOffsetPosition = { 0.f, 40.f };
        }
    }
    else
    {
        if (LARGE == m_eDoorSize)
            AABBDESC.vExtents = _float2(200.f, 200.f); // TEMP
        else if (MED == m_eDoorSize)
            AABBDESC.vExtents = _float2(200.f, 200.f); // TEMP
        else
            AABBDESC.vExtents = _float2(200.f, 200.f); // TEMP
    }

    if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
        TEXT("Com_Body2DCollider"), reinterpret_cast<CComponent**>(&m_p2DColliderComs[0]), &AABBDESC)))
        return E_FAIL;

    m_p2DColliderComs[0]->Set_Active(true);
    

    //Switch_Animation(H_LARGE_RED_CLOSE);
    if (FAILED(CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(pBodyDesc->strSectionTag, this)))
        return E_FAIL;


    return S_OK;
}

HRESULT CDoor_2D::Render()
{
    __super::Render();
#ifdef _DEBUG
    m_strSectionName = SECTION_MGR->Get_Cur_Section_Key();
    for (auto& p2DCollider : m_p2DColliderComs)
    {
        p2DCollider->Render(SECTION_MGR->Get_Section_RenderTarget_Size(m_strSectionName));
    }
#endif // _DEBUG

    return S_OK;
}



void CDoor_2D::Free()
{
    __super::Free();
}
