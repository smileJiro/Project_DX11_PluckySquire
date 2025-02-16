#include "Physx_Manager.h"
#include "GameInstance.h"
#include "Physx_EventCallBack.h"

_uint CPhysx_Manager::m_iShapeInstanceID = 0;

CPhysx_Manager::CPhysx_Manager(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: m_pGameInstance(CGameInstance::GetInstance())
	, m_pDevice(_pDevice)
	, m_pContext(_pContext)
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CPhysx_Manager::Initialize()
{
	// Event CallBack Class 
	m_pPhysx_EventCallBack = CPhysx_EventCallBack::Create();
	if (nullptr == m_pPhysx_EventCallBack)
		return E_FAIL;

	if (FAILED(Initialize_Foundation()))
		return E_FAIL;

#ifdef _DEBUG

	if (FAILED(Initialize_PVD()))
		return E_FAIL;

#endif // _DEBUG

	if (FAILED(Initialize_Physics()))
		return E_FAIL;

	if (FAILED(Initialize_Scene()))
		return E_FAIL;

	if (FAILED(Initialize_Material()))
		return E_FAIL;

	/* Test Code */
	//m_pGroundPlane = PxCreatePlane(*m_pPxPhysics, PxPlane(0, 1, 0, 99), *m_pPxMaterial[(_uint)ACTOR_MATERIAL::DEFAULT]);
	//m_pPxScene->addActor(*m_pGroundPlane);

	_float4x4 matTest = {};
	PxMat44 matPx;
	XMStoreFloat4x4(&matTest, XMMatrixIdentity());
	PxTransform transform(PxVec3(0.0f, -9.5f, 0.0f)); // 위치: (0, 0, 0)

	// 필요한 시각화 기능 활성화
#ifdef _DEBUG
	m_pPxScene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0f);
	m_pPxScene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f); // 충돌 형태 시각화
	//m_pPxScene->setVisualizationParameter(PxVisualizationParameter::eACTOR_AXES, 1.0f);

		/* Debug */
	m_pVIBufferCom = CVIBuffer_PxDebug::Create(m_pDevice, m_pContext, 30000);
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;


	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../../EngineSDK/hlsl/Shader_Debug.hlsl"), VTXPOSCOLOR::Elements, VTXPOSCOLOR::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;
#endif // _DEBUG


	return S_OK;
}

void CPhysx_Manager::Update(_float _fTimeDelta)
{
	//m_fTImeAcc : 지난 시뮬레이션 이후로 지난 시간.
	//m_fFixtedTimeStep : 고정된 시간 간격. 1/60초
	//1. 지난 시뮬이후로 1/60초 이상이 지났으면 시뮬레이션 해야 함.
	//	->만약 시뮬레이션이 안끝났으면? 물리 시뮬레이션 작업량이 너무 많아서 1/60안에 도저히 못끝냄.
	//  -> m_fFixtedTimeStep을 수정하거나, 다음 프레임으로 물ㄹ ㅣ시뮬레이션을 미룸. 
	//   -> 만약 다음 프레임으로 물리 시뮬을 미루면, 
	//

	if (nullptr != m_pPhysx_EventCallBack)
		m_pPhysx_EventCallBack->Update();

	m_fTimeAcc += _fTimeDelta;
	if (m_fFixtedTimeStep <= m_fTimeAcc)
	{
		m_pPxScene->simulate(m_fFixtedTimeStep);
		m_fTimeAcc -= m_fFixtedTimeStep;

		//fetch 끝났는지 확인
		if (m_pPxScene->fetchResults(true))
		{
#ifdef _DEBUG
			if (true == m_isDebugRender)
			{
				const PxRenderBuffer& RenderBuffer = m_pPxScene->getRenderBuffer();
				m_pVIBufferCom->Update_PxDebug(RenderBuffer);
			}
#endif // _DEBUG
		}

	}


//	if (nullptr != m_pPhysx_EventCallBack)
//		m_pPhysx_EventCallBack->Update();
//
//	//기존 코드
//	m_pPxScene->simulate(_fTimeDelta);
//
//	if (m_pPxScene->fetchResults(true))
//	{
//		if (nullptr != m_pPhysx_EventCallBack)
//			m_pPhysx_EventCallBack->Update();
//
//#ifdef _DEBUG
//		const PxRenderBuffer& RenderBuffer = m_pPxScene->getRenderBuffer();
//		m_pVIBufferCom->Update_PxDebug(RenderBuffer);
//#endif // _DEBUG
//	}

}

HRESULT CPhysx_Manager::Render()
{
	_float4x4 WorldMatrix = {};
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	m_pVIBufferCom->Bind_BufferDesc();
	m_pShader->Begin(0);
	m_pVIBufferCom->Render();

	return S_OK;
}

_float CPhysx_Manager::Get_Gravity()
{
	return m_pPxScene->getGravity().magnitude();
}

void CPhysx_Manager::Add_ShapeUserData(SHAPE_USERDATA* _pUserData)
{
	m_pShapeUserDatas.push_back(_pUserData);
}

void CPhysx_Manager::Delete_ShapeUserData()
{
	for (auto& pUserData : m_pShapeUserDatas)
	{
		delete pUserData;
		pUserData = nullptr;
	}
	m_pShapeUserDatas.clear();
}

_bool CPhysx_Manager::RayCast_Nearest(const _float3& _vOrigin, const _float3& _vRayDir, _float _fMaxDistance, _float3* _pOutPos, CActorObject** _ppOutActorObject)
{
	PxRaycastBuffer hit;

	PxVec3 vOrigin = { _vOrigin.x,_vOrigin.y, _vOrigin.z };
	PxVec3 vRayDir = { _vRayDir.x, _vRayDir.y, _vRayDir.z };

	_bool isResult = m_pPxScene->raycast(vOrigin, vRayDir, _fMaxDistance, hit);

	if (isResult && hit.hasBlock)
	{
		if (nullptr != _ppOutActorObject)
		{
			PxRigidActor* pActor = static_cast<PxRigidActor*>(hit.block.actor);
			ACTOR_USERDATA* pActorUserData = reinterpret_cast<ACTOR_USERDATA*>(pActor->userData);

			if(nullptr != pActorUserData)
				*_ppOutActorObject = pActorUserData->pOwner;
		}
		if(nullptr != _pOutPos)
			*_pOutPos = _float3(hit.block.position.x, hit.block.position.y, hit.block.position.z);

	}

	return isResult;
}

_bool CPhysx_Manager::RayCast_Nearest_GroupFilter(const _float3& _vOrigin, const _float3& _vRayDir, _float _fMaxDistance, _int _iGroupNum, _float3* _pOutPos, CActorObject** _ppOutActorObject)
{
	PxRaycastHit hitBuffer[10]; // 최대 10개까지 저장
	PxRaycastBuffer hit(hitBuffer, 10); // 버퍼 설정
	PxVec3 vOrigin = { _vOrigin.x,_vOrigin.y, _vOrigin.z };
	PxVec3 vRayDir = { _vRayDir.x, _vRayDir.y, _vRayDir.z };

	_bool isResult = m_pPxScene->raycast(vOrigin, vRayDir, _fMaxDistance, hit);

	for (PxU32 i = 0; i < hit.nbTouches; i++) {
		PxRigidActor* pActor = hit.touches[i].actor;
		ACTOR_USERDATA* pActorUserData = reinterpret_cast<ACTOR_USERDATA*>(pActor->userData);

		//지정된 그룹 제외
		if (_iGroupNum & pActorUserData->iObjectGroup)
		{
			isResult = false;
		}
		else
		{
			if (nullptr != _ppOutActorObject)
			{
				if (nullptr != pActorUserData)
					*_ppOutActorObject = pActorUserData->pOwner;
			}
			if (nullptr != _pOutPos)
				*_pOutPos = _float3(hit.touches[i].position.x, hit.touches[i].position.y, hit.touches[i].position.z);

			isResult = true;
			break;
		}
	}

	return isResult;
}

_bool CPhysx_Manager::RayCast(const _float3& _vOrigin, const _float3& _vRayDir, _float _fMaxDistance,list<CActorObject*>& _OutActors, list<RAYCASTHIT>& _OutRaycastHits)
{
	PxRaycastHit hitBuffer[10]; // 최대 10개까지 저장
	PxRaycastBuffer hit(hitBuffer, 10); // 버퍼 설정
	PxVec3 vOrigin = { _vOrigin.x,_vOrigin.y, _vOrigin.z };
	PxVec3 vRayDir = { _vRayDir.x, _vRayDir.y, _vRayDir.z };

	_bool isResult = m_pPxScene->raycast(vOrigin, vRayDir, _fMaxDistance, hit);
	for (PxU32 i = 0; i < hit.nbTouches; i++) {
		PxRigidActor* pActor = hit.touches[i].actor;
		ACTOR_USERDATA* pActorUserData = reinterpret_cast<ACTOR_USERDATA*>(pActor->userData);

		_OutActors.push_back(nullptr != pActorUserData ? pActorUserData->pOwner : nullptr);

		_OutRaycastHits.push_back({ _float3{ hit.touches[i].position.x, hit.touches[i].position.y, hit.touches[i].position.z }
		, _float3{hit.touches[i].normal.x,hit.touches[i].normal.y,hit.touches[i].normal.z}});
	}
	return isResult;
}

_bool CPhysx_Manager::Overlap(SHAPE_TYPE		_eShapeType ,SHAPE_DESC* _pShape, _fvector _vPos, list<CActorObject*>& _OutActors)
{
	// 1. 검사할 기하학: 반지름 1의 구
	PxGeometry* pxGeom = nullptr;
	switch (_eShapeType)
	{
	case Engine::SHAPE_TYPE::BOX:
	{
		SHAPE_BOX_DESC* pBoxDesc = static_cast<SHAPE_BOX_DESC*>(_pShape);
		pxGeom = new  PxBoxGeometry(pBoxDesc->vHalfExtents.x, pBoxDesc->vHalfExtents.y, pBoxDesc->vHalfExtents.z);
		break;
	}
	case Engine::SHAPE_TYPE::SPHERE:
	{
		SHAPE_SPHERE_DESC* pSphereDesc = static_cast<SHAPE_SPHERE_DESC*>(_pShape);
		pxGeom = new  PxSphereGeometry(pSphereDesc->fRadius);
		break;
	}
	case Engine::SHAPE_TYPE::CAPSULE:
	{
		SHAPE_CAPSULE_DESC* pCapsuleDesc = static_cast<SHAPE_CAPSULE_DESC*>(_pShape);
		pxGeom = new  PxCapsuleGeometry(pCapsuleDesc->fRadius, pCapsuleDesc->fHalfHeight);
		break;
	}
	default:
		break;
	}
	if (nullptr == pxGeom)
	{
		return false;
	}
	_bool isResult = Overlap(pxGeom, _vPos, _OutActors);
	delete pxGeom;
	return isResult;
}

_bool CPhysx_Manager::Overlap(PxGeometry* _pxGeom, _fvector _vPos, list<CActorObject*>& _OutActors)
{
	// 2. 기하학의 시작 위치와 회전
	PxTransform pxPose(PxVec3(XMVectorGetX(_vPos), XMVectorGetY(_vPos), XMVectorGetZ(_vPos)), PxQuat(PxIdentity));

	// 3. 결과를 저장할 overlap 버퍼 생성
	PxOverlapHit hits[10];
	PxOverlapBuffer pxOverlapBuffer(hits, 10);

	// 4. Overlap 쿼리 수행
	_bool isResult = m_pPxScene->overlap(*_pxGeom, pxPose, pxOverlapBuffer);

	if (isResult)
	{
		for (PxU32 i = 0; i < pxOverlapBuffer.nbTouches; i++)
		{
			PxRigidActor* pActor = pxOverlapBuffer.touches[i].actor;
			ACTOR_USERDATA* pActorUserData = reinterpret_cast<ACTOR_USERDATA*>(pActor->userData);

			_OutActors.push_back(nullptr != pActorUserData ? pActorUserData->pOwner : nullptr);
		}
	}
	return isResult;
}

HRESULT CPhysx_Manager::Initialize_Foundation()
{
	/* Create PxFoundation */
	m_pPxFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_Allocator, m_ErrorCallback);
	if (nullptr == m_pPxFoundation)
		return E_FAIL;
	return S_OK;
}

HRESULT CPhysx_Manager::Initialize_Physics()
{
	/* Setting World Desc */
	PxTolerancesScale TolerancesScale;
	TolerancesScale.length = 1.0f;
	TolerancesScale.speed = 9.81f;

	/* Create Physics */
	m_pPxPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_pPxFoundation, TolerancesScale, true, m_pPxPvd);


	/* PxCookingParams 설정 */
	PxCookingParams CookingParams(TolerancesScale);
	CookingParams.meshPreprocessParams = PxMeshPreprocessingFlag::eWELD_VERTICES; // 중복된 정점 병합
	CookingParams.meshWeldTolerance = 0.001f; // 병합 허용 오차
	
	// 4. PxCooking 생성
	m_pPxCooking = PxCreateCooking(PX_PHYSICS_VERSION, *m_pPxFoundation, CookingParams);
	if (nullptr == m_pPxCooking)
	{
		MSG_BOX("PxCreateCooking failed!");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CPhysx_Manager::Initialize_Scene()
{
	/* Setting Desc */
	PxSceneDesc SceneDesc(m_pPxPhysics->getTolerancesScale());
	SceneDesc.gravity = PxVec3(0.0f, -9.81f * 3.0f, 0.0f);
	SceneDesc.solverType = PxSolverType::eTGS;
	/* Create Dispatcher */

	m_pPxDefaultCpuDispatcher = PxDefaultCpuDispatcherCreate(2);
	if (nullptr == m_pPxDefaultCpuDispatcher)
		return E_FAIL;


	PxSceneLimits limits;
	limits.maxNbActors = 1000;     // 예상 액터 수
	limits.maxNbBodies = 100;      // 동적 바디 수
	limits.maxNbRegions = 4;       // 브로드페이스 영역 수
	SceneDesc.limits = limits;

	SceneDesc.cpuDispatcher = m_pPxDefaultCpuDispatcher;
	SceneDesc.filterShader = TWFilterShader;//TWFilterShader; //PxDefaultSimulationFilterShader;//; // 일단 기본값으로 생성 해보자.
	SceneDesc.simulationEventCallback = m_pPhysx_EventCallBack; // 일단 기본값으로 생성 해보자.
	SceneDesc.broadPhaseType = PxBroadPhaseType::eMBP;

	/* Create Scene */
	m_pPxScene = m_pPxPhysics->createScene(SceneDesc);
	if (nullptr == m_pPxScene)
		return E_FAIL;
	PxBounds3 regionBounds(PxVec3(-100.0f, -100.0f, -100.0f), PxVec3(100.0f, 100.0f, 100.0f));
	// PxBroadPhaseRegion 생성
	PxBroadPhaseRegion region;
	region.bounds = regionBounds;
	region.userData = nullptr; // 사용자 데이터가 필요하지 않으면 nullptr

	// Region 추가
	m_pPxScene->addBroadPhaseRegion(region);
	/* Setting Pvd */
	PxPvdSceneClient* pvdClient = m_pPxScene->getScenePvdClient();
#if defined(_DEBUG)
	if (pvdClient) {
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, false);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, false);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, false);
	}
#endif
#pragma region 추가적인 이벤트 처리나 flag 설정이 필요한 경우
	/* 추가적인 이벤트 처리나 flag 설정이 필요한 경우 */
	//// flags 설정
	//sceneDesc.flags |= PxSceneFlag::eENABLE_ACTIVE_ACTORS;  // 활성 액터 알림 활성화
	//sceneDesc.flags |= PxSceneFlag::eENABLE_CCD;           // 연속 충돌 감지 활성화

	// 시뮬레이션 이벤트 콜백 설정
	//class MySimulationEventCallback : public PxSimulationEventCallback
	//{
	//	// 콜백 함수들 구현
	//};
	//MySimulationEventCallback* callback = new MySimulationEventCallback();
	//sceneDesc.simulationEventCallback = callback;
#pragma endregion

	return S_OK;
}

HRESULT CPhysx_Manager::Initialize_Material()
{
	/* Actor_Enum에 있는 Material들을 생성한다. */

	for (_uint i = 0; i < (_uint)ACTOR_MATERIAL::CUSTOM; ++i)
	{
		_float3 vMaterialDesc = {};
		switch ((ACTOR_MATERIAL)i)
		{
		case Engine::ACTOR_MATERIAL::DEFAULT: // 일반 오브젝트 
			vMaterialDesc = { 0.7f, 0.8f, 0.1f };
			break;
		case Engine::ACTOR_MATERIAL::SLIPPERY: // 미끄러운
			vMaterialDesc = { 0.05f, 0.05f, 0.1f };
			break;
		case Engine::ACTOR_MATERIAL::BOUNCY: // 잘 튕기는
			vMaterialDesc = { 0.3f, 0.3f, 1.0f };
			break;
		case Engine::ACTOR_MATERIAL::STICKY: // 질퍽한
			vMaterialDesc = { 0.8f, 0.7f, 0.1f };
			break;
		case Engine::ACTOR_MATERIAL::NOFRICTION: // 노마찰
			vMaterialDesc = { 0.f, 0.f, 0.1f };
			break;
		case Engine::ACTOR_MATERIAL::CHARACTER_CAPSULE: // 캐릭터 캡슐(마찰꺼짐)
			vMaterialDesc = { 0, 0, 0 };
			break;
		case Engine::ACTOR_MATERIAL::NORESTITUTION: // 노반발력
			vMaterialDesc = { 0.7f, 0.8f,0.f };
			break;
		case Engine::ACTOR_MATERIAL::DOMINO: // 도미노용
			vMaterialDesc = { 0.5f, 0.6f,0.f };
			break;
		default:
			break;
		}

		m_pPxMaterial[i] = m_pPxPhysics->createMaterial(vMaterialDesc.x, vMaterialDesc.y, vMaterialDesc.z);
		if (Engine::ACTOR_MATERIAL::CHARACTER_CAPSULE == (ACTOR_MATERIAL)i)
			m_pPxMaterial[i]->setFlag(PxMaterialFlag::eDISABLE_FRICTION, true);
	}
	return S_OK;
}

HRESULT CPhysx_Manager::Initialize_PVD()
{
	m_pPxPvd = PxCreatePvd(*m_pPxFoundation);
	if (nullptr == m_pPxPvd)
		return E_FAIL;

	PxPvdTransport* pTransport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);


	if (false == m_pPxPvd->connect(*pTransport, PxPvdInstrumentationFlag::eALL))
	{
		PHYSX_RELEASE(m_pPxPvd);
		PHYSX_RELEASE(pTransport);
		return S_OK;
	}
		

	return S_OK;
}


CPhysx_Manager* CPhysx_Manager::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CPhysx_Manager* pInstance = new CPhysx_Manager(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CPhysx_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPhysx_Manager::Free()
{
	/////////////////////////////////
	/* Release 순서 건들지 마시오. */
	/////////////////////////////////


	// Shape User Data 정리
	Delete_ShapeUserData();

	// 1. Actor 및 Shape 관련 리소스 해제
	//if (m_pGroundPlane)
	//	m_pGroundPlane->release();
	if (m_pTestDesk)
	{
		m_pTestDesk->release();
		m_pTestDesk = nullptr;
	}

	// 2. Scene 및 Dispatcher 정리
	if (m_pPxScene)
	{
		m_pPxScene->release();
		m_pPxScene = nullptr;
	}
		
	if (m_pPxDefaultCpuDispatcher)
	{
		m_pPxDefaultCpuDispatcher->release();
		m_pPxDefaultCpuDispatcher = nullptr;
	}

	// 3. Material 정리
	for (_uint i = 0; i < (_uint)ACTOR_MATERIAL::CUSTOM; ++i)
	{
		if (m_pPxMaterial[i])
		{
			m_pPxMaterial[i]->release();
			m_pPxMaterial[i] = nullptr;
		}
	}

	// 4. Cooking 및 Physics 정리
	if (m_pPxCooking)
	{
		m_pPxCooking->release();
		m_pPxCooking = nullptr;
	}
	if (m_pPxPhysics)
	{
		m_pPxPhysics->release();
		m_pPxPhysics = nullptr;
	}

#ifdef _DEBUG

	// 5. PVD(PxVisualDebugger) 연결 해제 및 리소스 정리
	if (m_pPxPvd)
	{
		m_pPxPvd->disconnect(); // PVD 연결 해제

		if (auto pTransport = m_pPxPvd->getTransport())
		{
			m_pPxPvd->release();
			m_pPxPvd = nullptr;
			pTransport->release();
			pTransport = nullptr;
		}
	}
#endif // _DEBUG


	// 6. Foundation 정리
	if (m_pPxFoundation)
	{
		m_pPxFoundation->release();
		m_pPxFoundation = nullptr;
	}

	// 게임 및 DirectX 리소스 해제
	Safe_Release(m_pPhysx_EventCallBack);
	Safe_Release(m_pShader);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pGameInstance);

	// DirectX 리소스 해제 (가장 마지막)
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);

	// 부모 클래스 자원 해제
	__super::Free();

}
