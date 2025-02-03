#include "stdafx.h"
#include "MainTable.h"
#include "GameInstance.h"

CMainTable::CMainTable(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CActorObject(_pDevice, _pContext)
{
}

CMainTable::CMainTable(const CMainTable& _Prototype)
	:CActorObject(_Prototype)
{
}

HRESULT CMainTable::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMainTable::Initialize(void* _pArg)
{

    CMainTable::ACTOROBJECT_DESC* pDesc = static_cast<CMainTable::ACTOROBJECT_DESC*>(_pArg);

    pDesc->eStartCoord = COORDINATE_3D;
    pDesc->isCoordChangeEnable = false;
    pDesc->tTransform3DDesc.vInitialPosition = { 0.0f, -9.5f, 0.0f };
    /* Create Test Actor (Desc를 채우는 함수니까. __super::Initialize() 전에 위치해야함. )*/
    pDesc->eActorType = ACTOR_TYPE::STATIC;
    CActor::ACTOR_DESC ActorDesc;
    /* Actor의 주인 오브젝트 포인터 */
    ActorDesc.pOwner = this;

    /* 사용하려는 Shape의 형태를 정의 */
    SHAPE_BOX_DESC ShapeDesc = {};
    ShapeDesc.vHalfExtents = { 85.f, 10.f, 34.f };

    /* 해당 Shape의 Flag에 대한 Data 정의 */
    SHAPE_DATA ShapeData;
    ShapeData.pShapeDesc = &ShapeDesc;             
    ShapeData.eShapeType = SHAPE_TYPE::BOX;     
    ShapeData.eMaterial = ACTOR_MATERIAL::DEFAULT;  
    ShapeData.isSceneQuery = false;
    //ShapeData.isVisual = false;
    /* 최종으로 결정 된 ShapeData를 PushBack */
    ActorDesc.ShapeDatas.push_back(ShapeData);

    /* 충돌 필터에 대한 세팅 ()*/
    ActorDesc.tFilterData.MyGroup = OBJECT_GROUP::MAPOBJECT;
    ActorDesc.tFilterData.OtherGroupMask = OBJECT_GROUP::PLAYER | OBJECT_GROUP::MONSTER | OBJECT_GROUP::INTERACTION_OBEJCT | OBJECT_GROUP::MONSTER_PROJECTILE | OBJECT_GROUP::PLAYER_PROJECTILE;

    /* Actor Component Finished */
    pDesc->pActorDesc = &ActorDesc;

	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;


	return S_OK;
}

CMainTable* CMainTable::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CMainTable* pInstance = new CMainTable(_pDevice, _pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMainTable");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMainTable::Clone(void* _pArg)
{
	CMainTable* pInstance = new CMainTable(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CMainTable");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMainTable::Free()
{

	__super::Free();
}
