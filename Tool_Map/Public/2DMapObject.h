#pragma once

#include "Map_Tool_Defines.h"
#include "UIObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END


BEGIN(Map_Tool)

class C2DMapObject final : public CUIObject
{
public:
	typedef struct tag2DMapObjectDesc : CUIObject::UIOBJECT_DESC
	{
		_wstring strProtoTag;
		_float2 fRenderTargetSize;
		_float2 fTextureOffsetSize;
		//_float2 fDefaultPos;

	}MAPOBJ_2D_DESC;
private:
	C2DMapObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	C2DMapObject(const C2DMapObject& Prototype);
	virtual ~C2DMapObject() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void			Set_OffsetPos(_float2 _fPos);
	
private:
	CShader*		m_pShader = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CTexture*		m_pTextureCom = { nullptr };

	_float2			m_fTextureOffsetSize;

private:
	HRESULT Ready_Components(MAPOBJ_2D_DESC* Desc);
	HRESULT Bind_ShaderResources();

public:
	static C2DMapObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;



	// CUIObject을(를) 통해 상속됨
	virtual HRESULT Cleanup_DeadReferences() override;

};

END