#pragma once
#include "ModelObject.h"
#include "3DModel.h"


BEGIN(Camera_Tool)

class CMapObject final :  public CModelObject
{
private:
	CMapObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CMapObject(const CMapObject& _Prototype);
	virtual ~CMapObject() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* _pArg) override;
	virtual void			Priority_Update(_float _fTimeDelta) override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;
	virtual HRESULT			Render_Shadow() override;

	virtual void			Set_Diffuse_Color(const _float4 _fDiffuseColor) { m_fDefaultDiffuseColor = _fDiffuseColor; };
	virtual const _float4	Get_Diffuse_Color() { return m_fDefaultDiffuseColor; }

	void					Set_Color_Shader_Mode(C3DModel::COLOR_SHADER_MODE _eMode) { m_eColorShaderMode = _eMode; }
	C3DModel::COLOR_SHADER_MODE		Get_Color_Shader_Mode() { return m_eColorShaderMode; }

#ifdef _DEBUG
	CController_Model*		Get_ModelController() { return m_pControllerModel; }


	//HRESULT					Load_Override_Material(HANDLE _hFile);
	//HRESULT					Load_Override_Color(HANDLE _hFile);

#endif // _DEBUG

private :
	wstring					m_strModelName = L"";
	_float4x4				m_matWorld;

	_uint					m_iDiffuseIndex = 0;


	C3DModel::COLOR_SHADER_MODE		m_eColorShaderMode = C3DModel::COLOR_NONE;

	// magenta
	_float4						m_fDefaultDiffuseColor = { 1.f, 0.f, 1.f, 1.f };


public:
	static CMapObject*		Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*	Clone(void* _pArg) override;
	virtual void			Free() override;
};
END
