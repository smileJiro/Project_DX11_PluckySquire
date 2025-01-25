#pragma once
#include "UI.h"
#include "ShopPanel.h"

BEGIN(Engine)
class CShader;
class CModel;
class CVIBuffer_Collider;
END

class CShopPanel_BG : public CShopPanel
{


protected:
	explicit CShopPanel_BG(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CShopPanel_BG(const CShopPanel_BG& _Prototype);
	virtual ~CShopPanel_BG() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* _pArg) override;
	virtual void			Priority_Update(_float _fTimeDelta) override;
	virtual void			Update(_float _fTimeDelta) override {};
	virtual void			Late_Update(_float _fTimeDelta) override {};
	virtual void			Child_Update(_float _fTimeDelta) override;
	virtual void			Child_LateUpdate(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;


private:
	void					isRender();

protected:
	virtual HRESULT			Ready_Components() override;

public:
	static CShopPanel_BG*	Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*	Clone(void* _pArg);
	virtual void			Free() override;
	HRESULT					Cleanup_DeadReferences() override;


protected:
	_uint					m_iTextureCount = { 0 };




};

