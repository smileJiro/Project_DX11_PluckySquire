#pragma once
#include "SettingPanel.h"

BEGIN(Engine)
class CShader;
class CModel;
class CVIBuffer_Collider;
END

class CSettingPanelBG final : public CSettingPanel
{
protected:
	explicit CSettingPanelBG(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CSettingPanelBG(const CSettingPanelBG& _Prototype);
	virtual ~CSettingPanelBG() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* _pArg) override;
	virtual void			Priority_Update(_float _fTimeDelta) override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;

	virtual void			Child_Update(_float _fTimeDelta) override;
	virtual void			Child_LateUpdate(_float _fTimeDelta) override;


private:
	
	_uint					m_iTextureCount = { 0 };

protected:
	virtual HRESULT			Ready_Components() override;
	void					isRender();

public:
	static CSettingPanelBG*	Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*	Clone(void* _pArg);
	virtual void			Free() override;
	HRESULT					Cleanup_DeadReferences() override;



};

