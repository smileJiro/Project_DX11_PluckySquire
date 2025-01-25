#pragma once
#include "SettingPanel.h"

BEGIN(Engine)
class CShader;
class CModel;
class CVIBuffer_Collider;
END

class CESC_Goblin : public CSettingPanel
{


protected:
	explicit CESC_Goblin(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CESC_Goblin(const CESC_Goblin& _Prototype);
	virtual ~CESC_Goblin() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* _pArg) override;
	virtual void			Priority_Update(_float _fTimeDelta) override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;


private:
	_float					m_fMoveGoblinTime = { 0.0f };
	_float					m_fWaitGoblinTime = { 0.0f };

	_bool					m_isWait = {false};
	_bool					m_isMWait = { false };
	_bool					m_isUp = { false };
	_bool					m_isDown = { false };

protected:
	virtual HRESULT			Ready_Components() override;

public:
	static CESC_Goblin*	Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*	Clone(void* _pArg);
	virtual void			Free() override;
	//HRESULT					Cleanup_DeadReferences() override;



};

