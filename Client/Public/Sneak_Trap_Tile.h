#pragma once
#include "Sneak_Tile.h"
BEGIN(Client)
class CSneak_Trap_Tile : public CSneak_Tile
{
private:
	CSneak_Trap_Tile(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CSneak_Trap_Tile(const CSneak_Trap_Tile& _Prototype);
	virtual ~CSneak_Trap_Tile() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;

	virtual void			Restart();	
	virtual void			On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx);

private:
	virtual void			Active_Detection();


public:
	static CSneak_Trap_Tile* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;
};

END