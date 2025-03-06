#pragma once
#include "ModelObject.h"

BEGIN(Client)

class CSneak_Tile : public CModelObject
{
public:
	enum TILE_ANIMATION 
	{ DEFAULT_CLOSE, DEFAULT_CLOSED, DEFAULT_OPENED, DEFAULT_OPEN,
		TRAP_CLOSE, TRAP_CLOSED, TRAP_OPEN, TRAP_OPENDED, DEFAULT_RED, DEFAULT_YELLOW, TRAP_YELLOW };
	enum SNEAK_TILE_TYPE { DEFAULT, TRAP };
	enum TILE_DIRECTION { RIGHT, DOWN, LEFT, UP, LAST };
	typedef struct tagSneakTileDesc : public CModelObject::MODELOBJECT_DESC
	{
		_int	iTileIndex;
		_int	iAdjacents[LAST] = { -1, -1, -1, -1 };
	}SNEAK_TILEDESC;

protected:
	CSneak_Tile(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CSneak_Tile(const CSneak_Tile& _Prototype);
	virtual ~CSneak_Tile() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;


public:
	void		 Set_AdjacentTiles(TILE_DIRECTION _eDir, _int _iTileIndex) { if (_eDir >= LAST) return; m_AdjacentTiles[_eDir] = _iTileIndex; }
	_int		 Get_AdjacentTiles(TILE_DIRECTION _eDir) const { return m_AdjacentTiles[_eDir]; }
	_int		 Get_TileIndex() const { return m_iTileIndex; }
	_float2		 Get_TilePosition() const { return m_vTilePosition; }

protected:
	_int			m_AdjacentTiles[LAST] = { -1, -1, -1, -1 };
	_float2			m_vTilePosition = { 0.f, 0.f };
protected:
	_int					m_iTileIndex = { 0 };
	SNEAK_TILE_TYPE			m_eTileType = { DEFAULT };

public:
	virtual void Free() override;
};

END