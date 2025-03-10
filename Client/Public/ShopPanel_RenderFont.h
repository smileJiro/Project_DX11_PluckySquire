#pragma once
#include "UI.h"
#include "ShopPanel_New.h"

BEGIN(Engine)
class CShader;
class CModel;
class CVIBuffer_Collider;
END

class CShopPanel_RenderFont : public CShopPanel_New
{


protected:
	explicit CShopPanel_RenderFont(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CShopPanel_RenderFont(const CShopPanel_RenderFont& _Prototype);
	virtual ~CShopPanel_RenderFont() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* _pArg) override;
	virtual void			Priority_Update(_float _fTimeDelta) override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;

public:
	void					isRender();
	void					Render_ItemInfo();
	void					Render_ItemDialog();
	

private:
	
	void					Cal_ShopYesNOPos(_float2 _vRTSize, _float2 _vBGPos);

protected:
	virtual HRESULT			Ready_Components() override;

public:
	static CShopPanel_RenderFont*	Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*	Clone(void* _pArg);
	virtual void			Free() override;
	HRESULT					Cleanup_DeadReferences() override;


protected:
	_uint					m_iTextureCount = { 0 };




};

