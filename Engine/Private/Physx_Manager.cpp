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

	if (FAILED(Initialize_PVD()))
		return E_FAIL;
	
	if (FAILED(Initialize_Physics()))
		return E_FAIL;

	if (FAILED(Initialize_Scene()))
		return E_FAIL;

	if (FAILED(Initialize_Material()))
		return E_FAIL;


	/* Test Code */
	m_pGroundPlane = PxCreatePlane(*m_pPxPhysics, PxPlane(0, 1, 0, 99), *m_pPxMaterial[(_uint)ACTOR_MATERIAL::DEFAULT]);
	m_pPxScene->addActor(*m_pGroundPlane);

	_float4x4 matTest = {};
	PxMat44 matPx;
	XMStoreFloat4x4(&matTest, XMMatrixIdentity());
	PxTransform transform(PxVec3(0.0f, -10.0f, 0.0f)); // 위치: (0, 0, 0)

	// PxRigidStatic 객체 생성
	m_pTestDesk = m_pPxPhysics->createRigidStatic(transform);
	PxBoxGeometry boxGeometry(PxVec3(100.0f, 10.0f, 100.0f));
	PxRigidActorExt::createExclusiveShape(*m_pTestDesk, boxGeometry, *m_pPxMaterial[(_uint)ACTOR_MATERIAL::DEFAULT]);
	m_pTestDesk->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);

	/* 충돌 필터에 대한 세팅 ()*/
	PxFilterData FilterData;
	FilterData.word0 = 0x08;
	FilterData.word1 = 0x02; // OhterGroupMask는 비트연산자를 통해 여러 그룹을 포함 가능.

	PxU32 iNumShapes = m_pTestDesk->getNbShapes();

	vector<PxShape*> pShapes;
	pShapes.resize(iNumShapes);
	m_pTestDesk->getShapes(pShapes.data(), iNumShapes);

	for (auto& pShape : pShapes)
	{
		pShape->setSimulationFilterData(FilterData);
	}

	m_pPxScene->addActor(*m_pTestDesk);

	//m_pRigidDynamic = m_pPxPhysics->createRigidDynamic(transform);
	//PxCapsuleGeometry CapsuleGeometry2(0.5f, 0.2f);
	//m_pPxshape = PxRigidActorExt::createExclusiveShape(*m_pRigidDynamic, CapsuleGeometry2, *m_pPxMaterial);
	//m_pRigidDynamic->setGlobalPose(PxTransform(0.0f, 15.0f, 0.0f));
	//m_pRigidDynamic->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, true);
	//m_pRigidDynamic->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, true);
	//m_pRigidDynamic->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, false);
	//m_pPxScene->addActor(*m_pRigidDynamic);
	//
	// 필요한 시각화 기능 활성화
	m_pPxScene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0f);
	m_pPxScene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f); // 충돌 형태 시각화
	m_pPxScene->setVisualizationParameter(PxVisualizationParameter::eJOINT_LOCAL_FRAMES, 1.0f); // 관절 로컬 프레임
	m_pPxScene->setVisualizationParameter(PxVisualizationParameter::eACTOR_AXES, 1.0f);

	
	
	/* Debug */
	m_pVIBufferCom = CVIBuffer_PxDebug::Create(m_pDevice, m_pContext, 3000);
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

#ifdef _DEBUG
	
	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../../EngineSDK/hlsl/Shader_Debug.hlsl"), VTXPOSCOLOR::Elements, VTXPOSCOLOR::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;

#endif
	return S_OK;
}

void CPhysx_Manager::Update(_float _fTimeDelta)
{
	
	//_float fUpdateTime = 1.0f / 60.f;
	//m_pPxScene->simulate(fUpdateTime);

	//if (m_pPxScene->fetchResults(true))
	//{
	//	if(nullptr != m_pPhysx_EventCallBack)
	//		m_pPhysx_EventCallBack->Update();

	//	const PxRenderBuffer& RenderBuffer = m_pPxScene->getRenderBuffer();
	//	m_pVIBufferCom->Update_PxDebug(RenderBuffer);
	//}
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

	return S_OK;
}

HRESULT CPhysx_Manager::Initialize_Scene()
{
	/* Setting Desc */
	PxSceneDesc SceneDesc(m_pPxPhysics->getTolerancesScale());
	SceneDesc.gravity = PxVec3(0.0f, -9.81f * 3.0f, 0.0f);

	/* Create Dispatcher */
	m_pPxDefaultCpuDispatcher = PxDefaultCpuDispatcherCreate(s_iNumThreads);
	if (nullptr == m_pPxDefaultCpuDispatcher)
		return E_FAIL;

	SceneDesc.cpuDispatcher = m_pPxDefaultCpuDispatcher;
	SceneDesc.filterShader = TWFilterShader;//TWFilterShader; //PxDefaultSimulationFilterShader;//; // 일단 기본값으로 생성 해보자.
	SceneDesc.simulationEventCallback = m_pPhysx_EventCallBack; // 일단 기본값으로 생성 해보자.
	
	/* Create Scene */
	m_pPxScene = m_pPxPhysics->createScene(SceneDesc);
	if (nullptr == m_pPxScene)
		return E_FAIL;

	/* Setting Pvd */
	PxPvdSceneClient* pvdClient = m_pPxScene->getScenePvdClient();
	if (pvdClient)
	{
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}

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
			vMaterialDesc = { 0.7f, 0.5f, 0.1f };
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
		default:
			break;
		}

		m_pPxMaterial[i] = m_pPxPhysics->createMaterial(vMaterialDesc.x, vMaterialDesc.y, vMaterialDesc.z);
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
	// PhysX 리소스 해제
	m_pGroundPlane->release();
	m_pTestDesk->release();
	m_pPxScene->release();
	m_pPxDefaultCpuDispatcher->release();
	/* Scene 삭제후 곧바로 정리*/
	for(_uint i =0; i < (_uint)ACTOR_MATERIAL::CUSTOM; ++i)
		m_pPxMaterial[i]->release();

	m_pPxPhysics->release();
	if (nullptr != m_pPxPvd)
	{
		m_pPxPvd->disconnect(); // 연결 해제
		if (auto pTransport = m_pPxPvd->getTransport())
		{
			m_pPxPvd->release();
			pTransport->release();
		}
	}
	m_pPxFoundation->release();

	// 게임 및 DirectX 리소스 해제
	Safe_Release(m_pPhysx_EventCallBack);
	Safe_Release(m_pPlayer);
	Safe_Release(m_pShader);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pGameInstance);

	// DirectX 리소스 해제 (가장 마지막)
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);

	// 부모 클래스 자원 해제
	__super::Free();

}
