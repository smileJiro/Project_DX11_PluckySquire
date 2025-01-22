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
	virtual void				Set_LinearVelocity(_vector _vDirection, _float _fVelocity);
	virtual void				Set_AngularVelocity(const _float3& _vAngularVelocity);
	virtual void				Add_Force(const _float3& _vForce);// ÀÏ¹ÝÀûÀÎ Èû
	virtual void				Add_Impulse(const _float3& _vForce); // °­ÇÑ Èû

public:
	virtual void				Turn_TargetDirection(_vector _vDirection);

public:
	static CActor_Dynamic*		Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, _bool _isKinematic = false);
	CComponent*					Clone(void* _pArg) override;
	void						Free() override;
};

END