#pragma once
#include "ModelObject.h"
#include "3DModel.h"

BEGIN(Map_Tool)

class CMapObject final :  public CModelObject
{


public:
	enum MODE
	{
		NORMAL,
		PREVIEW,
		PICKING,
		MODE_END

	};
	enum OBJ_CREATE_TYPE
	{
		OBJ_CREATE,
		OBJ_LOAD,
		OBJ_CREATE_END
	};

	enum OPERATION
	{
		TRANSLATE = 7,
		ROTATE = 120,
		SCALE = 896
	};
public:
	typedef struct tagMapObjectDesc : public CModelObject::MODELOBJECT_DESC
	{
		_tchar			szModelName[MAX_PATH];
		OBJ_CREATE_TYPE eCreateType;

	}MAPOBJ_DESC;

public :
	typedef struct tagTextureInfo
	{
		_uint	iMaterialIndex;
		_uint	iTextureIndex;
		_tchar	szTextureName[MAX_PATH];
		ID3D11ShaderResourceView* pSRV;
	}TEXTURE_INFO;

	typedef struct tagOverride
	{
		_uint iMaterialIndex;
		_uint iTexTypeIndex;
		_uint iTexIndex;
	}OVERRIDE_MATERIAL_INFO;

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
	HRESULT					Render_Preview(_float4x4* _ViewMat, _float4x4* _ProjMat);

	void					Set_Operation(OPERATION _eOper) { m_CurrentGizmoOperation = _eOper; }
	OPERATION				Get_Operation() { return m_CurrentGizmoOperation; }

	void					Set_Mode(MODE _eMode) { m_eMode = _eMode; }

	void					Create_Complete();

	const wstring&			Get_ModelName() { return m_strModelName; }


	virtual void					Set_Diffuse_Color(_uint _iMaterialIndex, const _float4 _fDiffuseColor);
	virtual const _float4			Get_Diffuse_Color(_uint _iMaterialIndex);

	void							Set_Color_Shader_Mode(_uint _iMaterialIndex, C3DModel::COLOR_SHADER_MODE _eMode);
	C3DModel::COLOR_SHADER_MODE		Get_Color_Shader_Mode(_uint _iMaterialIndex);

#ifdef _DEBUG
	HRESULT					Get_Textures(vector<TEXTURE_INFO>& _Diffuses,_uint _eTextureType = aiTextureType_DIFFUSE);
	HRESULT					Add_Textures(TEXTURE_INFO& _tDiffuseInfo,_uint _eTextureType = aiTextureType_DIFFUSE);
	HRESULT					Delete_Texture(_uint _iIndex, _uint _eTextureType, _uint _iMaterialIndex = aiTextureType_DIFFUSE);
	HRESULT					Push_Texture(const _string _strTextureName, _uint _eTextureType = aiTextureType_DIFFUSE);
	CController_Model*		Get_ModelController() { return m_pControllerModel; }

	HRESULT					Add_Texture_Type(_uint _eTextureType);

	HRESULT					Save_Override_Material(HANDLE _hFile);
	HRESULT					Save_Override_Color(HANDLE _hFile);
	HRESULT					Save_Sksp(HANDLE _hFile);
	HRESULT					Load_Override_Material(HANDLE _hFile);
	HRESULT					Load_Override_Color(HANDLE _hFile);
	HRESULT					Load_Sksp(HANDLE _hFile);

	_uint					Get_MaterialCount();


#endif // _DEBUG


	void Set_SkspTag(const _string& _strSpskTag) { m_strSpskTag = _strSpskTag; }
	void Set_SkspType(_uint _iSpskType) { m_iSkspType = _iSpskType; }
	
	const _string& Set_SkspTag() { return m_strSpskTag; }
	_uint Get_SkspType() { return m_iSkspType; }

private :
	OPERATION				m_CurrentGizmoOperation = TRANSLATE;
	MODE					m_eMode = NORMAL;

	wstring					m_strModelName = L"";
	_float4x4				m_matWorld;

	_uint					m_iDiffuseIndex = 0;
	_uint					m_iSkspType = 0;
	_string					m_strSpskTag = "";

	vector<C3DModel::COLOR_SHADER_MODE>		m_eColorShaderModes;

public:
	static CMapObject*		Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*	Clone(void* _pArg) override;
	virtual void			Free() override;
};
END
