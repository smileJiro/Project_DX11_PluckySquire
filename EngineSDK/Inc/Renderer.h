#pragma once
#include "Base.h"
BEGIN(Engine)
class CGameInstance;
class CComponent;
class CGameObject;
class CShader;
class CVIBuffer_Rect;

class CRenderer : public CBase
{
public:
	enum RENDERGROUP { RG_BOOK_2D, RG_PRIORITY, RG_SHADOW, RG_NONBLEND, RG_TRAIL, RG_EFFECT, RG_BLEND, RG_UI, RG_END };
	enum DSVTYPE { DSV_SHADOW, DSV_BOOK2D, DSV_LAST };
private:
	CRenderer(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CRenderer() = default;

public:
	HRESULT Initialize();
	HRESULT Add_RenderObject(RENDERGROUP _eRenderGroup, CGameObject* _pRenderObject);
	HRESULT Draw_RenderObject();

#ifdef _DEBUG
public: /* component에 대한 렌더링을 별도로 수행시키기 위해 >>> diferred shader를 통해 렌더링하다보면 콜라이더 등 렌더 상태가 이상해져서 별도 처리 .*/
	HRESULT Add_DebugComponent(CComponent* pDebugCom)
	{
		m_DebugComponents.push_back(pDebugCom);
		Safe_AddRef(pDebugCom);
		return S_OK;
	}

#endif

private:
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pContext;
	CGameInstance* m_pGameInstance = nullptr;
	list<CGameObject*> m_RenderObjects[RG_END]; 

private: /* 직교 투영으로 그리기 위한 */
	CShader* m_pShader = nullptr;
	CVIBuffer_Rect* m_pVIBuffer = nullptr;
	_float4x4 m_WorldMatrix{}, m_ViewMatrix{}, m_ProjMatrix{};
	_uint m_iOriginViewportWidth{}, m_iOriginViewportHeight{}; // 그림자 셰이딩을 위해선 고해상도의 RTV를 사용할 것이다. 이로인해 기존의 RTV의 size를 저장해두어야한다.
	ID3D11DepthStencilView* m_pShadowDepthStencilView = nullptr;
	ID3D11DepthStencilView* m_pBook2DDepthStencilView = nullptr;

#ifdef _DEBUG
private:
	list<CComponent*> m_DebugComponents;
	_bool m_isDebugRender = true;
#endif // _DEBUG


private:
	/* 10.10 추가 : Is_Dead() == true; Object 는 해당 그리기 작업을 완수하고, 실제 메모리가 해제된다. Renderer 에서 원본 객체가 소멸되는 것. */
	HRESULT Render_Book2D();
	HRESULT Render_Priority();
	HRESULT Render_Shadow();
	HRESULT Render_NonBlend();			// Diffuse, Normal 등 최종 화면을 그려내기위한 정보들을 RTV에 저장하는 단계			
	HRESULT Render_Lights();			// RTV에 저장된 데이터를 기반으로 Shade RTV를 구성하는 단계 >>> 명암 연산 처리 수행.
	HRESULT Render_Final();				// 지금까지 구성된 RTV를 기반으로 최종 화면을 그려내는 작업. 							
	HRESULT Render_Blend();
	HRESULT Render_Effect();
	HRESULT Render_After_Effect();
	HRESULT Render_UI();

private:
	HRESULT Ready_DepthStencilView(DSVTYPE _eType, _uint _iWidth, _uint _iHeight);
	HRESULT SetUp_Viewport(_uint _iWidth, _uint _iHeight);
#ifdef _DEBUG
private:
	HRESULT Render_Debug();
	
#endif

public:
	static CRenderer* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void Free() override;

};

END