#pragma once
#include "Base.h"
BEGIN(Engine)
class CGameObject;
class CGameInstance;
class CShader;
class CVIBuffer_Rect;
class ENGINE_DLL CRenderGroup : public CBase
{
public:
	typedef struct tagRGDesc
	{
		// RenderGroup : 기존 RenderGroup보다 큰 개념의 Group
		_int iRenderGroupID = -1;
		// PriorityID : 기존 RenderGroup의 개념(Blend, NonBlend, UI...)
		_int iPriorityID = -1;

	}RG_DESC;
protected:
	CRenderGroup(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CRenderGroup() = default;

public:
	virtual HRESULT				Initialize(void* _pArg);
	virtual HRESULT				Render(CShader* _pRTShader, CVIBuffer_Rect* _pRTBuffer);

	HRESULT						Add_RenderObject(CGameObject* _pGameObject);

protected:
	ID3D11Device*				m_pDevice = nullptr;
	ID3D11DeviceContext*		m_pContext = nullptr;
	CGameInstance*				m_pGameInstance = nullptr;

public:
	_int						Get_RenderGroupID() const		{ return m_iRenderGroupID; }
	_int						Get_PriorityID() const			{ return m_iPriorityID; }
	_int						Get_FinalID() const				{ return m_iFinalID; }
protected:
	// RenderGroupID : 대분류 
	_int						m_iRenderGroupID = -1;
	// PriorityID : 같은 RenderGroupID 중에서도 우선순위를 결정.
	_int						m_iPriorityID = -1;
	// FinalRenderGroupID : m_iRenderGroupID + m_iPriorityID >>> Renderer에 등록되는 최종 Key값이 된다.
	_int						m_iFinalID = -1;
protected:
	// RenderGroup Object
	list<CGameObject*>			m_GroupObjects;

protected:
	HRESULT						Setup_Viewport(_float2 _vViewportSize);

public:
	static CRenderGroup* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* _pDesc);
	virtual void Free() override;
};
END
