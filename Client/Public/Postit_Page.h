#pragma once
#include "Client_Defines.h"
#include "ModelObject.h"

BEGIN(Client)
class CPostit_Page : public CModelObject
{
private :
	CPostit_Page(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CPostit_Page(const CPostit_Page& _Prototype);
	virtual ~CPostit_Page() = default;

public :
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* _pArg);
	virtual HRESULT Render();
	virtual HRESULT Render_Shadow(_float4x4* _pViewMatrix, _float4x4* _pProjMatrix);
	virtual HRESULT Change_Coordinate(COORDINATE _eCoordinate, _float3* _pNewPosition = nullptr);
	virtual void Check_FrustumCulling();


public:
	
	static CPostit_Page* Create();
	virtual void Free();
};
END
