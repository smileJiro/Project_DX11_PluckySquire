#pragma once
#include "ModelObject.h"


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
		_float4x4		matWorld;
		OBJ_CREATE_TYPE eCreateType;

	}MAPOBJ_DESC;

public :
	typedef struct tagDiffuseInfo
	{
		_uint	iMaterialIndex;
		_uint	iDiffuseIIndex;
		_tchar	szTextureName[MAX_PATH];
		ID3D11ShaderResourceView* pSRV;
	}DIFFUSE_INFO;

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
	virtual HRESULT			Render_Shadow() override;

	void					Set_Operation(OPERATION _eOper) { m_CurrentGizmoOperation = _eOper; }
	OPERATION				Get_Operation() { return m_CurrentGizmoOperation; }

	void					Set_Mode(MODE _eMode) { m_eMode = _eMode; }

	void					Create_Complete();

	const wstring&			Get_ModelName() { return m_strModelName; }

#ifdef _DEBUG
	HRESULT					Get_Textures(vector<DIFFUSE_INFO>& _Diffuses,_uint _eTextureType = aiTextureType_DIFFUSE);
	HRESULT					Add_Textures(DIFFUSE_INFO& _tDiffuseInfo,_uint _eTextureType = aiTextureType_DIFFUSE);
	CController_Model*		Get_ModelController() { return m_pControllerModel; }
	HRESULT					Save_Override_Material(HANDLE _hFile);
	HRESULT					Load_Override_Material(HANDLE _hFile);
#endif // _DEBUG

	void					Change_TextureIdx(_uint _iIndex, _uint _eTextureType, _uint _iMaterialIndex = aiTextureType_DIFFUSE);
	_uint					Get_TextureIdx(_uint _eTextureType, _uint _iMaterialIndex = aiTextureType_DIFFUSE);

private :
	OPERATION				m_CurrentGizmoOperation = TRANSLATE;
	MODE					m_eMode = NORMAL;

	wstring					m_strModelName;
	_float4x4				m_matWorld;

	_uint					m_iDiffuseIndex = 0;
public:
	static CMapObject*		Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*	Clone(void* _pArg) override;
	virtual void			Free() override;
};
END
