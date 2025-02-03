#pragma once
#include "ContainerObject.h"

BEGIN(Client)
typedef struct tagCharacterStat
{
	_float fJumpPower = 0;
	_float fMoveSpeed = 0;
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
	STAT& Get_Stat(COORDINATE _eCoord) { return m_tStat[_eCoord]; }

	void Stop_Rotate();
	void Stop_Move();
	void Add_Impuls(_vector _vForce);
	void Add_Force(_vector _vForce);
	void Rotate_To(_vector _vDirection);
	void Move_To(_fvector _vPosition);
protected:
	STAT m_tStat[COORDINATE::COORDINATE_LAST];
	_vector m_vLookBefore = { 0,0,-1 };
public:	
	virtual void Free() override;
};

END