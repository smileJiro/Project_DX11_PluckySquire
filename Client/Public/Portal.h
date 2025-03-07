#pragma once
#include "ContainerObject.h"
#include "Interactable.h"

BEGIN(Engine)
class CCollider;
class CEffect_System;
END

BEGIN(Client)
class CPortal abstract : public CContainerObject, public virtual IInteractable
{
public :
	enum PORTAL_TYPE
	{
		PORTAL_DEFAULT,
		PORTAL_ARROW,
		PORTAL_CANNON,
		PORTAL_LAST
	};
protected : 
	enum PORTAL_PART
	{
		PORTAL_PART_2D = 1,
		PORTAL_PART_3D, // 안쓸듯 !? 
		PORTAL_PART_EFFECT,
		PORTAL_PART_LAST
	};

public :
	typedef struct tagPortalDESC : public CContainerObject::CONTAINEROBJ_DESC
	{
		_float	fTriggerRadius;
		_uint	iPortalIndex = {};
		_bool   isFirstActive = false;
	}PORTAL_DESC;
protected:
	CPortal(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CPortal(const CPortal& _Prototype);
	virtual ~CPortal() = default;

public:
	virtual HRESULT			Initialize_Prototype(); 
	virtual HRESULT			Initialize(void* _pArg); 
	virtual void			Priority_Update(_float _fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;


	// 해당 포탈이 들어있는 섹션의 worldPostionMap이 설정되었을 때 호출되는 함수
	virtual HRESULT			Setup_3D_Postion();
	virtual HRESULT			Init_Actor();
	void					Use_Portal(CPlayer* _pUser);
protected:
	virtual HRESULT			Ready_Components(PORTAL_DESC* _pDesc);
	HRESULT					Ready_Particle();

	HRESULT					Change_Coordinate(COORDINATE _eCoordinate, _float3* _pNewPosition) override;

	virtual void			Active_OnDisable() override;
	virtual void			Active_OnEnable() override;


	// intractable 함수, 
public :
	virtual void			Interact(CPlayer* _pUser) {}
	virtual _bool			Is_Interactable(CPlayer* _pUser) { return false; }
	virtual _float			Get_Distance(COORDINATE _eCoord, CPlayer* _pUser) { return 0.f; }

	virtual void			On_InteractionStart(CPlayer* _pPlayer)override {}
	virtual void			On_InteractionEnd(CPlayer* _pPlayer) override {}

	_bool					Is_FirstActive() { return m_isFirstActive; }
	void					Set_FirstActive(_bool _bFirstActive);

	_uint					Get_PortalIndex() { return m_iPortalIndex; };
	void					Set_PortalIndex(_uint _iIndex) { m_iPortalIndex = _iIndex; }

	NORMAL_DIRECTION		Get_PortalNormal();

	PORTAL_TYPE				m_ePortalType = PORTAL_LAST;

	CCollider*				m_pColliderCom = { nullptr };

	_bool					m_isReady_3D = false;
	_bool					m_isFirstActive = true;	
	_float					m_fTriggerRadius = {};
	_uint					m_iPortalIndex = {};
	CEffect_System*			m_pEffectSystem = { nullptr };

public:
	void					Free() override;
};

END