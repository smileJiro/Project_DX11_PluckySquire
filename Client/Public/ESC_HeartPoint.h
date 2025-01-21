#pragma once
#include "SettingPanel.h"

BEGIN(Engine)
class CShader;
class CModel;
class CVIBuffer_Collider;
END

class ESC_HeartPoint : public CSettingPanel
{


protected:
	explicit ESC_HeartPoint(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit ESC_HeartPoint(const ESC_HeartPoint& _Prototype);
	virtual ~ESC_HeartPoint() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* _pArg) override;
	virtual void			Priority_Update(_float _fTimeDelta) override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;

	//virtual void			Child_Update(_float _fTimeDelta) override;
	//virtual void			Child_LateUpdate(_float _fTimeDelta) override;

private:
	_int					m_PlayerHP = { 4 };

protected:
	virtual HRESULT			Ready_Components() override;

public:
	static ESC_HeartPoint*	Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*	Clone(void* _pArg);
	virtual void			Free() override;
	//HRESULT					Cleanup_DeadReferences() override;



};

