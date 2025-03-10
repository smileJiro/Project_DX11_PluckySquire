#pragma once
#include "Sneak_FlipObject.h"

BEGIN(Client)

class CSneak_Tile : public CSneak_FlipObject
{
public:
	enum TILE_ANIMATION 
	{ DEFAULT_CLOSE, DEFAULT_CLOSED, DEFAULT_OPENED, DEFAULT_OPEN,
		TRAP_CLOSE, TRAP_CLOSED, TRAP_OPEN, TRAP_OPENDED, DEFAULT_RED, DEFAULT_YELLOW, TRAP_YELLOW };
	enum SNEAK_TILE_TYPE { DEFAULT, TRAP };
	enum TILE_STATE { CLOSE, OPEN, YELLOW, RED };
	typedef struct tagSneakTileDesc : public CSneak_FlipObject::FLIPOBJECT_DESC
	{
		_int	iTileIndex;
		_int	iAdjacents[(_uint)(F_DIRECTION::F_DIR_LAST)] = { -1, -1, -1, -1 };
	}SNEAK_TILEDESC;

protected:
	CSneak_Tile(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CSneak_Tile(const CSneak_Tile& _Prototype);
	virtual ~CSneak_Tile() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;
	
public:
	virtual void			Restart() = 0;
	void					Interact();
	
	void					Add_Detector(_bool _isRed) { if (_isRed) m_isRedDetect = true; else m_isYellowDetect = true; Active_Detection(); }
	void					Delete_Dector(_bool _isRed) { if (_isRed) m_isRedDetect = false; else m_isYellowDetect = false; Active_Detection(); }



public:
	void		 Set_AdjacentTiles(F_DIRECTION _eDir, _int _iTileIndex) 
	{ 
		if (_eDir >= F_DIRECTION::F_DIR_LAST) 
			return; 
		_int iDir = (_int)(_eDir);
		m_AdjacentTiles[iDir] = _iTileIndex;
	}
	_int		 Get_AdjacentTiles(F_DIRECTION _eDir) const { return m_AdjacentTiles[(_uint)(_eDir)]; }
	_int		 Get_TileIndex() const { return m_iTileIndex; }
	_float2		 Get_TilePosition() const { return m_vTilePosition; }
	TILE_STATE	 Get_TileState() const { return m_eCurState; }


protected:
	SNEAK_TILE_TYPE			m_eTileType = { DEFAULT };
	TILE_STATE				m_eCurState = { CLOSE };
	//_int					m_iYellowDetectorCount = { 0 };
	//_int					m_iRedDetectorCount = { 0 };
	_bool					m_isYellowDetect = { false };
	_bool					m_isRedDetect = { false };
	_int					m_iTileIndex = { 0 };
	_int					m_AdjacentTiles[(_uint)(F_DIRECTION::F_DIR_LAST)] = { -1, -1, -1, -1 };
	_float2					m_vTilePosition = { 0.f, 0.f };

	virtual void			Active_Detection() = 0;

public:
	virtual void Free() override;
};

END