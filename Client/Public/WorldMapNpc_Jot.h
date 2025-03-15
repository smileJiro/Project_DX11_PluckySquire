#pragma once
#include "ModelObject.h"
#include "WorldMapNPC.h"


BEGIN(Client)
class CWorldMapNpc_Jot final : public CModelObject
{
public:
	typedef struct tagWorldNPC :public CModelObject::MODELOBJECT_DESC
	{
		class CWorldMapNPC* pParent = nullptr;
	}WORLDMAP_PART_JOT_DESC;

public:
	enum ANIMATION
	{
		ANIM_DOWNIDLE = 0,
		ANIM_RIGHTIDLE = 1,
		ANIM_RIGHTWALK = 2,
		ANIM_UPIDLE = 3,
		ANIM_LAST = 4
	};


private:
	CWorldMapNpc_Jot(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CWorldMapNpc_Jot(const CWorldMapNpc_Jot& _Prototype);
	virtual ~CWorldMapNpc_Jot() = default;



public:
	virtual HRESULT				Initialize_Prototype();								// 프로토 타입 전용 Initialize
	virtual HRESULT				Initialize(void* _pArg);							// 초기화 시 필요한 매개변수를 void* 타입으로 넘겨준다.
	virtual void				Priority_Update(_float _fTimeDelta);				// 특정개체에 대한 참조가 빈번한 객체들이나, 등등의 우선 업데이트 되어야하는 녀석들.
	virtual void				Update(_float _fTimeDelta);
	virtual void				Late_Update(_float _fTimeDelta);
	virtual HRESULT				Render();

public:
	void						On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject);
	void						On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject);
	void						On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject);


protected:
	//virtual HRESULT				Ready_ActorDesc(void* _pArg) override;
	//virtual HRESULT				Ready_Components() override;
	//virtual HRESULT				Ready_PartObjects() override;

	virtual void				Interact(CPlayer* _pUser);
	virtual _bool				Is_Interactable(CPlayer* _pUser);
	virtual _float				Get_Distance(COORDINATE _eCoord, CPlayer* _pUser);

public:
	static CWorldMapNpc_Jot*	Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*		Clone(void* _pArg) override; // Clone() 프로토 타입이나 객체의 복사시 사용된다.
	virtual void				Free() override;
	virtual HRESULT				Cleanup_DeadReferences() override; // 참조 중인 게임오브젝트들 중 죽은 Dead상태인 오브젝트를 체크해서 참조해제.(액티브 false인 애들때매 만듬)

private:
	void						On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx);
	void						ChangeState_Panel();

private:
	class CWorldMapNPC* m_pParent = { nullptr };
};

END
