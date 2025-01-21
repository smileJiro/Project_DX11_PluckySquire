#pragma once
#include "Base.h"
BEGIN(Engine)
class CGameInstance;
class CVIBuffer_PxDebug;
class CShader;
class CGameObject;
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
	PxScene*					Get_Scene() const { return m_pPxScene; }
	PxMaterial*					Get_Material(ACTOR_MATERIAL _eType) const {	return m_pPxMaterial[(_uint)_eType]; }

private:
	ID3D11Device*				m_pDevice = nullptr;
	ID3D11DeviceContext*		m_pContext = nullptr;
	CGameInstance*				m_pGameInstance = nullptr;
	
private: /* Core PhysX */
	PxFoundation*				m_pPxFoundation = nullptr;
	PxPhysics*					m_pPxPhysics = nullptr;
	PxDefaultCpuDispatcher*		m_pPxDefaultCpuDispatcher = nullptr;

private:/* Scene (추후 분리 예정)*/
	PxScene*					m_pPxScene = nullptr;
	PxMaterial*					m_pPxMaterial[(_uint)ACTOR_MATERIAL::CUSTOM] = {};

private: /* Visual Debugger */
	PxPvd*						m_pPxPvd = nullptr;

private: /* iNumThreads */
	static constexpr _uint		s_iNumThreads = 2;

private:
	PxDefaultAllocator			m_Allocator = {};
	PxDefaultErrorCallback		m_ErrorCallback = {};

private: /* Test Object */
	PxRigidStatic*				m_pGroundPlane = nullptr;
	PxRigidStatic*				m_pTestDesk = nullptr;
	CVIBuffer_PxDebug*			m_pVIBufferCom = nullptr;
	CShader*					m_pShader = nullptr;

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
