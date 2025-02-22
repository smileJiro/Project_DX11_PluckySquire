#pragma once
#include "Base.h"
#include "physx/PxContactModifyCallback.h"

BEGIN(Engine)

class ENGINE_DLL CModifiableContacts
{
public:
	CModifiableContacts(PxContactSet& contactSet);
	~CModifiableContacts() = default;

public:
	_float Get_DynamicFriction(_uint _iIndex);
	_float Get_StaticFriction(_uint _iIndex);
	_float Get_Restitution(_uint _iIndex);
	_vector Get_Normal(_uint _iIndex);
	_vector Get_Point(_uint _iIndex);
	_vector Get_Velocity(_uint _iIndex);
	_uint Get_ContactCount();
		 
	void Set_DynamicFriction(_uint _iIndex, _float _fFriction);
	void Set_StaticFriction(_uint _iIndex, _float _fFriction);
	void Set_Restitution(_uint _iIndex, _float _fRestitution);
	void Set_Normal(_uint _iIndex, _fvector _vNormal);
	void Set_Normal(_uint _iIndex, _float3 _vNormal);
	void Set_Point(_uint _iIndex, _fvector _vPoint);
	void Set_Point(_uint _iIndex, _float3 _vPoint);
	void Set_Velocity(_uint _iIndex, _fvector _vVelocity);
	void Set_Velocity(_uint _iIndex, _float3 _vVelocity);
	void Ignore(_uint _iIndex);
private:
	PxContactSet& m_ContactSet;
};

class CPhysx_ContactModifyCallback :
    public PxContactModifyCallback, public CBase
{
private:
	CPhysx_ContactModifyCallback();
	virtual ~CPhysx_ContactModifyCallback() = default;
public:
    virtual void onContactModify(PxContactModifyPair* pairs, PxU32 count) override;
   
public:
	static CPhysx_ContactModifyCallback* Create();
	void Free() override;
};

END