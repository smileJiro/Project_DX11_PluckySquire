#include "stdafx.h"
#include "3DMapObject.h"
#include "GameInstance.h"


C3DMapObject::C3DMapObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CMapObject(_pDevice, _pContext)
{
}

C3DMapObject::C3DMapObject(const C3DMapObject& _Prototype)
    :CMapObject(_Prototype)
{
}

HRESULT C3DMapObject::Initialize_Prototype()
{
    return S_OK;
}

/// <summary>
/// 모델을 확인하고 셰잎 쿠킹 정보가 있을 경우에 생성하므로 MapObject::Initialize를 타지 않는다.
/// </summary>
/// <param name="_pArg">MAPOBJ_3D_DESC</param>
HRESULT C3DMapObject::Initialize(void* _pArg)
{
    MAPOBJ_3D_DESC* pDesc = static_cast<MAPOBJ_3D_DESC*>(_pArg);
    if (nullptr == _pArg)
        return E_FAIL;


    m_isCulling = pDesc->isCulling;
    #pragma region CModelObject::Initialize(pArg); 재구현부
        // Save 
        m_iShaderPasses[COORDINATE_2D] = pDesc->iShaderPass_2D;
        m_iShaderPasses[COORDINATE_3D] = pDesc->iShaderPass_3D;
        m_strModelPrototypeTag[COORDINATE_2D] = pDesc->strModelPrototypeTag_2D;
        m_strModelPrototypeTag[COORDINATE_3D] = pDesc->strModelPrototypeTag_3D;
        m_fFrustumCullingRange = pDesc->fFrustumCullingRange;

        m_iRenderGroupID_3D = pDesc->iRenderGroupID_3D;
        m_iPriorityID_3D = pDesc->iPriorityID_3D;

        if (FAILED(CModelObject::Ready_Components(pDesc)))
            return E_FAIL;
    #pragma endregion

    // 구조체는 if문 내부에 넣어서 포인터로 던지면, 사라지므로. 밖에 선언
    CActor::ACTOR_DESC ActorDesc;
    SHAPE_COOKING_DESC ShapeCookingDesc = {};
    SHAPE_DATA ShapeData;

    // 모델에 쿠킹 셰잎 정보가 있으면, 액터 정보를 ModelDesc에 포함시킨다.

    #pragma region CActorObject::Initialize(pArg) 준비작업 
        if (nullptr != m_pControllerModel)
        {
            CModel* pModel = m_pControllerModel->Get_Model(COORDINATE_3D);

            if (nullptr != pModel)
            {
                if (static_cast<C3DModel*>(pModel)->Has_CookingCollider())
                {

                    MAPOBJ_DESC* pDesc = static_cast<MAPOBJ_DESC*>(_pArg);

                    pDesc->eActorType = ACTOR_TYPE::STATIC;

                    ActorDesc.pOwner = this;

                    ActorDesc.FreezeRotation_XYZ[0] = true;
                    ActorDesc.FreezeRotation_XYZ[1] = true;
                    ActorDesc.FreezeRotation_XYZ[2] = true;

                    ActorDesc.FreezePosition_XYZ[0] = false;
                    ActorDesc.FreezePosition_XYZ[1] = false;
                    ActorDesc.FreezePosition_XYZ[2] = false;

                    ShapeCookingDesc.isLoad = true;
                    ShapeCookingDesc.isSave = false;
                    ShapeData.eShapeType = SHAPE_TYPE::COOKING;
                    ShapeData.eMaterial = ACTOR_MATERIAL::DEFAULT;
                    ShapeData.pShapeDesc = &ShapeCookingDesc;
                    ShapeData.isTrigger = false;
                    _float3 fScale =
                        _float3(XMVectorGetX(XMVector3Length(XMLoadFloat3((_float3*)&pDesc->tTransform3DDesc.matWorld.m[0]))),
                            XMVectorGetX(XMVector3Length(XMLoadFloat3((_float3*)&pDesc->tTransform3DDesc.matWorld.m[1]))),
                            XMVectorGetX(XMVector3Length(XMLoadFloat3((_float3*)&pDesc->tTransform3DDesc.matWorld.m[2]))));
                    _matrix matScale = XMMatrixScaling(fScale.x, fScale.y, fScale.z);
                    XMStoreFloat4x4(&ShapeData.LocalOffsetMatrix, matScale);
                    ActorDesc.ShapeDatas.push_back(ShapeData);
                    pDesc->pActorDesc = &ActorDesc;

                    ActorDesc.tFilterData.MyGroup = OBJECT_GROUP::MAPOBJECT;
                    ActorDesc.tFilterData.OtherGroupMask = OBJECT_GROUP::PLAYER | OBJECT_GROUP::MONSTER | OBJECT_GROUP::INTERACTION_OBEJCT;
                }
            }
        }
    #pragma endregion

    // ModelObject Initialize를 건너뛰고, 작업한다. 
    if (FAILED(CPartObject::Initialize(_pArg)))
        return E_FAIL;

    return S_OK;
}

void C3DMapObject::Priority_Update(_float _fTimeDelta)
{
    CPartObject::Priority_Update(_fTimeDelta);
}
void C3DMapObject::Update(_float _fTimeDelta)
{
    CModelObject::Update(_fTimeDelta);
}

/// <summary>
/// MapObject-ModelObject LateUpdate를 건너뛴다. 컬링 여부를 확인하고 RenderGroup에 집어넣는다.
/// </summary>
/// <param name="_fTimeDelta">시간</param>
void C3DMapObject::Late_Update(_float _fTimeDelta)
{
    CGameObject::Late_Update_Component(_fTimeDelta);

        /* Add Render Group */
        if (!m_isCulling || true == m_pGameInstance->isIn_Frustum_InWorldSpace(Get_FinalPosition(), 10.0f))
        {
            Register_RenderGroup(RENDERGROUP::RG_3D, PRIORITY_3D::PR3D_GEOMETRY);
            //if(nullptr != m_pActorCom)
            //    m_pActorCom->Set_ShapeRayCastFlag(true);
        }
        else
        {
            //if (nullptr != m_pActorCom)
            //    m_pActorCom->Set_ShapeRayCastFlag(false);
        }
}

/// <summary>
/// 개선 필요한 렌더 코드. 디퓨즈 없이 기본 컬러값으로 칠할지 여부를 확인하고, 매번 3D Shader 패스를 다르게 설정한다.
/// </summary>
/// <returns>렌더 성공 여부</returns>
HRESULT C3DMapObject::Render()
{
    switch (m_eColorShaderMode)
    {
        case Engine::C3DModel::COLOR_DEFAULT:
            if (FAILED(m_pShaderComs[COORDINATE_3D]->Bind_RawValue("g_vDefaultDiffuseColor", &m_fDefaultDiffuseColor, sizeof(_float4))))
                return E_FAIL;
            m_iShaderPasses[COORDINATE_3D] = 3;
            break;
        case Engine::C3DModel::MIX_DIFFUSE:
            if (FAILED(m_pShaderComs[COORDINATE_3D]->Bind_RawValue("g_vDefaultDiffuseColor", &m_fDefaultDiffuseColor, sizeof(_float4))))
                return E_FAIL;
            m_iShaderPasses[COORDINATE_3D] = 4;
            break;
        default:
            m_iShaderPasses[COORDINATE_3D] = 0;
            break;
    }
    return CModelObject::Render();
}

HRESULT C3DMapObject::Render_Shadow()
{
    return S_OK;
}



void C3DMapObject::Free()
{
    __super::Free();
}
