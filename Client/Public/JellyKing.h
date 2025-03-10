#pragma once
#include "ModelObject.h"
BEGIN(Client)
class CCandleGame;
class CJellyKing final : public CModelObject
{
public:
	enum STATE { STATE_CHEER_OLD, STATE_CHEER, STATE_CRY, STATE_GETATTENTION_1, STATE_GETATTENTION_2, STATE_IDLE, STATE_TALK, STATE_PANIC, STATE_LAST };
	typedef struct tagJellyKingDesc : CModelObject::MODELOBJECT_DESC
	{

	}JELLYKING_DESC;
private:
	CJellyKing(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CJellyKing(const CJellyKing& _Prototype);
	virtual ~CJellyKing() = default;
public:
	virtual HRESULT							Initialize_Prototype(); // 프로토 타입 전용 Initialize
	virtual HRESULT							Initialize(void* _pArg); // 초기화 시 필요한 매개변수를 void* 타입으로 넘겨준다.
	virtual void							Priority_Update(_float _fTimeDelta) override;
	virtual void							Update(_float _fTimeDelta) override;
	virtual void							Late_Update(_float _fTimeDelta) override;
	virtual HRESULT							Render() override;

public:
	void									On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject) override;

public:
	void									OnTrigger_Enter(const COLL_INFO& _My, const COLL_INFO& _Other) override;

public:
	// Get
	STATE									Get_CurState() const { return m_eCurState; }
	_int									Get_DialogueIndex() const { return m_iDialogueIndex; }
	// Set
	void									Set_DialogueIndex(_int _iIndex) { m_iDialogueIndex = _iIndex; }
	void									Plus_DialogueIndex() { ++m_iDialogueIndex; }

private:
	STATE									m_ePreState = STATE::STATE_LAST;
	STATE									m_eCurState = STATE::STATE_LAST;

private:
	_int									m_iDialogueIndex = 0;

private:
	_bool									m_isCandleGame = false;
	CCandleGame*							m_pCandleGame = nullptr;
private:
	void									State_Change();
	void									State_Change_CheerOld(); // X 
	void									State_Change_Cheer();
	void									State_Change_Cry();
	void									State_Change_GetAttention1();  // None Loop
	void									State_Change_GetAttention2();  // None Loop
	void									State_Change_Idle();
	void									State_Change_Talk();
	void									State_Change_Panic(); // X

private:
	void									Action_State(_float _fTimeDelta);
	void									Action_State_CheerOld(_float _fTimeDelta); // X 
	void									Action_State_Cheer(_float _fTimeDelta);
	void									Action_State_Cry(_float _fTimeDelta);
	void									Action_State_GetAttention1(_float _fTimeDelta);	// None Loop 
	void									Action_State_GetAttention2(_float _fTimeDelta);	// None Loop 
	void									Action_State_Idle(_float _fTimeDelta);
	void									Action_State_Talk(_float _fTimeDelta);
	void									Action_State_Panic(_float _fTimeDelta); // X

private:
	/* Active 변경시 호출되는 함수 추가. */

	virtual void							Active_OnEnable();
	virtual void							Active_OnDisable();

private:
	void									Finished_DialogueAction();

private:
	HRESULT									Ready_Components(JELLYKING_DESC* _pDesc);

private:
	void									On_AnimEnd(COORDINATE _eCoord, _uint _iAnimIdx);

public:
	static CJellyKing*						Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CGameObject*							Clone(void* _pArg) override;
	void									Free() override;
};
END