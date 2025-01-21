#pragma once
#include "Component.h"

typedef struct tagShapeData
{
	tagShapeData() { XMStoreFloat4x4(&LocalOffsetMatrix, XMMatrixIdentity()); }
	SHAPE_TYPE	eShapeType = SHAPE_TYPE::LAST;
	SHAPE_DESC* pShapeDesc = {};
	_float3		vLocalOffset = { 0.0f, 0.0f, 0.0f };
	_float4x4	LocalOffsetMatrix = {};
	_bool		isTrigger = false; /* 해당 Shape의 Trigger 여부*/

	_bool		isShapeMaterial = false;
	ACTOR_MATERIAL eMaterial = ACTOR_MATERIAL::DEFAULT;
}SHAPE_DATA;

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

public:/* Default PhysX */
	virtual void				Set_LinearVelocity(_vector _vDirection, _float _fVelocity) { return; }
	virtual void				Set_AngularVelocity(const _float3& _vAngularVelocity) { return; }
	virtual void				Add_Force(const _float3& _vForce) { return; };// 일반적인 힘
	virtual void				Add_Impulse(const _float3& _vForce) { return; }; // 강한 힘

public:
	virtual void				Turn_TargetDirection(_vector _vDirection) { return; }

public:
	// Get
	ACTOR_TYPE					Get_ActorType() const { return m_eActorType; }
	// Set 
protected:
	PxRigidActor*				m_pActor = nullptr; 
	CActorObject*				m_pOwner = nullptr;

protected: /* Actor Default Data */
	ACTOR_TYPE					m_eActorType = ACTOR_TYPE::LAST;	
	vector<PxShape*>			m_pShapes;
	_float4x4					m_OffsetMatrix = {};

	
protected: /* Collision Filter */


private:
	// 1. ShapeInfos 로 Shape을 생성
	HRESULT						Ready_Shapes(const vector<SHAPE_DATA>& ShapeDescs);
	// 2. Shape 정보와 Actor Desc를 기반으로 Actor를 생성.
	HRESULT						Ready_Actor(ACTOR_DESC* _pActorDesc);

public:
	void						Free() override;
};
END
