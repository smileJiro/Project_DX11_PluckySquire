#pragma once
#include "Base.h"
BEGIN(Engine)
class CGameInstance;
class CVIBuffer_PxDebug;
class CShader;
class CGameObject;
class CPhysx_EventCallBack;
class CPhysx_Manager final : public CBase
{
private:
	CPhysx_Manager(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CPhysx_Manager() = default;

private:
	HRESULT						Initialize(); /* PhysX 초기화 */
public:
	void						Update(_float _fTimeDelta);

	HRESULT						Render();
public:
	PxPhysics*					Get_Physics() const { return m_pPxPhysics; }
	PxCooking*					Get_Cooking() const { return m_pPxCooking; }
	PxScene*					Get_Scene() const { return m_pPxScene; }
	PxMaterial*					Get_Material(ACTOR_MATERIAL _eType) const {	return m_pPxMaterial[(_uint)_eType]; }
	_uint						Create_ShapeID() { return m_iShapeInstanceID++; };
private:
	ID3D11Device*				m_pDevice = nullptr;
	ID3D11DeviceContext*		m_pContext = nullptr;
	CGameInstance*				m_pGameInstance = nullptr;
	
private: /* Core PhysX */
	PxFoundation*				m_pPxFoundation = nullptr;
	PxPhysics*					m_pPxPhysics = nullptr;
	PxDefaultCpuDispatcher*		m_pPxDefaultCpuDispatcher = nullptr;

private: /* PhysX Cooking */
	PxCooking*					m_pPxCooking = nullptr;

private:/* Scene (추후 분리 예정)*/
	PxScene*					m_pPxScene = nullptr;
	PxMaterial*					m_pPxMaterial[(_uint)ACTOR_MATERIAL::CUSTOM] = {};

private: /* Visual Debugger */
	PxPvd*						m_pPxPvd = nullptr;

private: /* iNumThreads */
	static constexpr _uint		s_iNumThreads = 4;

private:
	PxDefaultAllocator			m_Allocator = {};
	PxDefaultErrorCallback		m_ErrorCallback = {};

private:
	static _uint				m_iShapeInstanceID;

private: /* Event CallBack Class */
	CPhysx_EventCallBack*		m_pPhysx_EventCallBack = nullptr;

public:
	void Add_ShapeUserData(SHAPE_USERDATA* _pUserData);
	void Delete_ShapeUserData();
	_bool RayCast_Nearest(const _float3& _vOrigin, const _float3& _vRayDir, _float _fMaxDistance, _float3* _pOutPos = nullptr, CActorObject** _ppOutActorObject = nullptr);

private: /* SHAPE_USERDATA : 메모리 해제용 */
	vector<SHAPE_USERDATA*> m_pShapeUserDatas;

private: /* Test Object */
	PxRigidStatic*				m_pGroundPlane = nullptr;
	PxRigidStatic*				m_pTestDesk = nullptr;
	CVIBuffer_PxDebug*			m_pVIBufferCom = nullptr;
	CShader*					m_pShader = nullptr;



public:
	void						Set_DebugRender(_bool _isDebugRender) { m_isDebugRender = _isDebugRender; }
private:
	_bool						m_isDebugRender = true;
	_float						m_fTimeAcc = 0.0f;
	_float						m_fFixtedTimeStep= 1.f/60.f;
private:
	HRESULT						Initialize_Foundation();
	HRESULT						Initialize_Physics();
	HRESULT						Initialize_Scene();
	HRESULT						Initialize_Material();
	HRESULT						Initialize_PVD();

public:
	void Set_Player(CGameObject* _pPlayer) { 
		if (nullptr != _pPlayer)
			Safe_Release(m_pPlayer);

		m_pPlayer = _pPlayer;
		Safe_AddRef(m_pPlayer);
	};

private:
	CGameObject*				m_pPlayer = nullptr;

public:
	static CPhysx_Manager*	Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void			Free(); /* PhysX 종료 후 객체 소멸 */
};
END
