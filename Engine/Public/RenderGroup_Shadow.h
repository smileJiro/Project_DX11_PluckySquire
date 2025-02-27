#pragma once
#include "RenderGroup.h"

BEGIN(Engine)
class CLight;
class ENGINE_DLL CRenderGroup_Shadow final : public CRenderGroup
{
protected:
	CRenderGroup_Shadow(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CRenderGroup_Shadow() = default;

public:
	virtual HRESULT				Render(CShader* _pRTShader, CVIBuffer_Rect* _pRTBuffer);

public:
	virtual HRESULT				Add_RenderObject(CGameObject* _pGameObject);
	HRESULT						Add_ShadowLight(CLight* _pShadowLight);
	HRESULT						Remove_ShadowLight(_int _iShadowLightID);
	HRESULT						Clear_ShadowLight();

private: /* Shadow Light의 정보를 기반으로 MRT를 세팅하고 ShadowMap을 그려낸다. 그려낸 각각의 Light들도 소유하고있으니, 추후 Lighting 과정에서 사용가능 */
	vector<CLight*>				m_ShadowLights; 

public:
	static CRenderGroup_Shadow* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* _pDesc);
	virtual void Free() override;
};

END