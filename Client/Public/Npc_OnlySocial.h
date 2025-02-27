#pragma once
#include "Npc.h"
#include "Interactable.h"



BEGIN(Client)
class CNPC_OnlySocial final: public CNPC, public IInteractable
{

public:
	struct NPC_ONLYSOCIAL : public tagNPCDesc
	{
		_wstring	strSectionid;
		_wstring	strAnimationName;
		_int		strCreateSection;
		_wstring	strDialogueId;
		_int		iStartAnimation;
		_float		vPositionX;
		_float		vPositionY;
		_float		vPositionZ;
		_float		CollsionScaleX;
		_float		CollsionScaleY;
		_bool		isDialog;
		_bool		is2D;
	};


private:
	CNPC_OnlySocial(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CNPC_OnlySocial(const CNPC_OnlySocial& _Prototype);
	virtual ~CNPC_OnlySocial() = default;



public:
	virtual HRESULT				Initialize_Prototype();								// 프로토 타입 전용 Initialize
	virtual HRESULT				Initialize(void* _pArg);							// 초기화 시 필요한 매개변수를 void* 타입으로 넘겨준다.
	virtual void				Priority_Update(_float _fTimeDelta);				// 특정개체에 대한 참조가 빈번한 객체들이나, 등등의 우선 업데이트 되어야하는 녀석들.
	virtual void				Update(_float _fTimeDelta);
	virtual void				Late_Update(_float _fTimeDelta);
	virtual HRESULT				Render();

public:
	void						On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject) {};
	void						On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject) {};
	void						On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject) {};


public:
	static CNPC_OnlySocial*			Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*		Clone(void* _pArg) override; // Clone() 프로토 타입이나 객체의 복사시 사용된다.
	virtual void				Free() override;
	virtual HRESULT				Cleanup_DeadReferences() override; // 참조 중인 게임오브젝트들 중 죽은 Dead상태인 오브젝트를 체크해서 참조해제.(액티브 false인 애들때매 만듬)

private:
	void						On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx) {};
	HRESULT						LoadFromJson(const std::wstring& filePath);

protected:
	virtual void Interact(CPlayer* _pUser) {};
	virtual _bool Is_Interactable(CPlayer* _pUser) { return false; };
	virtual _float Get_Distance(COORDINATE _eCoord, CPlayer* _pUser) { return 0.f; };

	virtual HRESULT Ready_ActorDesc(void* _pArg) { return S_OK; };
	virtual HRESULT Ready_Components() { return S_OK; };
	virtual HRESULT Ready_PartObjects() { return S_OK; };





};

END
