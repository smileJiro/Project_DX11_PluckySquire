#pragma once
#include "GameObject.h"
#include "Interactable.h"

BEGIN(Client)
class CPlayer;
class CJumpStarter :
	public CGameObject, public IInteractable
{
public:
	typedef struct tagJumpStarterDesc : public CGameObject::GAMEOBJECT_DESC
	{
		_float3 vBoxHalfExtents = { 0.f,0.f,0.f };
		_float3 vBoxOffset = { 0.f,0.f,0.f };
		_wstring strInitSectionTag;
		
		F_DIRECTION	eJumpMoveDir = F_DIRECTION::DOWN;
		_float2		fTargetPos = { 0.f, 150.f };
		_float		fMoveMag = 1.f;
	}JUMP_STARTER_DESC;
protected:
	explicit CJumpStarter(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CJumpStarter(const CJumpStarter& _Prototype);
	virtual ~CJumpStarter() = default;

	HRESULT				Initialize(void* _pArg);
	virtual void		Update(_float _fTimeDelta) override;
	virtual void		Late_Update(_float _fTimeDelta) override;
	virtual HRESULT		Render() override;

	virtual void		On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject);
	virtual void		On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject);
	virtual void		On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject);

public:
	void				Interact(CPlayer* _pUser) override;
	_bool				Is_Interactable(CPlayer* _pUser) override;
	_float				Get_Distance(COORDINATE _eCoord, CPlayer* _pUser) override;
	virtual void		On_InteractionStart(CPlayer* _pPlayer) override;
	virtual void		On_InteractionEnd(CPlayer* _pPlayer) override;
	//void				Move(_fvector _vForce, _float _fTimeDelta);

private:
	_bool				m_isBodyContact = false;
	_bool				m_isJump = false;
	F_DIRECTION			m_eJumpMoveDir = F_DIRECTION::DOWN;
	_float2				m_fTargetPos = { 0.f,100.f };
	_float 				m_fMoveMag = 1.f;
public:
	static CJumpStarter* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;

	// CGameObject을(를) 통해 상속됨
	HRESULT Cleanup_DeadReferences() override;
};

END