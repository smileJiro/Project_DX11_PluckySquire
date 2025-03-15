#pragma once
#include "Base.h"
// 1. 친구를 등록한다. 
// 2. 친구의 상태를 조작한다. 
// 3. 친구들끼리 뭉쳐다니기 기능에 필요한 데이터를 저장하고 제공한다.

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)
class CFriend;
class CFriend_Controller final : public CBase
{
	DECLARE_SINGLETON(CFriend_Controller)

private:
	CFriend_Controller();
	virtual ~CFriend_Controller() = default;
public:
	HRESULT							Initialize(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);

public: /* Default */
	HRESULT							Register_Friend(const _wstring& _strFriendTag, CFriend* _pFriend);
	HRESULT							Erase_Friend(const _wstring& _strFriendTag);
	CFriend*						Find_Friend(const _wstring& _strFriendTag);

public: /* Train */
	HRESULT							Start_Train();
	HRESULT							End_Train();
	HRESULT							Register_Friend_ToTrainList(const _wstring& _strFriendTag);
	HRESULT							Erase_Friend_FromTrainList(const _wstring& _strFriendTag);
	_wstring*						Find_Friend_FromTrainList(const _wstring& _strFriendTag);
	_wstring*						Find_Friend_FromTrainList(_uint _iIndex);
	
private:
	ID3D11Device*					m_pDevice = nullptr;
	ID3D11DeviceContext*			m_pContext= nullptr;
	CGameInstance*					m_pGameInstance = nullptr;

private:
	map<_wstring, CFriend*>			m_Friends;
	list<_wstring>					m_TrainList;

public:
	virtual void Free() override;
};

END