#pragma once

#include "GameEventExecuter.h"

/*
* 
* 
*	흐름
	
	0. Client\Bin\DataFiles\Trigger_Events.json
		새 json 객체 작성.
		"Actions"의 ActionTag를 "Create_EventExecuter" 로 맞춰야 한다.
		"EventTag" 에 실행할 태그를 이름지어서 순서대로 작성.

		ex. C02P0708_MonsterSpawnProcess 참조
		(   1. 번개가 내리친다
			2. 몬스터가 스폰된다)


	1. CTrigger_Manager.h :: enum EVENT_EXECUTER_ACTION_TYPE
		EventTag 종류별로 이름을 이름지어 추가하라.
	
	2. CTrigger_Manager.cpp ::Mapping_ExecuterTag()
		EVENT_EXECUTER_ACTION_TYPE와 EventTag를 매핑시켜라.
	
	3. switch_case에 해당하는 위치의 EVENT_EXECUTER_ACTION_TYPE에 하드코딩.
		자유롭게 써도 됨.


	해당 게임이벤트 모음을 트리거로 설정하고 싶다면,
	3D -> 김효림
	2D -> 박예슬
	문의

	250218_박예슬

*/



BEGIN(Client)

class CPlayer;
class CBook;

class CGameEventExecuter_C8 : public CGameEventExecuter
{
private:
	CGameEventExecuter_C8(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CGameEventExecuter_C8() = default;

public:
	virtual HRESULT					Initialize(void* _pArg) override;
	virtual void					Priority_Update(_float _fTimeDelta) override;
	virtual void					Update(_float _fTimeDelta) override;
	virtual void					Late_Update(_float _fTimeDelta) override;

	void Chapter8_Laser_Stage(_float _fTimeDelta);
	void Chapter8_Intro(_float _fTimeDelta);
	void Chapter8_Map_Intro(_float _fTimeDelta);
	void Chapter8_Intro_Postit_Sequence(_float _fTimeDelta);
	void Chapter8_Outro_Postit_Sequence(_float _fTimeDelta);
	void Chapter8_Sword(_float _fTimeDelta);
	void Chapter8_Stop_Stamp(_float _fTimeDelta);
	void Chapter8_Bomb_Stamp(_float _fTimeDelta);
	void Chapter8_Tilting_Glove(_float _fTimeDelta);
	virtual void							Change_PlayMap() override;


private:
	_bool				m_isPlayerFirstDead = false;
public:
	static CGameEventExecuter_C8* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void Free();

	// CGameObject을(를) 통해 상속됨
	virtual CGameObject* Clone(void* _pArg) override;
	virtual HRESULT Cleanup_DeadReferences() override;
};
END