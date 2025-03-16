#pragma once

#include "UI.h"

BEGIN(Engine)
class CShader;
class CModel;
class CVIBuffer_Collider;
END

BEGIN(Client)
class CLogo_BackGround final : public CUI
{
public:
	enum BACKGROUND_TYPE
	{
		MAIN_JOT,
		MAIN_HUMGRUMP,

		BACKGROUND_END
	};

	enum BG_CHARACTER_ANIM
	{
		INTO,
		LOOP,

		CHARACTER_ANIM_END
	};

	enum MAIN_PART
	{
		LOGO_CHARACTER,
		LOGO_TEXT_OBJECT,

		MAIN_PART_END
	};

	typedef struct tagMainLogoDesc : public CUIObject::UIOBJECT_DESC
	{
		_float4		vColor = {};
		_uint		iBackGroundMainType = {};

	}MAIN_LOGO_DESC;

protected:
	explicit CLogo_BackGround(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CLogo_BackGround(const CLogo_BackGround& _Prototype);
	virtual ~CLogo_BackGround() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* _pArg) override;
	virtual void			Priority_Update(_float _fTimeDelta) override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;

private:
	//_float3					m_vBackGroundColor = {};
	_uint					m_eBackGroundType = { BACKGROUND_END };

	class CModelObject*		m_pBackGroundParts[MAIN_PART_END] = { nullptr };

protected:
	virtual HRESULT			Ready_Components() override;
	HRESULT					Ready_Objects();

private:
	void					On_End_Animation(COORDINATE _eCoordinate, _uint _iAnimIndex);

public:
	static CLogo_BackGround* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*	Clone(void* _pArg);
	virtual void			Free() override;
	HRESULT					Cleanup_DeadReferences() override;
};
END
