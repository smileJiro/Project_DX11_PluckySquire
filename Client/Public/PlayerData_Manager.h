#pragma once
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
class CGameObject;
END

BEGIN(Client)
class CPlayerData_Manager final : public CBase
{
	DECLARE_SINGLETON(CPlayerData_Manager)

public:
	enum PLAYERDATA_TYPE
	{
		PLAYER_ITEM,
		BULB,

		PLAYERDATA_TYPE_END
	};

	enum PLAYERITEM_TYPE 
	{
		FLIPPING_GLOVE,
		TILTING_GLOVE,
		STOP_STAMP,
		BOMB_STAMP,

		ITEM_END
	};

private:
	CPlayerData_Manager();
	virtual ~CPlayerData_Manager() = default;

public:
	HRESULT					Initialize(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);

	HRESULT					Level_Enter(_int _iChangeLevelID);
	HRESULT					Level_Exit(_int _iChangeLevelID, _int _iNextChangeLevelID);

public:
	_uint					Get_BulbCount() { return m_iBulbCount; }
	_bool					Is_Own(PLAYERITEM_TYPE _eItemType);

public:
	HRESULT					Spawn_PlayerItem(_uint _iPrototypeLevelID, _uint _iLevelID, _wstring _szItemTag, _float3 _vPos);
	HRESULT					Spawn_Bulb(_uint _iPrototypeLevelID, _uint _iLevelID);
	
	void					Get_PlayerItem(_wstring _szItemTag);
	void					Increase_BulbCount() { m_iBulbCount += 1; };

public:
	void					Change_PlayerItemMode(_uint _iPlayerItemType, _uint _iItemMode);
	void					Change_PlayerItemMode(_wstring _strItemTag, _uint _iItemMode);

public:
	void					Register_Player(CPlayer* _pPlayer);
	CPlayer*				Get_Player_Ptr() { return m_pPlayer; }

private: 
	CGameInstance*						m_pGameInstance = { nullptr };
	ID3D11Device*						m_pDevice = { nullptr };
	ID3D11DeviceContext*				m_pContext = { nullptr };

private:
	class CPlayer*						m_pPlayer = { nullptr };
	map<_wstring, pair<_bool, class CPlayerItem*>>	m_ItemState;
	class CPlayerItem*					m_pCurItem = { nullptr };

	_uint								m_iBulbCount = {};

	// 이름 저장용
	pair<_wstring, _wstring>			m_ItemTags[ITEM_END] = {}; // first -> 구분용 이름, second -> model용 이름

private:
	void					Set_Tags();

public:
	virtual void			Free() override;
};
END
