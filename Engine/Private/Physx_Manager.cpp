#include "Physx_Manager.h"

CPhysx_Manager::CPhysx_Manager(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
}

HRESULT CPhysx_Manager::Initialize()
{
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


	PxSimulationEventCallback;
	return S_OK;
}

void CPhysx_Manager::Update(_float _fTimeDelta)
{
	m_pPxScene->simulate(1.0f / 60.f);

	m_pPxScene->fetchResults(true);
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
	SceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);

	/* Create Dispatcher */
	m_pPxDefaultCpuDispatcher = PxDefaultCpuDispatcherCreate(2);
	if (nullptr == m_pPxDefaultCpuDispatcher)
		return E_FAIL;

	SceneDesc.cpuDispatcher = m_pPxDefaultCpuDispatcher;
	SceneDesc.filterShader = PxDefaultSimulationFilterShader; // 일단 기본값으로 생성 해보자.
	
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
			vMaterialDesc = {0.5f, 0.4f, 0.2f};
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

	PHYSX_RELEASE(m_pGroundPlane); /* Scene 삭제 전 정리*/
	PHYSX_RELEASE(m_pPxScene);
	PHYSX_RELEASE(m_pPxDefaultCpuDispatcher);/* Scene 삭제후 곧바로 정리*/
	for(_uint i =0; i < (_uint)ACTOR_MATERIAL::CUSTOM; ++i)
		PHYSX_RELEASE(m_pPxMaterial[i]); /* Scene 삭제 후 정리 해야함. */
	PHYSX_RELEASE(m_pPxPhysics); /* Pvd보다 반드시 먼저 삭제되어야함. */
	if (nullptr != m_pPxPvd)
	{
		if (auto pTransport = m_pPxPvd->getTransport())
		{
			PHYSX_RELEASE(m_pPxPvd); /* Pvd를 정리하고 */
			PHYSX_RELEASE(pTransport); /* Transport를 정리 */
		}
		
	}
	PHYSX_RELEASE(m_pPxFoundation); /* 가장 마지막 정리 */

	__super::Free();
}
