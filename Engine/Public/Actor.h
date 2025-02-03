#pragma once
#include "Component.h"

typedef struct tagFilterData
{
	_uint MyGroup = 0;
	_uint OtherGroupMask = 0;
}FILTER_DATA;

typedef struct tagShapeData
{
	tagShapeData() { XMStoreFloat4x4(&LocalOffsetMatrix, XMMatrixIdentity()); }
	SHAPE_TYPE		eShapeType = SHAPE_TYPE::LAST;
	SHAPE_DESC*		pShapeDesc = nullptr;
	_float4x4		LocalOffsetMatrix = {};
	_bool			isTrigger = false;
	_bool			isSceneQuery = false;
	_bool			isVisual = true;
	ACTOR_MATERIAL	eMaterial = ACTOR_MATERIAL::DEFAULT;
	_uint			iShapeUse = 0;
	FILTER_DATA		FilterData = {};
}SHAPE_DATA;


BEGIN(Engine)
class CActorObject;
class ENGINE_DLL CActor abstract : public CComponent
{
public:
	typedef struct tagActorDesc
	{
		tagActorDesc() { XMStoreFloat4x4(&ActorOffsetMatrix, XMMatrixIdentity()); }
		CActorObject*			pOwner = nullptr;
		vector<SHAPE_DATA>		ShapeDatas;
		_float4x4				ActorOffsetMatrix = {};
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

	_float3						Get_GlobalPose();
	void						Set_GlobalPose(const _float3& _vPos);

	void						Set_PxActorDisable();
	void						Set_PxActorEnable();
public:
	virtual HRESULT				Change_Coordinate(COORDINATE _eCoordinate, _float3* _pNewPosition = nullptr);
	HRESULT						Add_Shape(const SHAPE_DATA& _ShapeData);
	HRESULT						Delete_Shape(_int _iShapeIndex);
public:
	virtual void				Turn_TargetDirection(_vector _vDirection) { return; }

public: /* Event Filter */
	void						Setup_SimulationFiltering(_uint _iMyGroup, _uint _iOtherGroupMask, _bool _isRunTime = false);

public:
	// Get
	const vector<PxShape*>&		Get_Shapes() { return m_Shapes; }
	ACTOR_TYPE					Get_ActorType() const { return m_eActorType; }

	// Set 
	virtual void				Set_ActorOffsetMatrix(_fmatrix _ActorOffsetMatrix); // 특별한 경우 아니면 사용을 비권장하겠음. 버그있는듯함.

	HRESULT						Set_ShapeLocalOffsetMatrix(_int _iShapeIndex, _fmatrix _ShapeLocalOffsetMatrix); // 크기는 안되고 위치랑 회전만 가능함.
	HRESULT						Set_ShapeLocalOffsetPosition(_int _iShapeIndex, const _float3& _vOffsetPos); // shape offset 위치 변경.
	HRESULT						Set_ShapeLocalOffsetQuaternion(_int _iShapeIndex, const _float4& _vQuat); // 쿼터니언
	HRESULT						Set_ShapeLocalOffsetPitchYawRoll(_int _iShapeIndex, const _float3& _vPitchYawRoll); // xyz 회전량 넣으면 쿼터니언으로 변환해서 넣음.
	HRESULT						Set_ShapeGeometry(_int _iShapeIndex, PxGeometryType::Enum _eType, SHAPE_DESC* _pDesc); // shape 크기변경
protected:
	PxRigidActor*				m_pActor = nullptr; 
	CActorObject*				m_pOwner = nullptr;

protected:
	vector<PxShape*>			m_Shapes;

protected: /* Actor Default Data */
	ACTOR_TYPE					m_eActorType = ACTOR_TYPE::LAST;	
	_float4x4					m_OffsetMatrix = {};

private:
	/* Active 변경시 호출되는 함수 추가. */
	virtual void Active_OnEnable() override;
	virtual void Active_OnDisable() override;

#ifdef _DEBUG
protected:
	PrimitiveBatch<VertexPositionColor>*	m_pBatch = nullptr;
	BasicEffect*							m_pEffect = nullptr;
	ID3D11InputLayout*						m_pInputLayout = nullptr;
	_float4									m_vDebugColor = { 0.0f, 1.0f, 0.0f, 1.0f };
#endif // _DEBUG



private:
	// 1. ShapeInfos 로 Shape을 생성
	HRESULT						Ready_Shapes(const vector<SHAPE_DATA>& ShapeDatas);
	// 2. Shape 정보와 Actor Desc를 기반으로 Actor를 생성.
	HRESULT						Ready_Actor(ACTOR_DESC* _pActorDesc);

public:
	void						Free() override;
};
END
//