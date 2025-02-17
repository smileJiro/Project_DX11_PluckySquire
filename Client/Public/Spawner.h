#pragma once
#include"GameObject.h"
/* 1. 객체 생성 Desc */
/* 2. 생성 위치 >>> 이 정보는 사실 Desc에 들어있다. */
/* 3. 생성 주기 */
/* 4. 풀링 사용 여부 */
/* 5. */

/* 1. 업데이트를 돌리며 시간체크 */
/* 2. 객체 생성 */
BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)
class CSpawner : public CGameObject
{
public:
	typedef struct tagSpawnerDesc : CController_Transform::CON_TRANSFORM_DESC
	{
		/* Add To Layer Data */
		LEVEL_ID eCurLevelID;
		LEVEL_ID eGameObjectPrototypeLevelID;
		_wstring strLayerTag;
		CController_Transform::CON_TRANSFORM_DESC* pObjectCloneDesc; /* 동적할당한 데이터로 desc을 채워야한다. */

		/* Spawner Data */
		_float fSpawnCycleTime;
		_uint iOneClycleSpawnCount; /* 1번 스폰시 생성되는 몬스터의 마리 수 >> 만약 여러마리 생성이라면 내부적으로 좌표 랜덤 돌려서 인근지역에 생성하겠음. */
		_float3 vSpawnPosition = {};

		/* Pooling Manager Data */
		_bool isPooling = false;
		_wstring strPoolingTag;
		Pooling_DESC tPoolingDesc;
		COORDINATE ePoolingObjectStartCoord = COORDINATE_LAST;
	}SPAWNER_DESC;

private:
	CSpawner(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CSpawner(const CSpawner& _Prototype);
	virtual ~CSpawner() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* _pArg);
	virtual void	Update(_float _fTimeDelta);


private: /* Add To Layer Data */
	LEVEL_ID m_eCurLevelID = LEVEL_ID::LEVEL_END;
	LEVEL_ID m_eGameObjectPrototypeLevelID = LEVEL_ID::LEVEL_END;
	_wstring m_strLayerTag;
	CController_Transform::CON_TRANSFORM_DESC* m_pObjectCloneDesc = nullptr;

private: /* Spawner Data */
	_float3 m_vSpawnPostion = {};
	_float2 m_vSpawnCycleTime = {};
	_uint m_iOneClycleSpawnCount = 0;

private: /* Pooling Data */
	_bool m_isPooling = true;
	COORDINATE m_ePoolingObjectStartCoord = COORDINATE_LAST;
	_wstring m_strPoolingTag;

private:
	_bool m_isDeleteObjectDesc = false;
private:
	HRESULT Spawn_Object();

private:
	virtual void Active_OnEnable(); // 활성 비활성 여부에 따라 Update 돌리는 유무가 달라짐.
	virtual void Active_OnDisable();

public:
	static CSpawner* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CGameObject* Clone(void* _Arg) override;
	void Free() override;
	HRESULT Cleanup_DeadReferences() override;
};

END