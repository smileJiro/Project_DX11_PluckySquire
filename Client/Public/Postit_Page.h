#pragma once
#include "Client_Defines.h"
#include "ModelObject.h"

BEGIN(Client)
class CPostit_Page : public CModelObject
{
public :
	typedef struct tagPostitPage : public CModelObject::MODELOBJECT_DESC
	{
		_wstring strInitSkspName;

	}POSTIT_PAGE_DESC;
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
private :
	CPostit_Page(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CPostit_Page(const CPostit_Page& _Prototype);
	virtual ~CPostit_Page() = default;

public :
	virtual HRESULT Initialize(void* _pArg);

public :
	void Anim_Action(POSTIT_PAGE_ANIM_TYPE eType, _bool _isLoop);

public:
	
	static CPostit_Page* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg);
	virtual void Free();
};
END
