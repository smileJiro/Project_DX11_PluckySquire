#pragma once
#include "Actor.h"
BEGIN(Engine)
class ENGINE_DLL CActor_Dynamic final : public CActor
{
private:
	CActor_Dynamic(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, ACTOR_TYPE _eActorType);
	CActor_Dynamic(const CActor_Dynamic& _Prototype);
	virtual ~CActor_Dynamic() = default;

public:
	HRESULT						Initialize_Prototype();
	HRESULT						Initialize(void* _pArg);
	void						Priority_Update(_float _fTimeDelta) override;
	void						Update(_float _fTimeDelta)  override;
	void						Late_Update(_float _fTimeDelta)  override;
public:
	HRESULT						Change_Coordinate(COORDINATE _eCoordinate, _float3* _pNewPosition = nullptr) override;
	_bool						Start_ParabolicTo(_vector _vPosition, _float _fLaunchRadianAngle,  _float _fGravityMag = 9.81f*3.f);

public:
	//Get
	_vector						Get_LinearVelocity();
	_vector						Get_AngularVelocity();

	//Set
	void							Set_Kinematic();
	void							Set_Dynamic();
	void							Set_SleepThreshold(_float _fThreshold);

	_bool						Is_Sleeping();
	virtual void				Set_LinearVelocity(_fvector _vDirection, _float _fVelocity);
	virtual void				Set_LinearVelocity(_fvector _vVelocity);
	virtual void				Set_AngularVelocity(const _float3& _vAngularVelocity);
	virtual void				Set_AngularVelocity(_fvector _vAngularVelocity);
	virtual void				Set_Rotation(_fvector _vAxis, _float _fRadian);
	virtual void				Set_Rotation(_fvector _vLook);
	virtual void				Set_LinearDamping(_float _fValue);
	virtual void				Set_AngularDamping(_float _fValue);
	void						Set_Gravity(_bool _isGravity);
	void						Freeze_Rotation(_bool _bFreezeX, _bool _bFreezeY, _bool _bFreezeZ);
	void						Freeze_Position(_bool _bFreezeX, _bool _bFreezeY, _bool _bFreezeZ);

	virtual void				Add_Force(const _float3& _vForce);// ÀÏ¹ÝÀûÀÎ Èû
	virtual void				Add_Impulse(const _float3& _vForce); // °­ÇÑ Èû
	virtual void				Add_Torque(const _float3& _vTorque); // °­ÇÑ Èû


	virtual void				Set_ActorOffsetMatrix(_fmatrix _ActorOffsetMatrix) override;
private:

public:
	static CActor_Dynamic*		Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, _bool _isKinematic = false);
	CComponent*					Clone(void* _pArg) override;
	void						Free() override;
};

END