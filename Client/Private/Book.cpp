#include "stdafx.h"
#include "Book.h"
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
#include "Detonator.h"
#include "SlipperyObject.h"


CBook::CBook(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CModelObject(_pDevice, _pContext)
{
}

CBook::CBook(const CBook& _Prototype)
	: CModelObject(_Prototype)
{
}


HRESULT CBook::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	return S_OK;
}

HRESULT CBook::Initialize(void* _pArg)
{

	BOOK_DESC* pDesc = static_cast<BOOK_DESC*>(_pArg);

	pDesc->Build_3D_Model(pDesc->iCurLevelID,
		L"book",
		L"Prototype_Component_Shader_VtxAnimMesh",
		(_uint)PASS_VTXANIMMESH::RENDERTARGET_MAPP
	);
	pDesc->iModelPrototypeLevelID_3D = LEVEL_STATIC;
	if (!pDesc->isInitOverride)
	{
		pDesc->tTransform3DDesc.vInitialPosition = _float3(2.f, 0.4f, -17.3f);
		pDesc->tTransform3DDesc.vInitialScaling = _float3(1.0f, 1.0f, 1.0f);
	}

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

	//책주변에는 없고 위에 플레이어가 있는지 감지하기
	SHAPE_BOX_DESC BoxDesc3 = {};
	BoxDesc3.vHalfExtents = { 19.8f, 1.2f, 5.6f };
	SHAPE_DATA ShapeData3;
	ShapeData3.pShapeDesc = &BoxDesc3;
	ShapeData3.eShapeType = SHAPE_TYPE::BOX;
	ShapeData3.eMaterial = ACTOR_MATERIAL::NORESTITUTION;
	ShapeData3.iShapeUse = (_uint)SHAPE_USE::SHAPE_TRIGER;
	ShapeData3.isTrigger = true;
	ShapeData3.FilterData.MyGroup = OBJECT_GROUP::MAPOBJECT;
	ShapeData3.FilterData.OtherGroupMask = OBJECT_GROUP::PLAYER;
	XMStoreFloat4x4(&ShapeData3.LocalOffsetMatrix, XMMatrixIdentity());
	ActorDesc.ShapeDatas.push_back(ShapeData3);



	ActorDesc.tFilterData.MyGroup = OBJECT_GROUP::MAPOBJECT;
	ActorDesc.tFilterData.OtherGroupMask = OBJECT_GROUP::MONSTER | OBJECT_GROUP::MONSTER_PROJECTILE | OBJECT_GROUP::TRIGGER_OBJECT | OBJECT_GROUP::PLAYER | OBJECT_GROUP::DYNAMIC_OBJECT;

	/* Actor Component Finished */
	pDesc->pActorDesc = &ActorDesc;

	__super::Initialize(pDesc);
	Set_AnimationLoop(COORDINATE_3D, 0, true);
	Set_AnimationLoop(COORDINATE_3D, 8, false);
	Set_AnimationLoop(COORDINATE_3D, 9, true);
	Set_Animation(0);


	Bind_AnimEventFunc("Player_Change_Action", bind(&CBook::PageAction_Call_PlayerEvent, this));

	Register_OnAnimEndCallBack(bind(&CBook::PageAction_End, this, placeholders::_1, placeholders::_2));


	/* Com_AnimEventGenerator */
	CAnimEventGenerator::ANIMEVTGENERATOR_DESC tAnimEventDesc{};
	tAnimEventDesc.pReceiver = this;
	tAnimEventDesc.pSenderModel = Get_Model(COORDINATE_3D);
	m_pAnimEventGenerator = 
		static_cast<CAnimEventGenerator*> 
		(m_pGameInstance->
			Clone_Prototype(PROTOTYPE::PROTO_COMPONENT, LEVEL_STATIC, TEXT("Prototype_Component_BookPageActionEvent"), &tAnimEventDesc));
	
	Add_Component(TEXT("AnimEventGenerator"), m_pAnimEventGenerator);
	Safe_AddRef(m_pAnimEventGenerator);

	Init_RT_RenderPos_Capcher();

	m_fInteractChargeTime = 0.0f;
	m_eInteractID = INTERACT_ID::BOOK;
	m_eInteractType = INTERACT_TYPE::NORMAL;
	m_eInteractKey = KEY::Q;

	return S_OK;
}

void CBook::Priority_Update(_float _fTimeDelta)
{
	__super::Priority_Update(_fTimeDelta);
}

void CBook::Update(_float _fTimeDelta)
{
	_float3 fDefaultPos = { };

	if (KEY_DOWN(KEY::M))
	{
		Event_Book_Main_Section_Change_Start(1, &fDefaultPos);
		m_pGameInstance->Start_SFX_Delay(_wstring(L"A_sfx_page_turn-") + to_wstring(rand() % 6), 0.5f, 50.f);

	}
	if (KEY_DOWN(KEY::N))
	{
		Event_Book_Main_Section_Change_Start(0, &fDefaultPos);
		m_pGameInstance->Start_SFX_Delay(_wstring(L"A_sfx_page_turn-") + to_wstring(rand() % 6), 0.5f, 50.f);


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
		if (ANIM_ACTION_NONE != m_eAnimAction)
		{
			m_fAccAnimTime += _fTimeDelta;
			
			// 어쩔수없는하드코딩..
			if (ANIM_ACTION_MAGICDUST == m_eAnimAction && 6.7f <= m_fAccAnimTime)
			{
				m_fAccAnimTime = 0.f;
				m_eAnimAction = ANIM_ACTION_NONE;
				Set_Animation(IDLE);
			}

			// 닫힌 애니메이션 ? 
			//if (false == Is_PlayingAnim())
		}


	__super::Update(_fTimeDelta);

}

void CBook::Late_Update(_float _fTimeDelta)
{
	if (KEY_DOWN(KEY::X))
	{
		_int isRenderState = m_eCurRenderState;
		isRenderState ^= 1;

		Change_RenderState((RT_RENDERSTATE)isRenderState);
	}
	__super::Late_Update(_fTimeDelta);
}

HRESULT CBook::Render()
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
		//_uint iShaderPass = (_uint)PASS_VTXANIMMESH::RENDERTARGET_MAPP;
		auto pMesh = pModel->Get_Mesh(i);
		_uint iMaterialIndex = pMesh->Get_MaterialIndex();

		
		if (FAILED(pModel->Bind_Material_PixelConstBuffer(iMaterialIndex, pShader)))
			return E_FAIL;

		switch (iMaterialIndex)
		{
			// 렌더타겟 메쉬들. 
			case Client::CBook::NEXT_RIGHT:
			case Client::CBook::NEXT_LEFT:
			case Client::CBook::CUR_RIGHT:
			case Client::CBook::CUR_LEFT:
		{


	#pragma region 현재 페이지인지, 다음 페이지인지 확인하고 SRV 가져오기.

			CSection* pTargetSection = nullptr;
			ID3D11ShaderResourceView* pResourceView = nullptr;
				switch (m_eCurAction)
				{
					// 이전 페이지로 진행 중이라면, 현재 페이지 -> 바뀌어야 할 페이지(이전 페이지)
					//								다음 페이지 -> 바뀌기 전 페이지(현재 페이지)
				case Client::CBook::PREVIOUS:
					if (!m_isAction)
					{
						if (CUR_LEFT == i || CUR_RIGHT == i)
							pTargetSection = SECTION_MGR->Find_Section(SECTION_MGR->Get_Prev_Section_Key());
						else if (NEXT_LEFT == i || NEXT_RIGHT == i)
							pTargetSection = SECTION_MGR->Find_Section(SECTION_MGR->Get_Cur_Section_Key());
						else
							MSG_BOX("Book Mesh Binding Error - Book.cpp");
						break;
					}
					// 나머지 케이스는				다음 -> 다음 (다음이 없으면, 기본 마테리얼 디퓨즈.)
					//								현재 -> 현재 
					// PREVIOUS 의 경우 m_isAction 전까찌는 일반 렌더(break 안태움 ! )
				case Client::CBook::NEXT:
				case Client::CBook::ACTION_LAST:
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
						MSG_BOX("Book Mesh Binding Error - Book.cpp");

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
					MSG_BOX("Book Mesh Binding Error - Book.cpp");
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


HRESULT CBook::Render_WorldPosMap(const _wstring& _strCopyRTTag, const _wstring& _strSectionTag)
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
	m_pShaderComs[COORDINATE_3D]->Bind_RawValue("g_fEndUV", &vEndCoord, sizeof(_float2));
	m_pShaderComs[COORDINATE_3D]->Bind_RawValue("g_iFlag", &iRotateFlag, sizeof(_uint));

	p3DModel->Bind_Matrices(m_pShaderComs[COORDINATE_3D], "g_BoneMatrices", CUR_LEFT);
	m_pShaderComs[COORDINATE_3D]->Begin((_uint)PASS_VTXANIMMESH::WORLDPOSMAP_BOOK);
	pLeftMesh->Bind_BufferDesc();
	pLeftMesh->Render();

	vStartCoord = { 0.5f, 0.0f };
	vEndCoord = { 1.f, 1.f };
	m_pShaderComs[COORDINATE_3D]->Bind_RawValue("g_fStartUV", &vStartCoord, sizeof(_float2));
	m_pShaderComs[COORDINATE_3D]->Bind_RawValue("g_fEndUV", &vEndCoord, sizeof(_float2));
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
	else 
	{
		CSection_2D* p2DSection = static_cast<CSection_2D*>(pSection);
		Safe_AddRef(pTexture);
		p2DSection->Set_WorldTexture(pTexture);
		//어차피 똑같은건데... 코드 분리하기 좀 늦은거같아서 그냥 똑같은거 한번 넣어서 포탈 생성 타게 만듬.
											   // 그를 위한 Safe_Addref(내부에서 한번 릴리즈 해주니까.) 
	}


	Safe_Release(pResource);
	Safe_Release(pTexture);

#pragma endregion // 2
    return S_OK;
}

_bool CBook::Book_Action(BOOK_PAGE_ACTION _eAction)
{

	//if (ACTION_LAST != m_eCurAction)
	//	return false;

	switch (_eAction)
	{
	case Client::CBook::PREVIOUS:
		if (!SECTION_MGR->Has_Prev_Section())
			return false;
		SECTION_MGR->SetActive_Section(SECTION_MGR->Get_Prev_Section_Key(),true);
		m_eCurAction = PREVIOUS;
		break;
	case Client::CBook::NEXT:
		if (!SECTION_MGR->Has_Next_Section())
			return false;
		SECTION_MGR->SetActive_Section(SECTION_MGR->Get_Next_Section_Key(),true);
		m_eCurAction = NEXT;
		break;
	case Client::CBook::ACTION_LAST:
		break;
	default:
		break;
	}



	return true;
}

void CBook::PageAction_End(COORDINATE _eCoord, _uint iAnimIdx)
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

void CBook::PageAction_Call_PlayerEvent()
{
	CGameObject* pGameObject = m_pGameInstance->Get_GameObject_Ptr(m_iCurLevelID, L"Layer_Player", 0);

	if (nullptr != pGameObject && COORDINATE_2D ==  pGameObject->Get_CurCoord())
	{
		_wstring strMoveSectionName = L"";
		if (FAILED(SECTION_MGR->Remove_GameObject_FromSectionLayer(pGameObject->Get_Include_Section_Name(),pGameObject)))
			return;
		CCarriableObject* pCarryingObj = static_cast<CPlayer*>(pGameObject)->Get_CarryingObject();
		if (pCarryingObj)
		{
			if (FAILED(SECTION_MGR->Remove_GameObject_FromSectionLayer(pCarryingObj->Get_Include_Section_Name(), pCarryingObj)))
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
				static_cast<CCamera_2D*>(pCamera)->Enter_Section(strMoveSectionName);
			}
		}
	}
}

HRESULT CBook::Init_RT_RenderPos_Capcher()
{
	m_pGameInstance->Add_RenderObject_New(RG_WORLDPOSMAP, PRWORLD_MAINBOOK, this);

	// 따로 찍어야할 섹션을 확인후 레지스터.
	SECTION_MGR->Register_WorldCapture(L"Chapter2_P0102", this);
	SECTION_MGR->Register_WorldCapture(L"Chapter4_P0102", this);
	SECTION_MGR->Register_WorldCapture(L"Chapter4_P0304", this);
	SECTION_MGR->Register_WorldCapture(L"Chapter4_P0506", this);
	SECTION_MGR->Register_WorldCapture(L"Chapter5_P0102", this);

	return S_OK;
}

void CBook::OnTrigger_Enter(const COLL_INFO& _My, const COLL_INFO& _Other)
{
	SHAPE_USE eShapeUse = (SHAPE_USE)_My.pShapeUserData->iShapeUse;
	switch (eShapeUse)
	{
	case Client::SHAPE_USE::SHAPE_TRIGER:
		if (OBJECT_GROUP::PLAYER == _Other.pActorUserData->iObjectGroup
			&& (_uint)SHAPE_USE::SHAPE_BODY == _Other.pShapeUserData->iShapeUse)
		{
			if(_My.pShapeUserData->iShapeIndex == 1)
				m_isPlayerAround = true;
			else if (_My.pShapeUserData->iShapeIndex == 2)
				m_isPlayerAbove = true;
		}
		break;
	}
}

void CBook::OnTrigger_Stay(const COLL_INFO& _My, const COLL_INFO& _Other)
{
}

void CBook::OnTrigger_Exit(const COLL_INFO& _My, const COLL_INFO& _Other)
{
	SHAPE_USE eShapeUse = (SHAPE_USE)_My.pShapeUserData->iShapeUse;
	switch (eShapeUse)
	{
	case Client::SHAPE_USE::SHAPE_TRIGER:
		if (OBJECT_GROUP::PLAYER == _Other.pActorUserData->iObjectGroup
			&& (_uint)SHAPE_USE::SHAPE_BODY == _Other.pShapeUserData->iShapeUse)
		{
			if (_My.pShapeUserData->iShapeIndex == 1)
				m_isPlayerAround = false;
			else if (_My.pShapeUserData->iShapeIndex == 2)
				m_isPlayerAbove = false;
		}
		break;
	}
}
void CBook::Interact(CPlayer* _pUser)
{
	if(m_isPlayerAround)
		_pUser->Set_State(CPlayer::TURN_BOOK);
	else if (m_isPlayerAbove)
	{
		if (_pUser->Is_DetonationMode())
		{
			_pUser->Set_State(CPlayer::BOMBER);
		}
		else
		{
			_pUser->Set_State(CPlayer::STAMP);
		}
	}
}

_bool CBook::Is_Interactable(CPlayer* _pUser)
{
	return (m_isPlayerAround || m_isPlayerAbove) 
		&& (false == _pUser->Is_CarryingObject())
		&& COORDINATE_3D == _pUser->Get_CurCoord();
}

_float CBook::Get_Distance(COORDINATE _eCoord, CPlayer* _pUser)
{
	return 9999.f;
}

_bool CBook::Is_DuringAnimation()
{
	return m_pControllerModel->Get_Model(m_pControllerTransform->Get_CurCoord())->Is_DuringAnimation();
}

void CBook::SlideObjects_RToL()
{
	CSection_Manager* pSectionMgr = CSection_Manager::GetInstance();
	CSection_2D* pSection = static_cast<CSection_2D*>( CSection_Manager::GetInstance()->Find_Section(pSectionMgr->Get_Cur_Section_Key()));
	list<CGameObject*> ObjList = pSection->Get_Section_Layer(SECTION_PLAYMAP_2D_RENDERGROUP::SECTION_2D_PLAYMAP_OBJECT)->Get_GameObjects();
	for (auto& pObj : ObjList)
	{
		if (XMVectorGetX( pObj->Get_FinalPosition()) > 0.f)
		{
			CSlipperyObject* pSlippery = dynamic_cast<CSlipperyObject*>(pObj);
			if(pSlippery)
				pSlippery->Start_Slip(_vector{ -1.0f, 0.f,0.f },500.f);
		}
	}
}

void CBook::SlideObjects_LToR()
{
	CSection_Manager* pSectionMgr = CSection_Manager::GetInstance();
	CSection_2D* pSection = static_cast<CSection_2D*>(CSection_Manager::GetInstance()->Find_Section(pSectionMgr->Get_Cur_Section_Key()));
	list<CGameObject*> ObjList = pSection->Get_Section_Layer(SECTION_PLAYMAP_2D_RENDERGROUP::SECTION_2D_PLAYMAP_OBJECT)->Get_GameObjects();
	for (auto& pObj : ObjList)
	{
		if (XMVectorGetX(pObj->Get_FinalPosition()) < 0.f)
		{
			CSlipperyObject* pSlippery = dynamic_cast<CSlipperyObject*>(pObj);
			if (pSlippery)
				pSlippery->Start_Slip(_vector{ 1.0f, 0.f,0.f },500.f);
		}
	}
}

HRESULT CBook::Execute_Action(BOOK_PAGE_ACTION _eAction, _float3 _fNextPosition)
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

void CBook::Execute_AnimEvent(_uint _iAnimIndex)
{
	m_eAnimAction = (BOOK_ANIM_ACTION)_iAnimIndex;
	Set_ReverseAnimation(false);
	Set_Animation(_iAnimIndex);
}

HRESULT CBook::Convert_Position_3DTo2D(_fvector _v3DPos, _vector* _pOutPosition)
{
	CSection_Manager* pSectionManager = CSection_Manager::GetInstance();
	_float2 vRenderTargetSize = pSectionManager->Get_Section_RenderTarget_Size(pSectionManager->Get_Cur_Section_Key());
	_float3	vBookWorldMin = pSectionManager->Get_BookMinWorldPos();
	_float3	vBookWorldMax = pSectionManager->Get_BookMaxWorldPos();
	_float2 vRatio = { (XMVectorGetX(_v3DPos) - vBookWorldMin.x) / (vBookWorldMax.x - vBookWorldMin.x)
	, (XMVectorGetZ(_v3DPos) - vBookWorldMin.z) / (vBookWorldMax.z - vBookWorldMin.z) };

	if (-0.f > vRatio.x || 1.f < vRatio.x
		|| -0.f > vRatio.y || 1.f < vRatio.y)
		return E_FAIL;
	// 2D 좌표계로 변환

	*_pOutPosition = { vRatio.x * vRenderTargetSize.x - vRenderTargetSize.x * 0.5f,
		 vRatio.y * vRenderTargetSize.y - vRenderTargetSize.y * 0.5f };
	return S_OK;
}



//void CBook::Calc_Page3DWorldMinMax()
//{//1. 책의 3d 월드 상에서 Min,Max 점 얻기
//	//2. _v3DPos가  Min, Max 안의 어떤 비율의 지점에 있는지 계산
//	//3. 비율 * 책 크기(section->RenderResolution)
//	CSection_Manager* pSectionManager = SECTION_MGR;
//	CSection* pSection = pSectionManager->Find_Section(pSectionManager->Get_Cur_Section_Key());
//
//	if (nullptr == pSection)
//		return;
//
//	CSection_2D* p2DSection = dynamic_cast<CSection_2D*>(pSection);
//
//	if (nullptr == p2DSection)
//		return;
//
//	ID3D11Texture2D* pTexture2D = p2DSection->Get_WorldTexture();
//
//	// 맵핑하여 데이터 접근
//	D3D11_MAPPED_SUBRESOURCE mappedResource;
//	if (FAILED(m_pContext->Map(pTexture2D, 0, D3D11_MAP_READ, 0, &mappedResource)))
//		return;
//	// 2D Transform 위치를 픽셀 좌표계로 변환. 해당 텍스쳐의 가로 세로 사이즈를 알아야함.
//	m_v2DWorldPixelSize.x = mappedResource.RowPitch / sizeof(_float) / 4;
//	m_v2DWorldPixelSize.y = mappedResource.DepthPitch / mappedResource.RowPitch;
//
//
//	_uint iLeftBotIndex = m_v2DWorldPixelSize.x * (m_v2DWorldPixelSize.y - 1) * 4;
//	_uint iRightTopIndex = (m_v2DWorldPixelSize.x - 2) * 4;
//	// float4 데이터 읽기
//	_float* fData = static_cast<_float*>(mappedResource.pData);
//	vBookWorldMin = { fData[iLeftBotIndex],fData[iLeftBotIndex + 1],fData[iLeftBotIndex + 2] };
//	vBookWorldMax = { fData[iRightTopIndex],fData[iRightTopIndex + 1],fData[iRightTopIndex + 2] };
//
//
//	m_pContext->Unmap(pTexture2D, 0);
//}

CBook* CBook::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CBook* pInstance = new CBook(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBook");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBook::Clone(void* _pArg)
{
	CBook* pInstance = new CBook(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CBook");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBook::Free()
{
	Safe_Release(m_pAnimEventGenerator);
	__super::Free();
}

void CBook::Change_RenderState(RT_RENDERSTATE _eRenderState)
{
	if (m_eCurRenderState == _eRenderState)
		return;

	switch (_eRenderState)
	{
	case Client::RT_RENDERSTATE::RENDERSTATE_LIGHT:
		m_iPriorityID_3D = PR3D_GEOMETRY;
		m_iShaderPasses[COORDINATE_3D] = (_uint)PASS_VTXANIMMESH::RENDERTARGET_MAPP;
		break;
	case Client::RT_RENDERSTATE::RENDERSTATE_NONLIGHT:
		m_iPriorityID_3D = PR3D_AFTERPOSTPROCESSING;
		m_iShaderPasses[COORDINATE_3D] = (_uint)PASS_VTXANIMMESH::RENDERTARGET_MAPP_AFTERPOSTPROCESSING;
		break;
	default:
		break;
	}

	m_eCurRenderState = _eRenderState;

}


