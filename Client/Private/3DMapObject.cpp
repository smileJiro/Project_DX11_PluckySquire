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

    pDesc->iObjectGroupID = OBJECT_GROUP::MAPOBJECT;
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
    vector <SHAPE_COOKING_DESC> vecShapeCookingDesc = {};
    vector<SHAPE_DATA>  vecShapeData;

    // 모델에 쿠킹 셰잎 정보가 있으면, 액터 정보를 ModelDesc에 포함시킨다.

    #pragma region CActorObject::Initialize(pArg) 준비작업 
        if (nullptr != m_pControllerModel && LEVEL_STATIC != pDesc->iModelPrototypeLevelID_3D)
        {
            CModel* pModel = m_pControllerModel->Get_Model(COORDINATE_3D);

            if (nullptr != pModel)
            {
                Engine::C3DModel* p3DModel = static_cast<Engine::C3DModel*>(pModel);
                if (p3DModel->Has_CookingCollider())
                {
                    _uint iColliderType = p3DModel->Get_CookingColliderType();
                    MAPOBJ_DESC* pDesc = static_cast<MAPOBJ_DESC*>(_pArg);

                    pDesc->eActorType = ACTOR_TYPE::STATIC;

                    ActorDesc.pOwner = this;

                    ActorDesc.FreezeRotation_XYZ[0] = true;
                    ActorDesc.FreezeRotation_XYZ[1] = true;
                    ActorDesc.FreezeRotation_XYZ[2] = true;

                    ActorDesc.FreezePosition_XYZ[0] = false;
                    ActorDesc.FreezePosition_XYZ[1] = false;
                    ActorDesc.FreezePosition_XYZ[2] = false;
                    ActorDesc.isAddActorToScene = true;



                    _float3 fScale =
                        _float3(XMVectorGetX(XMVector3Length(XMLoadFloat3((_float3*)&pDesc->tTransform3DDesc.matWorld.m[0]))),
                            XMVectorGetX(XMVector3Length(XMLoadFloat3((_float3*)&pDesc->tTransform3DDesc.matWorld.m[1]))),
                            XMVectorGetX(XMVector3Length(XMLoadFloat3((_float3*)&pDesc->tTransform3DDesc.matWorld.m[2]))));
                    _matrix matScale = XMMatrixScaling(fScale.x, fScale.y, fScale.z);

                    switch (iColliderType)
                    {
                    case 0:
                    case 1:
                    {
                        vecShapeData.resize(1);
                        vecShapeCookingDesc.resize(1);
                        vecShapeData[0].eShapeType = SHAPE_TYPE::COOKING;
                        vecShapeData[0].eMaterial = ACTOR_MATERIAL::DEFAULT;
                        vecShapeData[0].pShapeDesc = &vecShapeCookingDesc[0];
                        vecShapeData[0].isTrigger = false;
                        vecShapeData[0].isVisual = false;
                        XMStoreFloat4x4(&vecShapeData[0].LocalOffsetMatrix, matScale);
                        break;
                    }
                    case 2:
                    case 3:
                    {
                        _uint iMeshSize = (_uint)p3DModel->Get_Meshes().size();
						vecShapeData.resize(iMeshSize);
                        vecShapeCookingDesc.resize(iMeshSize);
						SHAPE_DATA tempShapeData = {};
						tempShapeData.eShapeType = SHAPE_TYPE::COOKING;
						tempShapeData.eMaterial = ACTOR_MATERIAL::DEFAULT;
						tempShapeData.isTrigger = false;
						tempShapeData.isVisual = false;
                        XMStoreFloat4x4(&tempShapeData.LocalOffsetMatrix, matScale);

                        fill(vecShapeData.begin(), vecShapeData.end(), tempShapeData);
                        _uint iMeshIndex = 0;
                        for (_uint i = 0; i < iMeshSize; ++i) 
                        {
                            vecShapeCookingDesc[i].iShapeIndex = i;
							vecShapeData[i].pShapeDesc = &vecShapeCookingDesc[i];
                        }
						break;
                    }
                    default:
                        break;
                    }

                    ActorDesc.ShapeDatas.insert(ActorDesc.ShapeDatas.end(), vecShapeData.begin(), vecShapeData.end());


                 
                    pDesc->pActorDesc = &ActorDesc;

                    ActorDesc.tFilterData.MyGroup = OBJECT_GROUP::MAPOBJECT;
                    ActorDesc.tFilterData.OtherGroupMask = OBJECT_GROUP::PLAYER | OBJECT_GROUP::MONSTER | OBJECT_GROUP::DYNAMIC_OBJECT | OBJECT_GROUP::MONSTER_PROJECTILE;
                }
            }
        }
    #pragma endregion

#pragma region CameraTool Picking용 효림
#ifdef _DEBUG
        if (LEVEL_CAMERA_TOOL == pDesc->iCurLevelID) {
            CRay::RAY_DESC RayDesc = {};
            RayDesc.fViewportWidth = (_float)g_iWinSizeX;
            RayDesc.fViewportHeight = (_float)g_iWinSizeY;

            if (FAILED(Add_Component(LEVEL_CAMERA_TOOL, L"Prototype_Component_Ray",
                TEXT("Com_Ray"), reinterpret_cast<CComponent**>(&m_pRayCom), &RayDesc)))
                return E_FAIL;
        }
#endif
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

    Register_RenderGroup(REDNERGROUP_SHADOWID, 0);
    if (false == m_isCulling || false == m_isFrustumCulling)
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
/// 렌더
/// </summary>
/// <returns>렌더 성공 여부</returns>
HRESULT C3DMapObject::Render()
{
    return CModelObject::Render();
}

void C3DMapObject::Set_MaterialConstBuffer_Albedo(_uint _iMaterialIndex, C3DModel::COLOR_SHADER_MODE _eColorMode, _float4 _fAlbedoColor)
{
    CModel* pModel = m_pControllerModel->Get_Model(COORDINATE_3D);
    if (nullptr == pModel)
        return;
    
    if (pModel)
    {
        C3DModel* p3DModel = static_cast<C3DModel*>(pModel);

        switch (_eColorMode)
        {
        case Engine::C3DModel::COLOR_DEFAULT:
            p3DModel->Set_MaterialConstBuffer_UseAlbedoMap(_iMaterialIndex, false, false);
            p3DModel->Set_MaterialConstBuffer_Albedo(_iMaterialIndex, _fAlbedoColor, true);
            break;
        case Engine::C3DModel::MIX_DIFFUSE:
            p3DModel->Set_MaterialConstBuffer_UseAlbedoMap(_iMaterialIndex, true, false);
            p3DModel->Set_MaterialConstBuffer_MultipleAlbedo(_iMaterialIndex, _fAlbedoColor, true);
            break;
        default:
            break;
        }
    }
}

void C3DMapObject::Free()
{
    __super::Free();
}
