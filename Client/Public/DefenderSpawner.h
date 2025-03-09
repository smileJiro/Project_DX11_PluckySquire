#pragma once
#include "Base.h"
#include "DefenderMonster.h"
#include "Pooling_Manager.h"
BEGIN(Client)
enum SPAWN_PATTERN
{
	SPAWN_PATTERN_DOT,
	SPAWN_PATTERN_ARROW,
	SPAWN_PATTERN_VERTICAL,
	SPAWN_PATTERN_RANDOM,
	SPAWN_PATTERN_LAST,
};
class CSection_Manager;
class CDefenderPlayer;
struct SPAWN_DESC
{
	friend class CDefenderSpawner;
	SPAWN_PATTERN ePattern = SPAWN_PATTERN_DOT;
	T_DIRECTION eDirection = T_DIRECTION::RIGHT;
	//À¯´Ö »ý»ê È½¼ö
	_uint iSpawnCount = 1; 
	//ÇÑ ¹øÀÇ À¯´Ö »ý»ê µô·¹ÀÌ.
	_float fUnitDelay = 0.f;

	_bool bAbsolutePosition = false;
	_vector vPosition = { 0.f,0.f,0.f };

	_float fPlayerDistance = 0.f;
	_float fHeight = 0.f;


	//Auto Àü¿ë
	_float fAutoCycleTime = -1.f;

public: 
	void Update(_float _fTimeDelta);
	void Reset_Cycle() { iCurrentSpawnCount = 0; fCycleTimeAcc = 0.f; fUnitTimeAcc = 0.f; }
	void Reset_Unit() { fUnitTimeAcc = 0.f; }
	_bool Is_UnitSpawnReady() { return fUnitTimeAcc >= fUnitDelay; }
	_bool Is_PatternEnd() { return iCurrentSpawnCount >= iSpawnCount; }

	_bool Is_Auto() { return fAutoCycleTime >= 0.f; }
	_bool Is_CycleEnd() { return fCycleTimeAcc >= fAutoCycleTime; }
private:
	_uint iCurrentSpawnCount = 0;
	_float fCycleTimeAcc = 0.f;
	_float fUnitTimeAcc = 0.f;
};
class CDefenderSpawner :
    public CGameObject
{
public:

	typedef struct tagDefenderSpawnerDesc : CGameObject::GAMEOBJECT_DESC
	{
		_wstring strPoolTag = TEXT("");
		_wstring strSectionName = TEXT("");
		CDefenderPlayer* pPlayer = nullptr;
	}DEFENDER_SPAWNER_DESC;
private:
	CDefenderSpawner(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CDefenderSpawner(const CDefenderSpawner& _Prototype);
	virtual ~CDefenderSpawner() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* _pArg);
	virtual void	Update(_float _fTimeDelta);

public:
	void Add_Spawn(SPAWN_DESC tDesc);

private: 
	void Spawn_Single(T_DIRECTION _eDirection, _vector _vPos = { 0.f,0.f,0.f });
	void Spawn(SPAWN_DESC& _tDesc);
	void Spawn_Dot(SPAWN_DESC& _tDesc);
	void Spawn_Arrow(SPAWN_DESC& _tDesc);
	void Spawn_Vertical(SPAWN_DESC& _tDesc);
	void Spawn_Random(SPAWN_DESC& _tDesc);
private:
	CSection_Manager* m_pSection_Manager = nullptr;
	class CPooling_Manager* m_pPoolMgr = nullptr;
	_wstring m_strPoolTag = TEXT("");
	CDefenderPlayer* m_pPlayer = nullptr;

	_uint m_iDotSequenceCount = 6;

	//Que
	list<SPAWN_DESC> m_SpawnList = {};
public:
	static CDefenderSpawner* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CGameObject* Clone(void* _pArg) override;
	HRESULT Cleanup_DeadReferences() override;
	virtual void Free() override;
};


END