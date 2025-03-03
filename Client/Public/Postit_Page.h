#pragma once
#include "Client_Defines.h"
#include "ModelObject.h"

BEGIN(Client)
class CPostit_Page : public CModelObject
{
public :
	enum POSTIT_PAGE_ANIM_TYPE
	{
		POSTIT_PAGE_TYPE_1
	};
private :
	CPostit_Page(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CPostit_Page(const CPostit_Page& _Prototype);
	virtual ~CPostit_Page() = default;

public :
	virtual HRESULT Initialize(void* _pArg);

public :


public:
	
	static CPostit_Page* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg);
	virtual void Free();
};
END
