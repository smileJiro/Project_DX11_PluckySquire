#pragma once
#include "Client_Defines.h"
#include "ModelObject.h"

BEGIN(Client)
class CPostit_Page : public CModelObject
{
public :
	enum POSTIT_PAGE_ANIM_TYPE
	{
		POSTIT_PAGE_APPEAR,
		POSTIT_PAGE_DISAPPEAR,
		POSTIT_PAGE_COLOR_UP,
		POSTIT_PAGE_COLOR_DOWN,
		POSTIT_PAGE_IDLE,
		POSTIT_PAGE_TALK_1,
		POSTIT_PAGE_TALK_2,
		POSTIT_PAGE_COLOR_TALK_UP,
		POSTIT_PAGE_COLOR_TALK_DOWN,
		POSTIT_PAGE_LAST
	};
	enum POSTIT_PAGE_POSTION_TYPE
	{
		POSTIT_PAGE_POSTION_TYPE_A,
		POSTIT_PAGE_POSTION_TYPE_B,
		POSTIT_PAGE_POSTION_TYPE_C,
		POSTIT_PAGE_POSTION_TYPE_D,
		POSTIT_PAGE_POSTION_TYPE_E,
		POSTIT_PAGE_POSTION_TYPE_F,
		POSTIT_PAGE_POSTION_TYPE_G,
		POSTIT_PAGE_POSTION_TYPE_H,
		POSTIT_PAGE_POSTION_TYPE_LAST
	};

public:
	typedef struct tagPostitPage : public CModelObject::MODELOBJECT_DESC
	{
		_wstring strInitSkspName;
		POSTIT_PAGE_POSTION_TYPE eFirstPostion = POSTIT_PAGE_POSTION_TYPE_A;
	}POSTIT_PAGE_DESC;

private :
	CPostit_Page(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CPostit_Page(const CPostit_Page& _Prototype);
	virtual ~CPostit_Page() = default;

public :
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* _pArg) override;
public :
	void Anim_Action(POSTIT_PAGE_ANIM_TYPE eType, _bool _isLoop, POSTIT_PAGE_POSTION_TYPE _ePostionType = POSTIT_PAGE_POSTION_TYPE_A);
	void Anim_Action_End(COORDINATE _eCoord, _uint iAnimIdx);

	POSTIT_PAGE_POSTION_TYPE m_ePosition;
	POSTIT_PAGE_ANIM_TYPE	m_eAnimAction;
	_float2 m_Positions[POSTIT_PAGE_POSTION_TYPE_LAST];
public:
	
	static CPostit_Page* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg);
	virtual void Free();
};
END
