#pragma once
#include "SettingPanel.h"

BEGIN(Engine)
class CShader;
class CModel;
class CVIBuffer_Collider;
END

class CESC_Back : public CSettingPanel
{


protected:
	explicit CESC_Back(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CESC_Back(const CESC_Back& _Prototype);
	virtual ~CESC_Back() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* _pArg) override;
	virtual void			Priority_Update(_float _fTimeDelta) override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;


protected:
	virtual HRESULT			Ready_Components() override;

public:
	static CESC_Back*	Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*	Clone(void* _pArg);
	virtual void			Free() override;
	//HRESULT					Cleanup_DeadReferences() override;



};

