#pragma once
#include "Sneak_Tile.h"
BEGIN(Client)
class CSneak_Fall_Tile : public CSneak_Tile
{
private:
	CSneak_Fall_Tile(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CSneak_Fall_Tile(const CSneak_Fall_Tile& _Prototype);
	virtual ~CSneak_Fall_Tile() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;

private:
	HRESULT Ready_Components();

public:
	static CSneak_Fall_Tile* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;
};

END