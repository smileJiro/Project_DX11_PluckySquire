#pragma once
#include "ContainerObject.h"

BEGIN(Client)
typedef struct tagCharacterStat
{
	_int iDamg = 1;
	_int iHP = 12;
	_int iMaxHP = 12;
}STAT;
class CCharacter abstract :   public CContainerObject
{
public:
	typedef struct tagCharacterDesc : public CContainerObject::CONTAINEROBJ_DESC
	{
		_float _fStepSlopeThreshold = 0.1f;
	}CHARACTER_DESC;

protected:
	CCharacter(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CCharacter(const CCharacter& _Prototype);
	virtual ~CCharacter() = default;

public:
	virtual HRESULT			Initialize(void* _pArg) override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;

	virtual void OnContact_Modify(const COLL_INFO& _My, const COLL_INFO& _Other, CModifiableContacts& _ModifiableContacts)override;

public:
	STAT& Get_Stat() { return m_tStat; }
	_float Get_StepSlopeThreshold() { return m_fStepSlopeThreshold; }

	void Stop_Rotate();
	void Stop_Move();
	void Stop_MoveXZ();

	_bool Move_To(_fvector _vPosition, _float _fEpsilon = 0.5f);
	_bool Check_Arrival(_fvector _vPosition, _float _fEpsilon = 0.5f);
	//캐릭터 기준 _vDir 방향을 바라보게 하는 함수. Y축으로만 회전함.
	void LookDirectionXZ_Kinematic(_fvector _vDir);
	void LookDirectionXZ_Dynamic(_fvector _vDir);
	//속도는 degree로 줘야함 (초당), 목표 방향에 도달시 true 리턴
	_bool Rotate_To(_fvector _vDirection, _float _fSpeed);
	_bool Rotate_To_Radians(_fvector _vDirection, _float _fSpeed);

	void KnockBack(_fvector _vForce);

protected:
	STAT m_tStat;
	_vector m_vLookBefore = { 0,0,-1 };

	_float3 m_vKnockBackDirection = {};
	_bool	m_isKnockBack = {};
	_float m_fKnockBackForce = {};
	_float m_fKnockBackAccTime = {};
	_float m_fStepSlopeThreshold = 0.1f;

public:	
	virtual void Free() override;
};

END