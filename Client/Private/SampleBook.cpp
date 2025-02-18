#include "stdafx.h"
#include "SampleBook.h"
#include "GameInstance.h"
#include "Controller_Model.h"
#include "3DModel.h"
#include "Section_Manager.h"
#include "AnimEventGenerator.h"
#include "RenderGroup_WorldPos.h"

#include "Camera_Manager.h"
#include "Camera_2D.h"
#include "Player.h"
#include "CarriableObject.h"


CSampleBook::CSampleBook(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CModelObject(_pDevice, _pContext)
{
}

CSampleBook::CSampleBook(const CSampleBook& _Prototype)
	: CModelObject(_Prototype)
{
}


HRESULT CSampleBook::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	return S_OK;
}

HRESULT CSampleBook::Initialize(void* _pArg)
{

	MODELOBJECT_DESC* pDesc = static_cast<MODELOBJECT_DESC*>(_pArg);

	pDesc->Build_3D_Model(pDesc->iCurLevelID,
		L"book",
		L"Prototype_Component_Shader_VtxAnimMesh",
		(_uint)PASS_VTXANIMMESH::RENDERTARGET_MAPP
	);
	pDesc->tTransform3DDesc.vInitialPosition = _float3(2.f, 0.4f, -17.3f);
	pDesc->tTransform3DDesc.vInitialScaling = _float3(1.0f, 1.0f, 1.0f);
	pDesc->tTransform3DDesc.fRotationPerSec = XMConvertToRadians(180.f);
	pDesc->tTransform3DDesc.fSpeedPerSec = 0.f;

	pDesc->iRenderGroupID_3D = RG_3D;
	pDesc->iPriorityID_3D = PR3D_GEOMETRY;
	pDesc->iObjectGroupID = OBJECT_GROUP::INTERACTION_OBEJCT;

	CActor::ACTOR_DESC ActorDesc;
	pDesc->eActorType = ACTOR_TYPE::STATIC;


	/* Actor의 주인 오브젝트 포인터 */
	ActorDesc.pOwner = this;

	/* Actor의 회전축을 고정하는 파라미터 */
	ActorDesc.FreezeRotation_XYZ[0] = true;
	ActorDesc.FreezeRotation_XYZ[1] = true;
	ActorDesc.FreezeRotation_XYZ[2] = true;

	/* Actor의 이동축을 고정하는 파라미터 (이걸 고정하면 중력도 영향을 받지 않음. 아예 해당 축으로의 이동을 제한하는)*/
	ActorDesc.FreezePosition_XYZ[0] = false;
	ActorDesc.FreezePosition_XYZ[1] = false;
	ActorDesc.FreezePosition_XYZ[2] = false;

	/* 사용하려는 Shape의 형태를 정의 */
	SHAPE_BOX_DESC BoxDesc = {};
	BoxDesc.vHalfExtents = { 19.8f, 0.77f, 5.6f };

	SHAPE_DATA ShapeData;
	ShapeData.pShapeDesc = &BoxDesc;              // 위에서 정의한 ShapeDesc의 주소를 저장.
	ShapeData.eShapeType = SHAPE_TYPE::BOX;     // Shape의 형태.
	ShapeData.eMaterial = ACTOR_MATERIAL::NORESTITUTION;
	ShapeData.iShapeUse = (_uint)SHAPE_USE::SHAPE_BODY;
	ShapeData.isTrigger = false;                    // Trigger 알림을 받기위한 용도라면 true
	XMStoreFloat4x4(&ShapeData.LocalOffsetMatrix, XMMatrixIdentity()); // Shape의 LocalOffset을 행렬정보로 저장.

	/* 최종으로 결정 된 ShapeData를 PushBack */
	ActorDesc.ShapeDatas.push_back(ShapeData);

	//책위에는 없고 주변에 플레이어가 있는지 감지하기
	SHAPE_BOX_DESC BoxDesc2 = {};
	BoxDesc2.vHalfExtents = { 21.8f, 0.3f, 7.6f };
	SHAPE_DATA ShapeData2;
	ShapeData2.pShapeDesc = &BoxDesc2;          
	ShapeData2.eShapeType = SHAPE_TYPE::BOX;
	ShapeData2.eMaterial = ACTOR_MATERIAL::NORESTITUTION;
	ShapeData2.iShapeUse = (_uint)SHAPE_USE::SHAPE_TRIGER;
	ShapeData2.isTrigger = true;    
	ShapeData2.FilterData.MyGroup = OBJECT_GROUP::MAPOBJECT;
	ShapeData2.FilterData.OtherGroupMask = OBJECT_GROUP::PLAYER;
	XMStoreFloat4x4(&ShapeData2.LocalOffsetMatrix, XMMatrixIdentity());
	ActorDesc.ShapeDatas.push_back(ShapeData2);

	ActorDesc.tFilterData.MyGroup = OBJECT_GROUP::MAPOBJECT;
	ActorDesc.tFilterData.OtherGroupMask = OBJECT_GROUP::MONSTER | OBJECT_GROUP::MONSTER_PROJECTILE | OBJECT_GROUP::TRIGGER_OBJECT | OBJECT_GROUP::PLAYER;

	/* Actor Component Finished */
	pDesc->pActorDesc = &ActorDesc;

	__super::Initialize(pDesc);
	Set_AnimationLoop(COORDINATE_3D, 0, true);
	Set_AnimationLoop(COORDINATE_3D, 8, false);
	Set_AnimationLoop(COORDINATE_3D, 9, true);
	Set_Animation(0);


	Bind_AnimEventFunc("Player_Change_Action", bind(&CSampleBook::PageAction_Call_PlayerEvent, this));

	Register_OnAnimEndCallBack(bind(&CSampleBook::PageAction_End, this, placeholders::_1, placeholders::_2));


	/* Com_AnimEventGenerator */
	CAnimEventGenerator::ANIMEVTGENERATOR_DESC tAnimEventDesc{};
	tAnimEventDesc.pReceiver = this;
	tAnimEventDesc.pSenderModel = Get_Model(COORDINATE_3D);
	m_pAnimEventGenerator = 
		static_cast<CAnimEventGenerator*> 
		(m_pGameInstance->
			Clone_Prototype(PROTOTYPE::PROTO_COMPONENT, m_iCurLevelID, TEXT("Prototype_Component_BookPageActionEvent"), &tAnimEventDesc));
	
	Add_Component(TEXT("AnimEventGenerator"), m_pAnimEventGenerator);


	Init_RT_RenderPos_Capcher();

	m_fInteractChargeTime = 0.0f;
	m_eInteractType = INTERACT_TYPE::NORMAL;
	m_eInteractKey = KEY::Q;

	return S_OK;
}

void CSampleBook::Priority_Update(_float _fTimeDelta)
{
	__super::Priority_Update(_fTimeDelta);
}

void CSampleBook::Update(_float _fTimeDelta)
{
	_float3 fDefaultPos = { };

	if (KEY_DOWN(KEY::M))
	{
		Event_Book_Main_Section_Change_Start(1, &fDefaultPos);

	}
	if (KEY_DOWN(KEY::N))
	{
		Event_Book_Main_Section_Change_Start(0, &fDefaultPos);

	}

		if ((ACTION_LAST != m_eCurAction) && true == m_isAction) {

			CGameObject* pGameObject = m_pGameInstance->Get_GameObject_Ptr(m_pGameInstance->Get_CurLevelID(), L"Layer_Player", 0);
			if (nullptr == pGameObject
				||
				pGameObject->Get_CurCoord() == COORDINATE_3D
				||
				CCamera_2D::FLIPPING_PAUSE == CCamera_Manager::GetInstance()->Get_CurCameraMode()
				)
			{
				if (m_eCurAction == NEXT)
				{
					Set_ReverseAnimation(false);
					Set_Animation(8);
				}

				if (m_eCurAction == PREVIOUS)
				{
					Set_ReverseAnimation(true);
					Set_Animation(8);
				}

				m_isAction = false;
			}
		}

		/*
		* 임시, 민용추가
		* 현재 Anim (책 넘기는 애니메이션 제외!)이 끝 나면 IDLE Animation으로 가게했음
		*/
		if (NONEANIM_ACTION != m_eAnimAction)
		{
			m_fAccAnimTime += _fTimeDelta;
			
			// 어쩔수없는하드코딩..
			if (MAGICDUST_ANIM_ACTION == m_eAnimAction && 6.7f <= m_fAccAnimTime)
			{
				m_fAccAnimTime = 0.f;
				m_eAnimAction = NONEANIM_ACTION;
				Set_Animation(IDLE);
			}
			//if (false == Is_PlayingAnim())
		}


	__super::Update(_fTimeDelta);

}

void CSampleBook::Late_Update(_float _fTimeDelta)
{
	//Register_RenderGroup(m_iRenderGroupID_3D, m_iPriorityID_3D);
	__super::Late_Update(_fTimeDelta);
}

HRESULT CSampleBook::Render()
{
	if (FAILED(Bind_ShaderResources_WVP()))
		return E_FAIL;
	COORDINATE eCoord = m_pControllerTransform->Get_CurCoord();
	CShader* pShader = m_pShaderComs[eCoord];
	_uint iShaderPass = m_iShaderPasses[eCoord];
	C3DModel* pModel = static_cast<C3DModel*>(m_pControllerModel->Get_Model(Get_CurCoord()));

	_float2 fLeftStartUV = { 0.f, 0.f };
	_float2 fLeftEndUV = { 0.5f, 1.f };
	_float2 fRightStartUV = { 0.5f, 0.f };
	_float2 fRightEndUV = { 1.f, 1.f };

	for (_uint i = 0; i < (_uint)pModel->Get_Meshes().size(); ++i)
	{
		_uint iRotateFlag = 0;
		_uint iShaderPass = (_uint)PASS_VTXANIMMESH::RENDERTARGET_MAPP;
		auto pMesh = pModel->Get_Mesh(i);
		_uint iMaterialIndex = pMesh->Get_MaterialIndex();

		
		if (FAILED(pModel->Bind_Material_PixelConstBuffer(iMaterialIndex, pShader)))
			return E_FAIL;

		switch (iMaterialIndex)
		{
			// 렌더타겟 메쉬들. 
			case Client::CSampleBook::NEXT_RIGHT:
			case Client::CSampleBook::NEXT_LEFT:
			case Client::CSampleBook::CUR_RIGHT:
			case Client::CSampleBook::CUR_LEFT:
		{


	#pragma region 현재 페이지인지, 다음 페이지인지 확인하고 SRV 가져오기.

			CSection* pTargetSection = nullptr;
			ID3D11ShaderResourceView* pResourceView = nullptr;
				switch (m_eCurAction)
				{
					// 이전 페이지로 진행 중이라면, 현재 페이지 -> 바뀌어야 할 페이지(이전 페이지)
					//								다음 페이지 -> 바뀌기 전 페이지(현재 페이지)
				case Client::CSampleBook::PREVIOUS:
					if (!m_isAction)
					{
						if (CUR_LEFT == i || CUR_RIGHT == i)
							pTargetSection = SECTION_MGR->Find_Section(SECTION_MGR->Get_Prev_Section_Key());
						else if (NEXT_LEFT == i || NEXT_RIGHT == i)
							pTargetSection = SECTION_MGR->Find_Section(SECTION_MGR->Get_Cur_Section_Key());
						else
							MSG_BOX("Book Mesh Binding Error - SampleBook.cpp");
						break;
					}
					// 나머지 케이스는				다음 -> 다음 (다음이 없으면, 기본 마테리얼 디퓨즈.)
					//								현재 -> 현재 
					// PREVIOUS 의 경우 m_isAction 전까찌는 일반 렌더(break 안태움 ! )
				case Client::CSampleBook::NEXT:
				case Client::CSampleBook::ACTION_LAST:
				default:

					if (CUR_LEFT == i || CUR_RIGHT == i)
						pTargetSection = SECTION_MGR->Find_Section(SECTION_MGR->Get_Cur_Section_Key());
					else if (NEXT_LEFT == i || NEXT_RIGHT == i)
					{
						if (SECTION_MGR->Has_Next_Section())
							pTargetSection = SECTION_MGR->Find_Section(SECTION_MGR->Get_Next_Section_Key());
						else
							if (FAILED(pModel->Bind_Material(pShader, "g_AlbedoTexture", i, aiTextureType_DIFFUSE, 0)))
								continue;
					}
					else
						MSG_BOX("Book Mesh Binding Error - SampleBook.cpp");

					break;
				}
	#pragma endregion

				if (nullptr != pTargetSection)
				{
					CSection_2D* pSection_2D = static_cast<CSection_2D*>(pTargetSection);

					pResourceView =  pSection_2D->Get_SRV_FromRenderTarget();

					if(nullptr != pResourceView)
						pShader->Bind_SRV("g_AlbedoTexture", pResourceView);
					

					if(pSection_2D->Is_Rotation())
						iRotateFlag = 1;
				}

#pragma region uv 매핑하기. (오른쪽 왼쪽 확인)
				if (CUR_LEFT == i || NEXT_LEFT == i)
				{
					if (FAILED(pShader->Bind_RawValue("g_fStartUV", &fLeftStartUV, sizeof(_float2))))
						return E_FAIL;
					if (FAILED(pShader->Bind_RawValue("g_fEndUV", &fLeftEndUV, sizeof(_float2))))
						return E_FAIL;
				}
				else if (CUR_RIGHT == i || NEXT_RIGHT == i)
				{
					if (FAILED(pShader->Bind_RawValue("g_fStartUV", &fRightStartUV, sizeof(_float2))))
						return E_FAIL;
					if (FAILED(pShader->Bind_RawValue("g_fEndUV", &fRightEndUV, sizeof(_float2))))
						return E_FAIL;
				}
				else
					MSG_BOX("Book Mesh Binding Error - SampleBook.cpp");
#pragma endregion



		}
		break;
		default:
			if (FAILED(pModel->Bind_Material(pShader, "g_AlbedoTexture", i, aiTextureType_DIFFUSE, 0)))
			{
				continue;
			}
			if (FAILED(pModel->Bind_Material(pShader, "g_NormalTexture", i, aiTextureType_NORMALS, 0)))
			{
				int a = 0;
			}
			if (FAILED(pModel->Bind_Material(pShader, "g_ORMHTexture", i, aiTextureType_BASE_COLOR, 0)))
			{
				int a = 0;
			}
			if (FAILED(pModel->Bind_Material(pShader, "g_MetallicTexture", i, aiTextureType_METALNESS, 0)))
			{
				int a = 0;
			}
			if (FAILED(pModel->Bind_Material(pShader, "g_RoughnessTexture", i, aiTextureType_DIFFUSE_ROUGHNESS, 0)))
			{
				int a = 0;
			}
			if (FAILED(pModel->Bind_Material(pShader, "g_AOTexture", i, aiTextureType_AMBIENT_OCCLUSION, 0)))
			{
				int a = 0;
			}
			
			break;
		}

		if (FAILED(pModel->Bind_Matrices(pShader, "g_BoneMatrices", i)))
			return E_FAIL;

		m_pShaderComs[COORDINATE_3D]->Bind_RawValue("g_iFlag", &iRotateFlag, sizeof(_uint));

		pShader->Begin(iShaderPass);

		pMesh->Bind_BufferDesc();
		pMesh->Render();
	}

	return S_OK;
}

HRESULT CSampleBook::Render_Shadow()
{
	return S_OK;
}

HRESULT CSampleBook::Render_WorldPosMap(const _wstring& _strCopyRTTag, const _wstring& _strSectionTag)
{
#pragma region 1. 책의 메인 페이지 좌, 우 메쉬를 찾아서 렌더.

	CSection* pSection = SECTION_MGR->Find_Section(_strSectionTag);
	ID3D11Resource* pResource = nullptr;
	
	ID3D11ShaderResourceView* pSRV = m_pGameInstance->Get_RT_SRV(_strCopyRTTag);
	pSRV->GetResource(&pResource);
	
	
	ID3D11Texture2D* pTexture = nullptr;

	_uint iRotateFlag = 0;

	if (nullptr == pSection)
	{
		D3D11_TEXTURE2D_DESC desc = {};
		desc.Width = RTSIZE_BOOK2D_X; // 원본 텍스처 너비
		desc.Height = RTSIZE_BOOK2D_Y; // 원본 텍스처 높이
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; // 원본 텍스처와 동일한 포맷
		desc.SampleDesc.Count = 1;
		desc.Usage = D3D11_USAGE_STAGING; // CPU 읽기 전용
		desc.BindFlags = 0; // 바인딩 없음
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		m_pDevice->CreateTexture2D(&desc, nullptr, &pTexture);

	}
	else
	{
		CSection_2D* p2DSection = static_cast<CSection_2D*>(pSection);
		pTexture = p2DSection->Get_WorldTexture();
		Safe_AddRef(pTexture);
		if (nullptr == pTexture)
			return E_FAIL;

		if (p2DSection->Is_Rotation())
			iRotateFlag = 1;
	}





	if (FAILED(m_pShaderComs[COORDINATE_3D]->Bind_Matrix("g_WorldMatrix", m_pControllerTransform->Get_WorldMatrix_Ptr())))
		return E_FAIL;

	C3DModel* p3DModel = static_cast<C3DModel*>(m_pControllerModel->Get_Model(COORDINATE_3D));
	if (nullptr == p3DModel)
		return E_FAIL;

	CMesh* pLeftMesh = p3DModel->Get_Mesh(CUR_LEFT);
	CMesh* pRightMesh = p3DModel->Get_Mesh(CUR_RIGHT);


	_float2 vStartCoord = { 0.0f, 0.0f };
	_float2 vEndCoord = { 0.5f, 1.f };
	m_pShaderComs[COORDINATE_3D]->Bind_RawValue("g_fStartUV", &vStartCoord, sizeof(_float2));
	m_pShaderComs[COORDINATE_3D]->Bind_RawValue("g_fEndUV", &vStartCoord, sizeof(_float2));
	m_pShaderComs[COORDINATE_3D]->Bind_RawValue("g_iFlag", &iRotateFlag, sizeof(_uint));

	p3DModel->Bind_Matrices(m_pShaderComs[COORDINATE_3D], "g_BoneMatrices", CUR_LEFT);
	m_pShaderComs[COORDINATE_3D]->Begin((_uint)PASS_VTXANIMMESH::WORLDPOSMAP_BOOK);
	pLeftMesh->Bind_BufferDesc();
	pLeftMesh->Render();

	vStartCoord = { 0.5f, 0.0f };
	vEndCoord = { 1.f, 1.f };
	m_pShaderComs[COORDINATE_3D]->Bind_RawValue("g_fStartUV", &vStartCoord, sizeof(_float2));
	m_pShaderComs[COORDINATE_3D]->Bind_RawValue("g_fEndUV", &vStartCoord, sizeof(_float2));
	p3DModel->Bind_Matrices(m_pShaderComs[COORDINATE_3D], "g_BoneMatrices", CUR_RIGHT);
	m_pShaderComs[COORDINATE_3D]->Begin((_uint)PASS_VTXANIMMESH::WORLDPOSMAP_BOOK);
	pRightMesh->Bind_BufferDesc();
	pRightMesh->Render();

#pragma region 2. 읽기 전용 텍스쳐에 WorldMapPos를 복사.


	m_pContext->CopyResource(pTexture, pResource);


	if (nullptr == pSection)
	{
		SECTION_MGR->Set_BookWorldPosMapTexture(pTexture);
		Safe_AddRef(pTexture);
	}


	Safe_Release(pResource);
	Safe_Release(pTexture);

#pragma endregion // 2
    return S_OK;
}

_bool CSampleBook::Book_Action(BOOK_PAGE_ACTION _eAction)
{

	//if (ACTION_LAST != m_eCurAction)
	//	return false;

	switch (_eAction)
	{
	case Client::CSampleBook::PREVIOUS:
		if (!SECTION_MGR->Has_Prev_Section())
			return false;
		SECTION_MGR->SetActive_Section(SECTION_MGR->Get_Prev_Section_Key(),true);
		m_eCurAction = PREVIOUS;
		break;
	case Client::CSampleBook::NEXT:
		if (!SECTION_MGR->Has_Next_Section())
			return false;
		SECTION_MGR->SetActive_Section(SECTION_MGR->Get_Next_Section_Key(),true);
		m_eCurAction = NEXT;
		break;
	case Client::CSampleBook::ACTION_LAST:
		break;
	default:
		break;
	}



	return true;
}

void CSampleBook::PageAction_End(COORDINATE _eCoord, _uint iAnimIdx)
{
#pragma region 책 넘기는 Action

	if (ACTION_LAST != m_eCurAction)
	{
		if (NEXT == m_eCurAction)
		{
			if (SECTION_MGR->Has_Next_Section())
				Event_Book_Main_Section_Change_End(SECTION_MGR->Get_Next_Section_Key());
		}
		else if (PREVIOUS == m_eCurAction)
		{
			// 이거 애니메이션 마지막프레임이 실행되는것 때문에, 앞으로가기와 뒤로가기의
			// 실제 섹션 변경시점을 다르게 해줘야 할거같음 지금상황에선 어쩔수없이.
			// 애니메이션이 못생긴걸 탓합니다. 
			//if (SECTION_MGR->Has_Prev_Section())
			//	Event_Book_Main_Section_Change(SECTION_MGR->Get_Prev_Section_Key()->c_str());
			SECTION_MGR->Change_Prev_Section();
		}
		if (CCamera_Manager::CAMERA_TYPE::TARGET_2D == CCamera_Manager::GetInstance()->Get_CameraType())
		{
			Event_Book_Main_Change(CCamera_Manager::GetInstance()->Get_CameraType());
		}

		Set_Animation(0);
		m_eCurAction = ACTION_LAST;
	}
#pragma endregion
}

void CSampleBook::PageAction_Call_PlayerEvent()
{
	CGameObject* pGameObject = m_pGameInstance->Get_GameObject_Ptr(m_iCurLevelID, L"Layer_Player", 0);

	if (nullptr != pGameObject && COORDINATE_2D ==  pGameObject->Get_CurCoord())
	{
		_wstring strMoveSectionName = L"";
		if (FAILED(SECTION_MGR->Remove_GameObject_ToCurSectionLayer(pGameObject)))
			return;
		CCarriableObject* pCarryingObj = static_cast<CPlayer*>(pGameObject)->Get_CarryingObject();
		if (pCarryingObj)
		{
			if (FAILED(SECTION_MGR->Remove_GameObject_ToCurSectionLayer(pCarryingObj)))
				return;
		}
		if (NEXT == m_eCurAction)
		{
			if (SECTION_MGR->Has_Next_Section())
				strMoveSectionName = SECTION_MGR->Get_Next_Section_Key();
		}
		else if (PREVIOUS == m_eCurAction)
		{
			if (SECTION_MGR->Has_Prev_Section())
				strMoveSectionName = SECTION_MGR->Get_Prev_Section_Key();
		}
		
		if (L"" != strMoveSectionName)
		{
			_vector vNewPos = XMVectorSet(m_fNextPos.x, m_fNextPos.y, 0.f, 1.f);

			pGameObject->Set_Position(vNewPos);

			if (FAILED(SECTION_MGR->Add_GameObject_ToSectionLayer(strMoveSectionName, pGameObject, SECTION_2D_PLAYMAP_OBJECT)))
				return;
			if(pCarryingObj)
			{
				if (FAILED(SECTION_MGR->Add_GameObject_ToSectionLayer(strMoveSectionName, pCarryingObj, SECTION_2D_PLAYMAP_OBJECT)))
					return;
			}
			if (CCamera_Manager::CAMERA_TYPE::TARGET_2D == CCamera_Manager::GetInstance()->Get_CameraType())
			{
				CCamera* pCamera = CCamera_Manager::GetInstance()->Get_Camera(CCamera_Manager::TARGET_2D);
				static_cast<CCamera_2D*>(pCamera)->Set_Include_Section_Name(strMoveSectionName);
			}
		}
	}
}

HRESULT CSampleBook::Init_RT_RenderPos_Capcher()
{
	m_pGameInstance->Add_RenderObject_New(RG_WORLDPOSMAP, PRWORLD_MAINBOOK, this);

	// 따로 찍어야할 섹션을 확인후 레지스터.
	SECTION_MGR->Register_WorldCapture(L"Chapter2_P0102", this);
	SECTION_MGR->Register_WorldCapture(L"Chapter4_P0102", this);
	SECTION_MGR->Register_WorldCapture(L"Chapter4_P0304", this);

	return S_OK;
}

void CSampleBook::OnTrigger_Enter(const COLL_INFO& _My, const COLL_INFO& _Other)
{
	SHAPE_USE eShapeUse = (SHAPE_USE)_My.pShapeUserData->iShapeUse;
	switch (eShapeUse)
	{
	case Client::SHAPE_USE::SHAPE_TRIGER:
		if (OBJECT_GROUP::PLAYER == _Other.pActorUserData->iObjectGroup
			&& (_uint)SHAPE_USE::SHAPE_BODY == _Other.pShapeUserData->iShapeUse)
		{
			m_isPlayerAround = true;
		}
		break;
	}
}

void CSampleBook::OnTrigger_Stay(const COLL_INFO& _My, const COLL_INFO& _Other)
{
}

void CSampleBook::OnTrigger_Exit(const COLL_INFO& _My, const COLL_INFO& _Other)
{
	SHAPE_USE eShapeUse = (SHAPE_USE)_My.pShapeUserData->iShapeUse;
	switch (eShapeUse)
	{
	case Client::SHAPE_USE::SHAPE_TRIGER:
		if (OBJECT_GROUP::PLAYER == _Other.pActorUserData->iObjectGroup
			&& (_uint)SHAPE_USE::SHAPE_BODY == _Other.pShapeUserData->iShapeUse)
		{
			m_isPlayerAround = false;
		}
		break;
	}
}
void CSampleBook::Interact(CPlayer* _pUser)
{
	_pUser->Set_State(CPlayer::TURN_BOOK);
}

_bool CSampleBook::Is_Interactable(CPlayer* _pUser)
{
	return m_isPlayerAround && (false == _pUser->Is_CarryingObject());
}

_float CSampleBook::Get_Distance(COORDINATE _eCoord, CPlayer* _pUser)
{
	return 9999.f;
}

HRESULT CSampleBook::Execute_Action(BOOK_PAGE_ACTION _eAction, _float3 _fNextPosition)
{
	if (Book_Action(_eAction))
	{
		m_fNextPos = _fNextPosition;
		m_isAction = true;
		if(CCamera_Manager::CAMERA_TYPE::TARGET_2D == CCamera_Manager::GetInstance()->Get_CameraType())
			CCamera_Manager::GetInstance()->Change_CameraMode(CCamera_2D::FLIPPING_UP);
	}
	return S_OK;
}

void CSampleBook::Execute_AnimEvent(_uint _iAnimIndex)
{
	m_eAnimAction = (BOOK_ANIM_ACTION)_iAnimIndex;
	Set_ReverseAnimation(false);
	Set_Animation(_iAnimIndex);
}

CSampleBook* CSampleBook::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CSampleBook* pInstance = new CSampleBook(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSampleBook");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSampleBook::Clone(void* _pArg)
{
	CSampleBook* pInstance = new CSampleBook(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CSampleBook");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSampleBook::Free()
{
	Safe_Release(m_pAnimEventGenerator);
	__super::Free();
}


