#pragma once
#include "Client_Defines.h"
#include "ModelObject.h"
#include "AnimEventReceiver.h"
#include "Interactable.h"

BEGIN(Engine)
class CAnimEventGenerator;
END

BEGIN(Client)

class CSampleBook final : public CModelObject, public IAnimEventReceiver, public IInteractable
{
public:
	enum BOOK_PAGE_ACTION
	{
		PREVIOUS,
		NEXT,
		ACTION_LAST
	};

	enum BOOK_ANIM_ACTION
	{
		ANIM_ACTION_NONE = 0, 
		ANIM_ACTION_MAGICDUST = 5,
		ANIM_ACTION_CLOSEBYHAND = 12,
	};

	enum BOOK_ANIMATION
	{
		IDLE = 0 ,
		PAGE_IMPACT,
		OPEN_TO_FLAT,
		OPEN_IDLE,
		OPEN,
		MAGICDUST = 5,
		FLATTEN,

		FLAT_PAGE_TURN = 8,
		ACTION = 8,

		CLOSED_IDLE = 10,
		CLOSE_R_TO_L = 11,
		CLOSE_L_TO_R = 12,
		BOOKOUT_CLOSE,
		ANIMATION_LAST
	};
	
	enum BOOK_MESH_INDEX
	{
		NEXT_RIGHT = 8,
		NEXT_LEFT = 9,
		CUR_RIGHT = 10,
		CUR_LEFT = 11,
		INDEX_LAST
	};


private:

	CSampleBook(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CSampleBook(const CSampleBook& _Prototype);
	virtual ~CSampleBook() = default;
public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* _pArg) override;
	HRESULT					Init_RT_RenderPos_Capcher();


public:
	virtual void OnTrigger_Enter(const COLL_INFO& _My, const COLL_INFO& _Other)override;
	virtual void OnTrigger_Stay(const COLL_INFO& _My, const COLL_INFO& _Other)override;
	virtual void OnTrigger_Exit(const COLL_INFO& _My, const COLL_INFO& _Other)override;
	virtual void Check_FrustumCulling() { m_isFrustumCulling = false; }// 항상 컬링하지않음. };
public:
	void Interact(CPlayer* _pUser) override;
	_bool Is_Interactable(CPlayer* _pUser) override;
	_float Get_Distance(COORDINATE _eCoord, CPlayer* _pUser) override;

public :	
	virtual void			Priority_Update(_float _fTimeDelta) override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;

	virtual HRESULT			Render_WorldPosMap(const _wstring& _strCopyRTTag, const _wstring& _strSectionTag) override;

	_bool					Book_Action(BOOK_PAGE_ACTION _eAction);
	void					PageAction_End(COORDINATE _eCoord, _uint iAnimIdx);
	void					PageAction_Call_PlayerEvent();
	BOOK_PAGE_ACTION		Get_ActionType() { return m_eCurAction; }
	_bool					Get_PlayerAround() { return m_isPlayerAround; }

	_bool					Is_DuringAnimation();

	void					SlideObjects_RToL();
	void					SlideObjects_LToR();
public:						
	HRESULT					Execute_Action(BOOK_PAGE_ACTION _eAction, _float3 _fNextPosition);
	void					Execute_AnimEvent(_uint _iAnimIndex);
	HRESULT					Convert_Position_3DTo2D(_fvector _v3DPos, _vector* _pOutPosition);

private:
	//void					Calc_Page3DWorldMinMax();	
private :
	CAnimEventGenerator*	m_pAnimEventGenerator = { nullptr };
	BOOK_PAGE_ACTION		m_eCurAction = ACTION_LAST;
	BOOK_ANIM_ACTION		m_eAnimAction = ANIM_ACTION_NONE;
	_float					m_fAccAnimTime = 0.f;
	_float3					m_fNextPos = {};
	_bool					m_isAction = { false };
	_bool					m_isPlayerAround= { false };
	_bool					m_isPlayerAbove= { false };




public:
	void					Change_RenderState(RT_RENDERSTATE _eRenderState);

private: /* Render State */
	RT_RENDERSTATE		m_eCurRenderState = RT_RENDERSTATE::RENDERSTATE_LIGHT;

public:
	static CSampleBook* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;



};

END