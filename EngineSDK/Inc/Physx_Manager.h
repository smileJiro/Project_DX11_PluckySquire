#pragma once
#include "Base.h"
BEGIN(Engine)
class CGameInstance;
class CPhysx_Manager final : public CBase
{
private:
	CPhysx_Manager(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CPhysx_Manager() = default;

private:
	HRESULT Initialize(); /* PhysX 초기화 */
	
public:
	void Update(_float _fTimeDelta);

private:
	HRESULT Initialize_Foundation();
	HRESULT Initialize_Physics();
	HRESULT Initialize_Scene();
	HRESULT Initialize_Material();
	HRESULT Initialize_PVD();

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
	PxMaterial*					m_pPxMaterial = nullptr;

private: /* Visual Debugger */
	PxPvd*						m_pPxPvd = nullptr;

private: /* iNumThreads */
	static constexpr _uint		s_iNumThreads = 2;

private:
	PxDefaultAllocator			m_Allocator = {};
	PxDefaultErrorCallback		m_ErrorCallback = {};



private: /* Test Object */
	PxRigidStatic*				m_pGroundPlane = nullptr;
public:
	static CPhysx_Manager*	Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void			Free(); /* PhysX 종료 후 객체 소멸 */
};
END
