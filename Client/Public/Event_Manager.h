#pragma once
#include "Base.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CGameInstance;
class CGameObject;
END

BEGIN(Client)
class CUI_Fade;
typedef struct tagEvent_Desc
{
	EVENT_TYPE eType = EVENT_TYPE::LAST;
	vector<DWORD_PTR> Parameters;
}EVENT;

class CEvent_Manager final : public CBase
{
	DECLARE_SINGLETON(CEvent_Manager)
private:
	CEvent_Manager();
	virtual ~CEvent_Manager() = default;

public:
	HRESULT Initialize(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	void Update(_float _fTimeDelta);
	void AddEvent(const EVENT& _tEvent) { m_Events.push_back(_tEvent); }

private:
	HRESULT Execute(const EVENT& _tEvent);

	HRESULT Execute_CreateObject(const EVENT& _tEvent);
	HRESULT Execute_DeleteObject(const EVENT& _tEvent);
	HRESULT Execute_LevelChange(const EVENT& _tEvent);
	HRESULT Execute_SetActive(const EVENT& _tEvent);
	HRESULT Execute_Setup_SimulationFilter(const EVENT& _tEvent);
	HRESULT Execute_Change_Coordinate(const EVENT& _tEvent);
	HRESULT Execute_Set_Kinematic(const EVENT& _tEvent);

	HRESULT Execute_ChangeMonsterState(const EVENT& _tEvent);
	HRESULT Execute_ChangeBossState(const EVENT& _tEvent);

	HRESULT	Execute_CameraTrigger(const EVENT& _tEvent);

private:
	HRESULT Client_Level_Enter(_int _iChangeLevelID);
	HRESULT Client_Level_Exit(_int _iChangeLevelID, _int _iNextChangeLevelID);

private:
	CGameInstance* m_pGameInstance = nullptr;
	vector<CGameObject*> m_DeadObjectsList;
	vector<pair<CBase*, _bool>> m_DelayActiveList;
	vector<EVENT> m_Events;

	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;

public:
	virtual void Free() override;
};

END

/* TODO :: 레벨 체인지 수정 완료 10.31 Tool도 수정해야함 */