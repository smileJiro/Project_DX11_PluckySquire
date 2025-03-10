#pragma once
#include "ModelObject.h"
BEGIN(Client)
class CMug_Alien final : public CModelObject
{
public:
	enum STATE { STATE_IDLE, STATE_TALK, STATE_LAST };
	typedef struct tagMug_AlienDesc : CModelObject::MODELOBJECT_DESC
	{

	}MUG_ALIEN_DESC;
private:
	CMug_Alien(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CMug_Alien(const CMug_Alien& _Prototype);
	virtual ~CMug_Alien() = default;
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
	void									Set_DialogueIndex(_int _iIndex) { m_iDialogueIndex = _iIndex; }
	void									Plus_DialogueIndex() { ++m_iDialogueIndex; }

private:
	STATE									m_ePreState = STATE::STATE_LAST;
	STATE									m_eCurState = STATE::STATE_LAST;

private:
	_int									m_iDialogueIndex = 0;

private:
	void									State_Change();
	void									State_Change_Idle();
	void									State_Change_Talk();

private:
	void									Action_State(_float _fTimeDelta);
	void									Action_State_Idle(_float _fTimeDelta);
	void									Action_State_Talk(_float _fTimeDelta);

private:
	void									Finished_DialogueAction();

private:
	HRESULT									Ready_Components(MUG_ALIEN_DESC* _pDesc);

public:
	static CMug_Alien*						Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CGameObject*							Clone(void* _pArg) override;
	void									Free() override;
};
END