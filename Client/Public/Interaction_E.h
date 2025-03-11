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
	void					Cal_PlayerHighPos(CGameObject* _pGameObject);
	void					Cal_ObjectPos(CGameObject* _pGameObject);
	void					Cal_DisplayPos(_float2 _vRTSize, CGameObject* _pGameObject);
	void					Display_Text(_float3 _vPos, _float2 _vRTSize, IInteractable* _pGameObject);

private:
	wstring					m_strIntaractName;
	wstring					m_strDisplayText;
	_float3					m_vObejctPos;

	_float					m_fScaledWidth;
	_float					m_fScaledHeight;

	_float					m_screenX;
	_float					m_screenY;
	wstring					m_preSectionName = { TEXT(" ") };

	_bool					m_isDeleteRender = { false };

	COORDINATE				m_ePrePlayerCoord = { COORDINATE_LAST };

private: /* ÅÂ¿õ Ãß°¡ */
	_float4					m_vInteractionColor = { 45.f / 255.f, 210.f / 255.f, 56.f / 255.f, 1.0f };
	_float					m_fInteractionRatio = 0.0f;

};

END

