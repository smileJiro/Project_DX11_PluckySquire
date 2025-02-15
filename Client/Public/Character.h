#pragma once
#include "ContainerObject.h"

BEGIN(Client)
typedef struct tagCharacterStat
{
	_float fDamg = 1.f;
	_float fHP = 12.f;
}STAT;
class CCharacter abstract :   public CContainerObject
{
public:
	typedef struct tagCharacterDesc : public CContainerObject::CONTAINEROBJ_DESC
	{
		
	}CHARACTER_DESC;

protected:
	CCharacter(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CCharacter(const CCharacter& _Prototype);
	virtual ~CCharacter() = default;

public:
	virtual void			Update(_float _fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;
public:
	STAT& Get_Stat() { return m_tStat; }

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
public:	
	virtual void Free() override;
};

END