#pragma once
#include "Component.h"

typedef struct tagShapeData
{
	tagShapeData() { XMStoreFloat4x4(&LocalOffsetMatrix, XMMatrixIdentity()); }
	SHAPE_TYPE		eShapeType = SHAPE_TYPE::LAST;
	SHAPE_DESC*		pShapeDesc = nullptr;
	_float3			vLocalOffset = { 0.0f, 0.0f, 0.0f };
	_float4x4		LocalOffsetMatrix = {};
	_bool			isTrigger = false;

	_bool			isShapeMaterial = false;
	ACTOR_MATERIAL	eMaterial = ACTOR_MATERIAL::DEFAULT;
	_uint			iShapeUse = 0;
}SHAPE_DATA;

typedef struct tagFilterData
{
	_uint MyGroup = 0;
	_uint OtherGroupMask = 0;
}FILTER_DATA;
BEGIN(Engine)
class CActorObject;
class ENGINE_DLL CActor abstract : public CComponent
{
public:
	typedef struct tagActorDesc
	{
		tagActorDesc() { XMStoreFloat4x4(&OffsetMatrix, XMMatrixIdentity()); }
		CActorObject*			pOwner = nullptr;
		vector<SHAPE_DATA>		ShapeDatas;
		_float4x4				OffsetMatrix = {};
		_bool					FreezeRotation_XYZ[3] = { false, true, false };
		_bool					FreezePosition_XYZ[3] = { false, false, false };
		
		FILTER_DATA				tFilterData = {};
	}ACTOR_DESC;
protected:
	CActor(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, ACTOR_TYPE _eActorType);
	CActor(const CActor& _Prototype);
	virtual ~CActor() = default;

public:
	HRESULT						Initialize_Prototype();
	HRESULT						Initialize(void* _pArg);
	void						Priority_Update(_float _fTimeDelta) override;
	void						Update(_float _fTimeDelta)  override;
	void						Late_Update(_float _fTimeDelta)  override;
#ifdef _DEBUG
public:
	virtual HRESULT				Render();
#endif
public:/* Default PhysX */
	virtual void				Set_LinearVelocity(_vector _vDirection, _float _fVelocity) { return; }
	virtual void				Set_AngularVelocity(const _float3& _vAngularVelocity) { return; }
	virtual void				Add_Force(const _float3& _vForce) { return; };// 일반적인 힘
	virtual void				Add_Impulse(const _float3& _vForce) { return; }; // 강한 힘
	
public:
	virtual HRESULT				Change_Coordinate(COORDINATE _eCoordinate, _float3* _pNewPosition = nullptr);
public:
	virtual void				Turn_TargetDirection(_vector _vDirection) { return; }

public: /* Event Filter */
	void						Setup_SimulationFiltering(_uint _iMyGroup, _uint _iOtherGroupMask, _bool _isRunTime = false);

public:
	// Get
	ACTOR_TYPE					Get_ActorType() const { return m_eActorType; }
	// Set 
protected:
	PxRigidActor*				m_pActor = nullptr; 
	CActorObject*				m_pOwner = nullptr;

protected: /* Actor Default Data */
	ACTOR_TYPE					m_eActorType = ACTOR_TYPE::LAST;	
	_float4x4					m_OffsetMatrix = {};
	
protected:
	ACTOR_USERDATA				m_UserData;

private:
	/* Active 변경시 호출되는 함수 추가. */
	virtual void Active_OnEnable() override;
	virtual void Active_OnDisable() override;

#ifdef _DEBUG
protected:
	vector<PxShape*>						m_pTriggerShapes;	
	PrimitiveBatch<VertexPositionColor>*	m_pBatch = nullptr;
	BasicEffect*							m_pEffect = nullptr;
	ID3D11InputLayout*						m_pInputLayout = nullptr;
	_float4									m_vDebugColor = { 0.0f, 1.0f, 0.0f, 1.0f };
#endif // _DEBUG



private:
	// 1. ShapeInfos 로 Shape을 생성
	HRESULT						Ready_Shapes(const vector<SHAPE_DATA>& ShapeDescs);
	// 2. Shape 정보와 Actor Desc를 기반으로 Actor를 생성.
	HRESULT						Ready_Actor(ACTOR_DESC* _pActorDesc);

public:
	void						Free() override;
};
END
