#pragma once
#include "ModelObject.h"
/* 1. Idle 상태는 일반적인 상태로 이동 상태까지 함께 정의 한다. */
BEGIN(Client)
class CZetPack_Father final : public CModelObject
{
public:
	enum STATE 
	{
		STATE_FIREENGINE, // 아이템 주는 거
		STATE_REASSEMBLE, // 조립
		STATE_HAPPY,
		STATE_IDLE,
		STATE_SAD,
		STATE_TALK,
		STATE_NONRENDER,
		STATE_LAST
	};

public:
	typedef struct tagZetPackFatherDesc : CModelObject::MODELOBJECT_DESC
	{
	}ZETPACK_FATHER_DESC;

private:
	CZetPack_Father(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CZetPack_Father(const CZetPack_Father& _Prototype);
	virtual ~CZetPack_Father() = default;

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

public:
	// Get
	STATE									Get_CurState() const { return m_eCurState; }
	_int									Get_DialogueIndex() const { return m_iDialogueIndex; }
	// Set
	void									Set_CurState(STATE _eCurState) { m_eCurState = _eCurState; }
	void									Set_DialogueIndex(_int _iIndex) { m_iDialogueIndex = _iIndex; }
	void									Plus_DialogueIndex() { ++m_iDialogueIndex; }

private:
	STATE									m_ePreState = STATE::STATE_LAST;
	STATE									m_eCurState = STATE::STATE_LAST;

private:
	_int									m_iDialogueIndex = 0;
	_bool									m_isContactPlayer = false;
private:
	void									State_Change();
	void									State_Change_State_FireEngine();
	void									State_Change_Reassemble();
	void									State_Change_Happy();
	void									State_Change_Idle();
	void									State_Change_Sad();
	void									State_Change_Talk();
	void									State_Change_NoneRender();

private:
	void									Action_State(_float _fTimeDelta);
	void									Action_State_State_FireEngine(_float _fTimeDelta);
	void									Action_State_Reassemble(_float _fTimeDelta);
	void									Action_State_Happy(_float _fTimeDelta);
	void									Action_State_Idle(_float _fTimeDelta);
	void									Action_State_Sad(_float _fTimeDelta);
	void									Action_State_Talk(_float _fTimeDelta);
	void									Action_State_NoneRender(_float _fTimeDelta);

private:
	void									Finished_DialogueAction();

private:
	void									On_AnimEnd(COORDINATE _eCoord, _uint _iAnimIdx);

private:
	HRESULT									Ready_Components(ZETPACK_FATHER_DESC* _pDesc);

public:
	static CZetPack_Father*					Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CZetPack_Father*						Clone(void* _pArg);
	void									Free() override;
};
END
