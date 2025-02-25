#pragma once
#include "CarriableObject.h"

BEGIN(Client)
class CJumpPad :
    public CCarriableObject
{
public:
	enum ANIMIDX_2D
	{
		PLATFORMER_BOUNCE_2D,
		PLATFORMER_CARRY_2D,
		TOP_CARRY_2D,
		TOP_IDLE_2D
	};
	enum ANIMIDX_3D
	{
		LARGE_JUMP_3D,
		SMALL_JUMP_3D,
		WOOBLE_3D,
	};
	typedef struct tagJumpPadDesc : public CCarriableObject::CARRIABLE_DESC
	{
		_bool bPlarformerMode = false;
	}JUMPPAD_DESC;
private:
	explicit CJumpPad(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CJumpPad(const CJumpPad& _Prototype);
	virtual ~CJumpPad() = default;
public:
	HRESULT Initialize(void* _pArg);
	virtual void		Update(_float _fTimeDelta) override;
	virtual void		Late_Update(_float _fTimeDelta) override;
	virtual HRESULT Render() override;


public:
	virtual HRESULT	Change_Coordinate(COORDINATE _eCoordinate, _float3* _pNewPosition = nullptr) override;

	virtual void OnContact_Enter(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas) override;
	virtual void OnContact_Stay(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas) override;
	virtual void OnContact_Exit(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas) override;
	virtual void OnTrigger_Enter(const COLL_INFO& _My, const COLL_INFO& _Other)override;
	virtual void OnTrigger_Stay(const COLL_INFO& _My, const COLL_INFO& _Other)override;
	virtual void OnTrigger_Exit(const COLL_INFO& _My, const COLL_INFO& _Other)override;
	virtual void On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;
	virtual void On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;
	virtual void On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;

private:
	//0
	_bool m_bPlatformerMode = false;
	_float m_fBouncePower3D = 15.f;
	_float m_fBouncePower2D = 1000.f;
	_float m_fCollisionSlopeThreshold = -0.9f;
public:
	static CJumpPad* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;
};

END