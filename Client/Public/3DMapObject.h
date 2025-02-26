#pragma once
#include "MapObject.h"
#include "3DModel.h"


BEGIN(Client)

class C3DMapObject abstract : public CMapObject
{
public:
	typedef struct tag3DMapObjectDesc : public CMapObject::MAPOBJ_DESC
	{
		// 오버라이드 정보가 있는지?
		_bool isOverride = false;
		// 프러스텀 컬링할지 여부를 판단한다.
		_bool isCulling = true;
		_bool isSksp = false;
		_string strSkspTag = "";
	}MAPOBJ_3D_DESC;

protected:
	C3DMapObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	C3DMapObject(const C3DMapObject& _Prototype);
	virtual ~C3DMapObject() = default;

public:
	virtual HRESULT					Initialize_Prototype() override;
	virtual HRESULT					Initialize(void* _pArg) override;
	virtual void					Priority_Update(_float _fTimeDelta) override;
	virtual void					Update(_float _fTimeDelta) override;
	virtual void					Late_Update(_float _fTimeDelta) override;
	virtual HRESULT					Render() override;


	virtual void					Set_Diffuse_Color(const _float4 _fDiffuseColor) { m_fDefaultDiffuseColor = _fDiffuseColor; };
	virtual const _float4			Get_Diffuse_Color() { return m_fDefaultDiffuseColor; }

	void							Set_Color_Shader_Mode(C3DModel::COLOR_SHADER_MODE _eMode) { m_eColorShaderMode = _eMode; }
	C3DModel::COLOR_SHADER_MODE		Get_Color_Shader_Mode() { return m_eColorShaderMode; }

protected:
	_uint							m_iDiffuseIndex = 0;

	C3DModel::COLOR_SHADER_MODE		m_eColorShaderMode = C3DModel::COLOR_NONE;

	// magenta
	_float4							m_fDefaultDiffuseColor = { 1.f, 0.f, 1.f, 1.f };
	_bool							m_isCulling = true;

public:
	virtual void			Free() override;
};
END
