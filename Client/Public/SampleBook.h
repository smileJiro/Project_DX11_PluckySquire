#pragma once
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


	enum BOOK_ANIMATION
	{
		IDLE = 0 ,
		ACTION = 8,
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

public:
	void Interact(CPlayer* _pUser) override;
	_bool Is_Interactable(CPlayer* _pUser) override;
	_float Get_Distance(COORDINATE _eCoord, CPlayer* _pUser) override;
public :	
	virtual void			Priority_Update(_float _fTimeDelta) override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;
	virtual HRESULT			Render_Shadow() override;
	virtual HRESULT			Render_WorldPosMap(const _wstring& _strCopyRTTag, const _wstring& _strSectionTag) override;

	_bool					Book_Action(BOOK_PAGE_ACTION _eAction);
	void					PageAction_End(COORDINATE _eCoord, _uint iAnimIdx);
	void					PageAction_Call_PlayerEvent();


public:						
	HRESULT					Execute_Action(BOOK_PAGE_ACTION _eAction, _float3 _fNextPosition);

private :
	CAnimEventGenerator*	m_pAnimEventGenerator = { nullptr };
	BOOK_PAGE_ACTION		m_eCurAction = ACTION_LAST;
	_float3					m_fNextPos = {};
	_bool					m_isAction = { false };
	_bool					m_isPlayerAround= { false };
public:
	static CSampleBook* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;



};

END