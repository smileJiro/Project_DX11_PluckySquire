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
	pDesc->tTransform3DDesc.vInitialPosition = _float3(218.18f, -45.61f, -127.13f);

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
	//if (KEY_DOWN(KEY::L))
	//	Start_Turn_Door(true);
	//if (KEY_DOWN(KEY::I))
	//	Start_Turn_DoorKnob(true);

	Turn_Door(_fTimeDelta);
	Turn_DoorKnob(_fTimeDelta);

	__super::Update(_fTimeDelta); /* Part Object Update */
}

void CRoom_Door::Late_Update(_float _fTimeDelta)
{
	__super::Late_Update(_fTimeDelta); /* Part Object Late_Update */
}

HRESULT CRoom_Door::Render()
{
	/* ModelÀÌ ¾ø´Â Container Object °°Àº °æ¿ì Debug ¿ëÀ¸·Î »ç¿ëÇÏ°Å³ª, ÆùÆ® ·»´õ¿ëÀ¸·Î. */
	return S_OK;
}

void CRoom_Door::Turn_Door(_float _fTimeDelta)
{
	if (false == m_isTurnDoor)
		return;

	_float fRatio = m_pGameInstance->Calculate_Ratio(&m_fTurnDoorTime, _fTimeDelta, EASE_IN_OUT);

	if (fRatio >= (1.f - EPSILON)) {
		m_pControllerTransform->TurnAngle(XMConvertToRadians(m_fTurnDoorAngle) - XMConvertToRadians(m_fPreAngle));
		m_fTurnDoorTime.y = 0.f;
		m_fPreAngle = 0.f;
		m_isTurnDoor = false;

		m_fTurnDoorAngle *= -1.f;

		return;
	}

	_float fAngle = m_pGameInstance->Lerp(0.f, m_fTurnDoorAngle, fRatio);

	_float fResultAngle = fAngle - m_fPreAngle;
	m_fPreAngle = fAngle;

	m_pControllerTransform->TurnAngle(XMConvertToRadians(fResultAngle));
}

void CRoom_Door::Turn_DoorKnob(_float _fTimeDelta)
{
	if (false == m_isTurnDoorKnob)
		return;

	_float fRatio = m_pGameInstance->Calculate_Ratio(&m_fTurnDoorKnobTime, _fTimeDelta, EASE_IN_OUT);

	if (fRatio >= (1.f - EPSILON)) {
		m_PartObjects[DOOR_PART_KNOB]->Get_ControllerTransform()->TurnAngle(XMConvertToRadians(m_fTurnDoorKnobAngle) - XMConvertToRadians(m_fPreKnobAngle), XMVectorSet(1.f, 0.f, 0.f, 0.f));
		m_fTurnDoorKnobTime.y = 0.f;
		m_isTurnDoorKnob = false;
		m_fPreKnobAngle = 0.f;

		m_fTurnDoorKnobAngle *= -1.f;

		return;
	}

	_float fAngle = m_pGameInstance->Lerp(0.f, m_fTurnDoorKnobAngle, fRatio);

	_float fResultAngle = fAngle - m_fPreKnobAngle;
	m_fPreKnobAngle = fAngle;

	m_PartObjects[DOOR_PART_KNOB]->Get_ControllerTransform()->TurnAngle(XMConvertToRadians(fResultAngle), XMVectorSet(1.f, 0.f, 0.f, 0.f));
}

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
	BodyDesc.tTransform3DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);
	BodyDesc.tTransform3DDesc.vInitialScaling = _float3(1, 1, 1);
	BodyDesc.tTransform3DDesc.fRotationPerSec = XMConvertToRadians(180.f);
	BodyDesc.tTransform3DDesc.fSpeedPerSec = 10.f;
	BodyDesc.iRenderGroupID_3D = RG_3D;
	BodyDesc.iPriorityID_3D = PR3D_GEOMETRY;

	m_PartObjects[DOOR_PART_DOOR] = static_cast<CRoom_Door_Body*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_Room_Door_Body"), &BodyDesc));

	/* Door_Knob */
	CModelObject::MODELOBJECT_DESC Desc{};
	Desc.eStartCoord = m_pControllerTransform->Get_CurCoord();
	Desc.iCurLevelID = m_iCurLevelID;
	Desc.isCoordChangeEnable = m_pControllerTransform->Is_CoordChangeEnable();

	Desc.iModelPrototypeLevelID_3D = LEVEL_STATIC;
	Desc.strModelPrototypeTag_3D = TEXT("SM_Doorknob");
	Desc.strShaderPrototypeTag_3D = TEXT("Prototype_Component_Shader_VtxMesh");
	Desc.iShaderPass_3D = (_uint)PASS_VTXMESH::DEFAULT;
	Desc.pParentMatrices[COORDINATE_3D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_3D);
	Desc.tTransform3DDesc.vInitialPosition = _float3(2.3f, 67.7f, -54.f);
	Desc.tTransform3DDesc.vInitialScaling = _float3(1, 1, 1);
	Desc.tTransform3DDesc.fRotationPerSec = XMConvertToRadians(180.f);
	Desc.tTransform3DDesc.fSpeedPerSec = 10.f;
	Desc.iRenderGroupID_3D = RG_3D;
	Desc.iPriorityID_3D = PR3D_GEOMETRY;

	m_PartObjects[DOOR_PART_KNOB] = static_cast<CModelObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &Desc));

	/* Door_plaue*/
	Desc.strModelPrototypeTag_3D = TEXT("door_plaque");
	Desc.tTransform3DDesc.vInitialPosition = _float3(4.f, 93.f, -30.f);
	Desc.tTransform3DDesc.vInitialRotation = _float3(0.f, XMConvertToRadians(-90.f), 0.f);
	m_PartObjects[DOOR_PART_DOORPLAQUE] = static_cast<CModelObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &Desc));

	///* Door_Sticker_Zap*/
	//Desc.strModelPrototypeTag_3D = TEXT("SM_sticker_zap");
	//Desc.tTransform3DDesc.vInitialPosition = _float3(4.f, 93.f, -30.f);
	//Desc.tTransform3DDesc.vInitialRotation = _float3(0.f, XMConvertToRadians(90.f), XMConvertToRadians(0.f));
	//m_PartObjects[DOOR_PART_STICKER_ZAP] = static_cast<CModelObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &Desc));

	/* Door_Sticker_01*/ // ¼Ò¼¼Áö
	Desc.strModelPrototypeTag_3D = TEXT("SM_stickers_01");
	Desc.tTransform3DDesc.vInitialPosition = _float3(5.f, 54.f, -40.f);
	Desc.tTransform3DDesc.vInitialRotation = _float3(0.f, XMConvertToRadians(-90.f), XMConvertToRadians(0.f));
	m_PartObjects[DOOR_PART_STICKER_01] = static_cast<CModelObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &Desc));

	/* Door_Sticker_02*/ // ÇØ°ñ
	Desc.strModelPrototypeTag_3D = TEXT("SM_stickers_02");
	Desc.tTransform3DDesc.vInitialPosition = _float3(5.f, 110.f, -15.f);
	Desc.tTransform3DDesc.vInitialRotation = _float3(0.f, XMConvertToRadians(-90.f), XMConvertToRadians(0.f));
	m_PartObjects[DOOR_PART_STICKER_02] = static_cast<CModelObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &Desc));

	/* Door_Sticker_04*/ // ÀÌ»óÇÑ ¾ÆÀú¾¾
	Desc.strModelPrototypeTag_3D = TEXT("SM_stickers_04");
	Desc.tTransform3DDesc.vInitialPosition = _float3(5.2f, 90.f, -17.f);
	Desc.tTransform3DDesc.vInitialRotation = _float3(0.f, XMConvertToRadians(-90.f), XMConvertToRadians(0.f));
	m_PartObjects[DOOR_PART_STICKER_04] = static_cast<CModelObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &Desc));

	/* Door_Sticker_05 */ // °ø·æ
	Desc.strModelPrototypeTag_3D = TEXT("SM_stickers_05");
	Desc.tTransform3DDesc.vInitialPosition = _float3(5.f, 50.f, -18.f);
	Desc.tTransform3DDesc.vInitialRotation = _float3(0.f, XMConvertToRadians(-90.f), XMConvertToRadians(0.f));
	m_PartObjects[DOOR_PART_STICKER_05] = static_cast<CModelObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &Desc));

	/* Door_Sticker_05_2 */ // °ø·æ
	Desc.strModelPrototypeTag_3D = TEXT("SM_stickers_05");
	Desc.tTransform3DDesc.vInitialPosition = _float3(5.f, 96.f, -23.f);
	Desc.tTransform3DDesc.vInitialRotation = _float3(0.f, XMConvertToRadians(-90.f), XMConvertToRadians(-10.f));
	m_PartObjects[DOOR_PART_STICKER_05_2] = static_cast<CModelObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &Desc));

	/* Door_Sticker_07 */ // Hey
	Desc.strModelPrototypeTag_3D = TEXT("SM_stickers_07");
	Desc.tTransform3DDesc.vInitialPosition = _float3(5.f, 55.f, -20.f);
	Desc.tTransform3DDesc.vInitialRotation = _float3(0.f, XMConvertToRadians(-90.f), XMConvertToRadians(-7.f));
	m_PartObjects[DOOR_PART_STICKER_07] = static_cast<CModelObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &Desc));

	/* Door_Sticker_09 */ // µÅÁö
	Desc.strModelPrototypeTag_3D = TEXT("SM_stickers_09");
	Desc.tTransform3DDesc.vInitialPosition = _float3(4.f, 30.f, -34.f);
	Desc.tTransform3DDesc.vInitialRotation = _float3(0.f, XMConvertToRadians(-90.f), XMConvertToRadians(-6.f));
	m_PartObjects[DOOR_PART_STICKER_09] = static_cast<CModelObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &Desc));


	/* Door_Sticker_09 */ // µÅÁö
	Desc.strModelPrototypeTag_3D = TEXT("SM_stickers_09");
	Desc.tTransform3DDesc.vInitialPosition = _float3(5.f, 105.f, -34.f);
	Desc.tTransform3DDesc.vInitialRotation = _float3(0.f, XMConvertToRadians(-90.f), XMConvertToRadians(-6.f));
	Desc.tTransform3DDesc.vInitialScaling = _float3(0.3f, 0.3f, 0.3f);
	m_PartObjects[DOOR_PART_STICKER_09_2] = static_cast<CModelObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &Desc));


	/* Door_Sticker_09 */ // µÅÁö
	Desc.strModelPrototypeTag_3D = TEXT("SM_stickers_09");
	Desc.tTransform3DDesc.vInitialPosition = _float3(5.f, 106.f, -30.f);
	Desc.tTransform3DDesc.vInitialRotation = _float3(0.f, XMConvertToRadians(-90.f), XMConvertToRadians(-3.f));
	Desc.tTransform3DDesc.vInitialScaling = _float3(0.3f, 0.3f, 0.3f);
	m_PartObjects[DOOR_PART_STICKER_09_3] = static_cast<CModelObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &Desc));

	/* Door_Sticker_09 */ // µÅÁö
	Desc.strModelPrototypeTag_3D = TEXT("SM_stickers_09");
	Desc.tTransform3DDesc.vInitialPosition = _float3(5.f, 104.5f, -26.f);
	Desc.tTransform3DDesc.vInitialRotation = _float3(0.f, XMConvertToRadians(-90.f), XMConvertToRadians(6.f));
	Desc.tTransform3DDesc.vInitialScaling = _float3(0.3f, 0.3f, 0.3f);
	m_PartObjects[DOOR_PART_STICKER_09_4] = static_cast<CModelObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &Desc));

	/* Door_Sticker_10 */ // ·ÎÄÏ
	Desc.strModelPrototypeTag_3D = TEXT("SM_stickers_10");
	Desc.tTransform3DDesc.vInitialPosition = _float3(5.f, 95.f, -39.f);
	Desc.tTransform3DDesc.vInitialRotation = _float3(0.f, XMConvertToRadians(-90.f), XMConvertToRadians(-45.f));
	Desc.tTransform3DDesc.vInitialScaling = _float3(1.f, 1.f, 1.f);
	m_PartObjects[DOOR_PART_STICKER_10] = static_cast<CModelObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &Desc));
	
	/* Door_Sticker_11 */ // ÇÒ¹è
	Desc.strModelPrototypeTag_3D = TEXT("SM_stickers_11");
	Desc.tTransform3DDesc.vInitialPosition = _float3(5.f, 90.f, -43.f);
	Desc.tTransform3DDesc.vInitialRotation = _float3(0.f, XMConvertToRadians(-90.f), XMConvertToRadians(0.f));
	Desc.tTransform3DDesc.vInitialScaling = _float3(0.45f, 0.45f, 0.45f);
	m_PartObjects[DOOR_PART_STICKER_11] = static_cast<CModelObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &Desc));

	/* Door_Sticker_12 */
	//Desc.strModelPrototypeTag_3D = TEXT("SM_stickers_12");
	//Desc.tTransform3DDesc.vInitialPosition = _float3(4.f, 93.f, -30.f);
	//Desc.tTransform3DDesc.vInitialRotation = _float3(0.f, XMConvertToRadians(90.f), XMConvertToRadians(0.f));
	//m_PartObjects[DOOR_PART_STICKER_12] = static_cast<CModelObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &Desc));

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
