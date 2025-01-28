#pragma once

#include "Map_Tool_Defines.h"
#include "UIObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END


BEGIN(Map_Tool)

class C2DDefault_RenderObject final : public CUIObject
{
public:
	typedef struct tagDefaultRenderObjectDesc: public CUIObject::UIOBJECT_DESC
	{

	}DEFAULT_RENDER_OBJECT_DESC;

private:
	C2DDefault_RenderObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	C2DDefault_RenderObject(const C2DDefault_RenderObject& Prototype);
	virtual ~C2DDefault_RenderObject() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	_bool	Toggle_Mode() { m_is2DMode = !m_is2DMode; return m_is2DMode; }
public :
	void Set_Color(_float4& fColor) { m_fBackColor = fColor; }

	_bool	Is_2DMode() { return m_is2DMode; }
	_bool	IsCursor_In(_float2 _fCursorPos);
private:
	CShader*		m_pShader = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	
	_bool			m_isBackColorRender = false;
	_bool			m_is2DMode = true;
	_float4x4		m_TargetProjMatrix{};
	_float4			m_fBackColor = {1.f,0.f,1.f,1.f};
	_float2			m_fTargetSize = {};
private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();
	void	Update_RenderWorld();

public:
	static C2DDefault_RenderObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;



	// CUIObject을(를) 통해 상속됨
	virtual HRESULT Cleanup_DeadReferences() override;

};

END