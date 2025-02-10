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
		_float2 fRenderTargetSize;
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

	HRESULT	Change_RenderGroup(_float2 _fRenderTargetSize, _int _iRenderGroupID, _int _iPriorityID);


	void Texture_Output(const _wstring& _strTexturePath);
	void Set_Texture_Mode(const _string& _strTextureName);
	void Set_Default_Render_Mode();
	void Set_Default_RenderTarget_Name(const _wstring& _strRenderTargetTag) { m_DefaultRenderTargetName = _strRenderTargetTag; };

public :
	void Set_Color(_float4& fColor) { m_fBackColor = fColor; }

	_bool	Is_2DMode() { return m_is2DMode; }
	_bool	IsCursor_In(_float2 _fCursorPos);

private:
	CShader*		m_pShader = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CTexture*		m_pTextureCom = { nullptr };

	_bool			m_isDefaultRenderMode = true;
	_bool			m_isBackColorRender = false;
	_bool			m_is2DMode = true;
	_float4x4		m_TargetProjMatrix{};
	_float4			m_fBackColor = {1.f,0.f,1.f,1.f};
	_float2			m_fTargetSize = {};

	_wstring		m_DefaultRenderTargetName = L"";

	_int			m_iRenderGroupID = RG_2D;
	_int			m_iPriorityID = PR2D_BOOK_SECTION;

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