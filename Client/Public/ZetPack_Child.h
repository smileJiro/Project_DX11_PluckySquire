#pragma once
#include "ModelObject.h"
/* 1. Idle 상태는 일반적인 상태로 이동 상태까지 함께 정의 한다. */
BEGIN(Client)
class CZetPack_Child final : public CModelObject
{
public:
	enum STATE 
	{
		STATE_IDLE,
		STATE_TALK,
		STATE_CHASE,
		STATE_PORTALOUT, // 플레이어의 상태를 추적하여 플레이어가 포탈밖으로 나갔다면, 자기 자신의 렌더를 종료함
		STATE_PORTALIN, // 플레이어의 상태를 추적하여 플레이어가 포탈안으로 들어갔다면, 자기자신을 플레이어와 같은 섹션에 넣음.
		STATE_LAST
	};
	enum ANIMINDEX
	{
		IDLE_DOWN,
		IDLE_RIGHT,
		IDLE_UP,
		TALK_DOWN,
		TALK_RIGHT,
		TALK_UP,

		ANIM_LAST
	};
	enum DIRECTION
	{
		DIR_DOWN,
		DIR_RIGHT,
		DIR_UP,
		DIR_LEFT,

		DIR_LAST
	};
public:
	typedef struct tagZetPackChildDesc : CModelObject::MODELOBJECT_DESC
	{
		class CPlayer* pPlayer = nullptr;
	}ZETPACK_CHILD_DESC;

private:
	CZetPack_Child(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CZetPack_Child(const CZetPack_Child& _Prototype);
	virtual ~CZetPack_Child() = default;

public:
	virtual HRESULT							Initialize_Prototype(); // 프로토 타입 전용 Initialize
	virtual HRESULT							Initialize(void* _pArg); // 초기화 시 필요한 매개변수를 void* 타입으로 넘겨준다.
	virtual void							Priority_Update(_float _fTimeDelta) override;
	virtual void							Update(_float _fTimeDelta) override;
	virtual void							Late_Update(_float _fTimeDelta) override;
	virtual HRESULT							Render() override;

public:
	void									On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject) override;
	void									On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject) override;
	void									On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject) override;
private:
	CPlayer*								m_pPlayer = nullptr;

private:
	STATE									m_ePreState = STATE::STATE_LAST;
	STATE									m_eCurState = STATE::STATE_LAST;
	DIRECTION								m_eDirection = DIRECTION::DIR_DOWN;

private:
	_bool									m_isContactPlayer = false; /* 플레이어와의 첫 만남을 판별 */
	_float									m_fChaseInterval = 60.f;

private:
	_int									m_iDialogueIndex = 0;
	
private:
	void									State_Change();
	void									State_Change_Idle();
	void									State_Change_Talk();
	void									State_Change_Chase();
	void									State_Change_PortalOut();
	void									State_Change_PortalIn();

private:
	void									Action_State(_float _fTimeDelta);
	void									Action_State_Idle(_float _fTimeDelta);
	void									Action_State_Talk(_float _fTimeDelta);
	void									Action_State_Chase(_float _fTimeDelta);
	void									Action_State_PortalOut(_float _fTimeDelta);
	void									Action_State_PortalIn(_float _fTimeDelta);

private:
	void									Update_AnimationDirection();
	void									ChaseToTarget(_float _fTimeDelta);
	void									Finished_DialogueAction();
private:
	HRESULT									Ready_Components(ZETPACK_CHILD_DESC* _pDesc);

public:
	static CZetPack_Child*					Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CZetPack_Child*							Clone(void* _pArg);
	void									Free() override;

};
END
