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

public:
	_uint					Get_BulbCount() { return m_iBulbCount; }

public:
	HRESULT					Spawn_Item(_uint _iPrototypeLevelID, _uint _iLevelID, _wstring _szItemTag, _float3 _vPos);
	HRESULT					Spawn_Bulb(_uint _iPrototypeLevelID, _uint _iLevelID, _wstring _szItemTag);
	
	void					Get_Item(_wstring _szItemTag);

private: 
	CGameInstance*						m_pGameInstance = { nullptr };
	ID3D11Device*						m_pDevice = { nullptr };
	ID3D11DeviceContext*				m_pContext = { nullptr };

private:
	map<_wstring, pair<_bool, class CPlayerItem*>>	m_ItemState;
	class CPlayerItem*						m_pCurItem = { nullptr };

	_uint								m_iBulbCount = {};

	// 이름 저장용
	pair<_wstring, _wstring>			m_ItemTags[ITEM_END] = {}; // first -> 구분용 이름, second -> model용 이름

private:
	void					Set_Tags();

public:
	virtual void			Free() override;
};
END
