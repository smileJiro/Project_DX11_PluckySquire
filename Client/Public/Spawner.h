#pragma once
#include "Controller_Transform.h"
/* 1. 객체 생성 Desc */
/* 2. 생성 위치 >>> 이 정보는 사실 Desc에 들어있다. */
/* 3. 생성 주기 */
/* 4. 풀링 사용 여부 */
/* 5. */

/* 1. 업데이트를 돌리며 시간체크 */
/* 2. 객체 생성 */
BEGIN(Engine)
class CGameObject;
class CGameInstance;
END

BEGIN(Client)
class CSpawner : public CBase
{
public:
	typedef struct tagSpawnerDesc
	{
		CController_Transform::CON_TRANSFORM_DESC* pObjectCloneDesc; /* 동적할당한 데이터로 desc을 채워야한다. */
		_float fSpawnCycleTime;
		_uint iOneClycleSpawnCount; /* 1번 스폰시 생성되는 몬스터의 마리 수 >> 만약 여러마리 생성이라면 내부적으로 좌표 랜덤 돌려서 인근지역에 생성하겠음. */
		_bool isPooling = false;


	}SPAWNER_DESC;
private:
	CSpawner(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CSpawner() = default;

public:
	HRESULT Initialize(SPAWNER_DESC* _pDesc);
	HRESULT Update(_float _fTimeDelta);

	
private:
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;
	CGameInstance* m_pGameInstance = nullptr;

private:
	CController_Transform::CON_TRANSFORM_DESC* m_pObjecClonetDesc = nullptr;
	_float2 m_vSpawnCycleTime = {};
	_uint m_iOneClycleSpawnCount = 0;
	_bool m_isPooling = false;

private:
	HRESULT Spawn_Object();

private:
	virtual void Active_OnEnable(); // 활성 비활성 여부에 따라 Update 돌리는 유무가 달라짐.
	virtual void Active_OnDisable();

public:
	static CSpawner* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, SPAWNER_DESC* _pDesc);
	void Free() override;
};

END