#pragma once
#include "UI.h"

BEGIN(Engine)
class CShader;
class CModel;
class CVIBuffer_Collider;
END

BEGIN(Client)

class CSettingPanel : public CUI
{
protected:
	explicit CSettingPanel(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CSettingPanel(const CSettingPanel& _Prototype);
	virtual ~CSettingPanel() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* _pArg) override;
	virtual void			Priority_Update(_float _fTimeDelta) override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;
	virtual void			Child_Update(_float _fTimeDelta) {}
	virtual void			Child_LateUpdate(_float _fTimeDelta) {}
	virtual HRESULT			Render(_int _index = 0) override;


protected:
	CUI::SETTINGPANEL		m_eSettingPanel = CUI::SETTINGPANEL::SETTING_END;

protected:
	CUI::SETTINGPANEL		Get_SettingPanel() { return m_eSettingPanel; }


private:
	void					isRender();

protected:
	virtual HRESULT			Ready_Components() override;

public:
	static CSettingPanel*	Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*	Clone(void* _pArg);
	virtual void			Free() override;
	HRESULT					Cleanup_DeadReferences() { return S_OK; };
};

END

