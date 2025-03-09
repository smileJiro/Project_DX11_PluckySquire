#include "stdafx.h"
#include "Room_Door.h"
#include "GameInstance.h"
#include "Layer.h"

#include "Room_Door_Body.h"

CRoom_Door::CRoom_Door(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CContainerObject(_pDevice, _pContext)
{
}

CRoom_Door::CRoom_Door(const CRoom_Door& _Prototype)
    : CContainerObject(_Prototype)
{
}

HRESULT CRoom_Door::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CRoom_Door::Initialize(void* _pArg)
{
	if (nullptr == _pArg)
		return E_FAIL;

	CContainerObject::CONTAINEROBJ_DESC* pDesc = static_cast<CONTAINEROBJ_DESC*>(_pArg);

	m_iCurLevelID = pDesc->iCurLevelID;

	pDesc->iNumPartObjects = DOOR_PART_END;
	pDesc->eStartCoord = COORDINATE_3D;
	pDesc->isCoordChangeEnable = false;
	pDesc->tTransform3DDesc.fSpeedPerSec = 10.f;
	pDesc->tTransform3DDesc.fRotationPerSec = XMConvertToRadians(180.f);
	pDesc->tTransform2DDesc.vInitialPosition = _float3(218.18f, -45.61f, -127.13f);

	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	if (FAILED(Ready_PartObjects()))
		return E_FAIL;

	if (FAILED(Ready_Components()))

		return E_FAIL;
    return S_OK;
}

void CRoom_Door::Priority_Update(_float _fTimeDelta)
{
	__super::Priority_Update(_fTimeDelta); /* Part Object Priority_Update */
}


void CRoom_Door::Update(_float _fTimeDelta)
{
	__super::Update(_fTimeDelta); /* Part Object Update */
}

void CRoom_Door::Late_Update(_float _fTimeDelta)
{
	__super::Late_Update(_fTimeDelta); /* Part Object Late_Update */
}

HRESULT CRoom_Door::Render()
{
	/* Model이 없는 Container Object 같은 경우 Debug 용으로 사용하거나, 폰트 렌더용으로. */
	return S_OK;
}

//void CRoom_Door::Start_Turn_Knob()
//{
//	if (false == m_isTurnKnob)
//		return;
//
//	CLayer* pLayer = m_pGameInstance->Find_Layer(m_iCurLevelID, TEXT("Layer_Door"));
//	const list<CGameObject*>& GameObjects = pLayer->Get_GameObjects();
//	
//	/*for (auto& Object : GameObjects) {
//		if(Object->Get_Name())
//	}*/
//}
//
//void CRoom_Door::Start_Turn_Door()
//{
//	if (false == m_isTurnDoor)
//		return;
//}

HRESULT CRoom_Door::Ready_Components()
{
	return S_OK;
}

HRESULT CRoom_Door::Ready_PartObjects()
{
	/* Door_Body */

	CRoom_Door_Body::ROOM_DOOR_BODY_DESC BodyDesc{};
	BodyDesc.pRoomDoor = this;
	BodyDesc.eStartCoord = m_pControllerTransform->Get_CurCoord();
	BodyDesc.iCurLevelID = m_iCurLevelID;
	BodyDesc.isCoordChangeEnable = m_pControllerTransform->Is_CoordChangeEnable();

	BodyDesc.iModelPrototypeLevelID_3D = LEVEL_STATIC;
	BodyDesc.strModelPrototypeTag_3D = TEXT("SM_door");
	BodyDesc.strShaderPrototypeTag_3D = TEXT("Prototype_Component_Shader_VtxMesh");
	BodyDesc.iShaderPass_3D = (_uint)PASS_VTXMESH::DEFAULT;
	BodyDesc.pParentMatrices[COORDINATE_3D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_3D);
	BodyDesc.tTransform2DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);
	BodyDesc.tTransform2DDesc.vInitialScaling = _float3(1, 1, 1);
	BodyDesc.tTransform2DDesc.fRotationPerSec = XMConvertToRadians(180.f);
	BodyDesc.tTransform2DDesc.fSpeedPerSec = 10.f;
	BodyDesc.iRenderGroupID_3D = RG_3D;
	BodyDesc.iPriorityID_3D = PR3D_GEOMETRY;

	m_PartObjects[PART_BODY] = static_cast<CRoom_Door_Body*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_Room_Door_Body"), &BodyDesc));


	return S_OK;
}

CRoom_Door* CRoom_Door::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CRoom_Door* pInstance = new CRoom_Door(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CRoom_Door");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CRoom_Door::Clone(void* _pArg)
{
	CRoom_Door* pInstance = new CRoom_Door(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CRoom_Door");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRoom_Door::Free()
{

	__super::Free();
}
