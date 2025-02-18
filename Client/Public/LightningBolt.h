#pragma once
#include "Monster.h"
#include "Client_Defines.h"

BEGIN(Client)
class CLightningBolt final : public CContainerObject, public IAnimEventReceiver
{
public:
	typedef struct tagLightningBolt_Desc : public CContainerObject::CONTAINEROBJ_DESC
	{
		_float fLifeTime;
	}LIGHTNINGBOLT_DESC;

	enum Animation2D
	{
		CRAWLER_01_END_LIGHTNING_CRAWLER01_END,
		CRAWLER_01_LOOP_LIGHTNING_CRAWLER01_LOOP,
		CRAWLER_02_END_LIGHTNING_CRAWLER02_END,
		CRAWLER_02_LOOP_LIGHTNING_CRAWLER02DOWN,
		LIGHTNING_BOLT,
		LIGHTNING_DISC,
		ANIM2D_LAST,
	};

protected:
	CLightningBolt(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CLightningBolt(const CLightningBolt& _Prototype);
	virtual ~CLightningBolt() = default;

public:
	virtual HRESULT Initialize_Prototype(); // 프로토 타입 전용 Initialize
	virtual HRESULT Initialize(void* _pArg); // 초기화 시 필요한 매개변수를 void* 타입으로 넘겨준다.
	virtual void			Priority_Update(_float _fTimeDelta) override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;

public:
	virtual HRESULT		Change_Coordinate(COORDINATE _eCoordinate, _float3* _pNewPosition = nullptr) override;
	void Change_Animation();
	void Animation_End(COORDINATE _eCoord, _uint iAnimIdx);
	void Lightning();

public:
	virtual void			On_Hit(CGameObject* _pHitter, _float _fDamg);

	virtual void OnTrigger_Enter(const COLL_INFO& _My, const COLL_INFO& _Other)override;
	virtual void OnTrigger_Stay(const COLL_INFO& _My, const COLL_INFO& _Other)override;
	virtual void OnTrigger_Exit(const COLL_INFO& _My, const COLL_INFO& _Other)override;

	virtual void	On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;
	virtual void	On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;
	virtual void	On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;


	virtual void Active_OnEnable() override;
	virtual void Active_OnDisable() override;

private:
	_float m_fLifeTime = { 0.f };
	_float m_fAccTime = { 0.f };

	_bool m_isStop = {};

	CAnimEventGenerator* m_pAnimEventGenerator = { nullptr };

private:
	virtual HRESULT					Ready_Components();
	virtual HRESULT					Ready_PartObjects();

public:
	static CLightningBolt* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;
};
END