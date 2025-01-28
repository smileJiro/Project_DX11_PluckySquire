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
	STAT& Get_Stat(COORDINATE _eCoord) { return m_tStat[_eCoord]; }

protected:
	STAT m_tStat[COORDINATE::COORDINATE_LAST];
public:	
	virtual void Free() override;
};

END