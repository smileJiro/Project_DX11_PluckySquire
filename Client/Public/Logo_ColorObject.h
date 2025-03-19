#pragma once
#include "ModelObject.h"

BEGIN(Client)

class CLogo_ColorObject : public CModelObject
{
public:
	typedef struct tagColorObjectDesc : public CModelObject::MODELOBJECT_DESC
	{
		_float4 vColor = {};
		_uint	iColorObjectType = { COLOROBJECT_TYPE::DEFAULT };
		_bool	isBlinking = { false };
		_bool	isOrthogonal = { false };
	}COLOROBJECT_DESC;

	enum COLOROBJECT_TYPE
	{
		DEFAULT,
		LOGO_TEXTOBJECT,
		LOGO_TEXTOBJECT_BUTTON,

		COLOROBJECT_TYPE
	};

protected:
	CLogo_ColorObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CLogo_ColorObject(const CLogo_ColorObject& _Prototype);
	virtual ~CLogo_ColorObject() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void	Update(_float _fTimeDelta) override;
	virtual void	Late_Update(_float _fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_uint			m_eColorObjectType = {};

	_bool			m_isBlinking = {};
	_uint			m_iPreFadeAlphaState = { CModelObject::FADEALPHA_DEFAULT };

private:
	/* Active 변경시 호출되는 함수 추가. */
	virtual void	Active_OnEnable() override;
	virtual void	Active_OnDisable() override;

	void			Blinking();

public:
	static CLogo_ColorObject* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*	Clone(void* _pArg) override;
	virtual void			Free() override;
};

END
