#pragma once
#include "UI.h"

BEGIN(Engine)
class CShader;
class CModel;
class CVIBuffer_Collider;
END

BEGIN(Client)
class CInteraction_E : public CUI
{


protected:
	explicit CInteraction_E(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CInteraction_E(const CInteraction_E& _Prototype);
	virtual ~CInteraction_E() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* _pArg) override;
	virtual void			Priority_Update(_float _fTimeDelta) override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;

private:


protected:
	virtual HRESULT			Ready_Components() override;

public:
	static CInteraction_E*	Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*	Clone(void* _pArg);
	virtual void			Free() override;
	HRESULT					Cleanup_DeadReferences() override;

private:
	void					Cal_PlayerHighPos();
	void					Cal_ObjectPos(CGameObject* _pGameObject);
	void					Cal_DisplayPos(_float2 _vRTSize, CGameObject* _pGameObject);
	void					Display_Text(_float3 _vPos, _float2 _vRTSize, IInteractable* _pGameObject);

private:
	wstring					m_strIntaractName;
	wstring					m_strDisplayText;
	_float3					m_vObejctPos;


};

END

