#pragma once
#include "ModelObject.h"
#include "AnimEventReceiver.h"

BEGIN(Engine)
class CAnimEventGenerator;
END

BEGIN(Client)

class CSampleBook final : public CModelObject, public IAnimEventReceiver
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
	virtual void			Priority_Update(_float _fTimeDelta) override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;
	virtual HRESULT			Render_Shadow() override;
	virtual HRESULT			Render_WorldPosMap() override;

	_bool					Book_Action(BOOK_PAGE_ACTION _eAction);
	void					PageAction_End(COORDINATE _eCoord, _uint iAnimIdx);
	void					PageAction_Call_PlayerEvent();

public :


private :
	CAnimEventGenerator*	m_pAnimEventGenerator = { nullptr };
	BOOK_PAGE_ACTION		m_eCurAction = ACTION_LAST;
	ID3D11Texture2D*		m_pStagingTexture = nullptr;

	_bool					m_isAction = { false };

public:
	static CSampleBook* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;

};

END