#pragma once
#include "GameObject.h"
/* 1. 간편하게 생성삭제 및 업데이트를 돌리기 위해 GameObject를 상속받는 하나의 객체로 생성한다. */
/* 2. 캔들 게임 객체는 게임 시작 시, Candle 객체들을 생성하고 레이어에 담는다, 그 후 본인이 Vector 로 Candle* 를 관리한다. */
/* 3. 캔들벡터를 순회하며, FlameLoop 상태인 Candle을 체크한다. */
/* 4. 모든 캔들이 FlameLoop 상태가 되면 isClear 가 활성화 되고, 미리 지정해둔 이벤트 함수가 호출된다. */
/* 5. 캔들게임의 삭제는 외부에서 수행한다 >>> 아저씨가 하던지 이벤트 처리기로 하던지 등등 */
/* 6. 캔들게임이 종료되더라도 Candle 객체는 레이어에서 삭제시키지 않는다. 다만 UI는 Active False 처리한다. */

BEGIN(Client)
class CCandle;
class CCandleGame final : public CGameObject
{
public:
	typedef struct tagCandleGameDesc : CGameObject::GAMEOBJECT_DESC
	{
		vector<_float3> CandlePositions; // 3D캔들의 위치를 설정.
	}CANDLEGAME_DESC;

private:
	CCandleGame(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CCandleGame(const CCandleGame& _Prototype);
	virtual ~CCandleGame() = default;

public:
	virtual HRESULT				Initialize_Prototype();				
	virtual HRESULT				Initialize(void* _pArg);			
	virtual void				Priority_Update(_float _fTimeDelta);
public:
	void						OnOffCandleUIs(_bool _isOnOff);
public:
	// Get
	_bool						Is_ClearGame() const { return m_isClearGame; }

private:
	void						Check_Clear(_float _fTimeDelta);	// 게임 클리어를 검사하는 함수.
	void						ClearGame();	// 게임 클리어 시 1회 호출되는 함수.

private:
	vector<CCandle*>			m_Candles;

	_float2						m_vClearCheckCycleTime = { 1.0f/ 20.f, 0.0f }; // 매프레임 정밀하게 돌 필요없으니.
	_uint						m_iFlameCandleCount = 0;
	_bool						m_isClearGame = false;

private:
	HRESULT						Ready_Candles(CANDLEGAME_DESC* _pDesc);

	
public:
	static	CCandleGame*		Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CCandleGame*				Clone(void* _pArg);
	void						Free() override;

	// CGameObject을(를) 통해 상속됨
	HRESULT Cleanup_DeadReferences() override;
};

END