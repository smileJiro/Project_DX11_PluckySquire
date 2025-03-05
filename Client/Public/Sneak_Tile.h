#pragma once
#include "ModelObject.h"

BEGIN(Engine)
class C2DModel;
END

class CSneak_Tile : public CModelObject
{
public:
	//enum DESTINATION {};
	enum SNEAK_TILE_TYPE { DEFAULT, FALL };
	enum TILE_DIRECTION {LEFT, RIGHT, UP, DOWN, LAST };
	typedef struct tagSneakTileDesc : public CModelObject::MODELOBJECT_DESC
	{
		_int	_iTileIndex;
	}SNEAK_TILEDESC;

protected:
	CSneak_Tile(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CSneak_Tile(const CSneak_Tile& _Prototype);
	virtual ~CSneak_Tile() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;


public:
	void		 Set_AdjacentTiles(TILE_DIRECTION _eDir, CSneak_Tile* _pTile) { if (_eDir >= LAST) return; m_AdjacentTiles[_eDir] = _pTile; }
	CSneak_Tile* Get_AdjacentTiles(TILE_DIRECTION _eDir) { return m_AdjacentTiles[_eDir]; }


protected:
	C2DModel*				m_pYellowTile = { nullptr };
	C2DModel*				m_pRedTile = { nullptr };
	CSneak_Tile*			m_AdjacentTiles[LAST] = { nullptr, nullptr, nullptr, nullptr };

protected:
	_int					m_iTileIndex = { 0 };
	SNEAK_TILE_TYPE			m_eTileType = { DEFAULT };

public:
	virtual void Free() override;
};

