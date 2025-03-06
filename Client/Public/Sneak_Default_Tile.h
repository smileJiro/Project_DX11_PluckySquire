#pragma once
#include "Sneak_Tile.h"

BEGIN(Client)
class CSneak_Default_Tile : public CSneak_Tile
{
private:
	CSneak_Default_Tile(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CSneak_Default_Tile(const CSneak_Default_Tile& _Prototype);
	virtual ~CSneak_Default_Tile() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;



public:
	static CSneak_Default_Tile* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;
};
//
END