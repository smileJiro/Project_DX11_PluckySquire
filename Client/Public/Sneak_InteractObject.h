#pragma once
#include "Sneak_FlipObject.h"

BEGIN(Engine)
END

BEGIN(Client)




class CSneak_Tile;

class CSneak_InteractObject : public CSneak_FlipObject
{
public:
	enum INTERACTOBJECT_TYPE {PRESSURE_PLATE, SWITCH, H_DOOR, V_DOOR, R_DOOR};
public:
	typedef struct tagSneakInteractObjectDesc : public CSneak_FlipObject::FLIPOBJECT_DESC
	{
		_int _iTileIndex = { -1 };
		_bool _isBlocked = { false };
		_bool _isBlockChangable = { false };
		_bool _isPlayerInteractable = { false };
		_bool _isCollisionInteractable = { false };
	}SNEAK_INTERACTOBJECT_DESC;

private:
	CSneak_InteractObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CSneak_InteractObject(const CSneak_InteractObject& _Prototype);
	virtual ~CSneak_InteractObject() = default;


public:
	virtual HRESULT Initialize(void* _pArg) override;

public:
	void	Register_Tiles(class CSneak_Tile* _pTile);
	void	Register_Objects(class CSneak_InteractObject* _pObject);
	void	Interact();

	_bool	Is_CollisionInteractable() const { return m_isCollisionInteractable; }
	_int	Get_TileIndex() const { return m_iTileIndex; }
private:
	_bool	m_isFlipped = { false };
	_bool	m_isBlocked = { false };
	_bool	m_isBlockChangable = { false };
	_bool	m_isPlayerInteractable = { false };
	_bool	m_isCollisionInteractable = { false };

	_int	m_iTileIndex = { -1 };
	
	vector<class CSneak_InteractObject*> m_pInteractObjects;
	vector<class CSneak_Tile*>			 m_pTiles;

public:
	static CSneak_InteractObject* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;

};

NLOHMANN_JSON_SERIALIZE_ENUM(CSneak_InteractObject::INTERACTOBJECT_TYPE, {
{CSneak_InteractObject::INTERACTOBJECT_TYPE::PRESSURE_PLATE, "PRESSURE_PLATE"},
{CSneak_InteractObject::INTERACTOBJECT_TYPE::SWITCH, "SWITCH"},
{CSneak_InteractObject::INTERACTOBJECT_TYPE::H_DOOR, "H_DOOR"},
{CSneak_InteractObject::INTERACTOBJECT_TYPE::V_DOOR, "V_DOOR"},
{CSneak_InteractObject::INTERACTOBJECT_TYPE::R_DOOR, "R_DOOR"},


	});
END


