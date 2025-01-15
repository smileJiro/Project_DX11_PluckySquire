#pragma once
#include "ModelObject.h"


BEGIN(Map_Tool)

class CMapObject final :  public CModelObject
{


public:
	enum MODE
	{
		NORMAL,
		Preview,
		PICKING,
		MODE_END

	};
	enum OBJ_CREATE_TYPE
	{
		OBJ_CREATE,
		OBJ__LOAD,
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

	virtual bool			Check_Picking(_fvector _vRayPos, _fvector _vRayDir, _float3* _vReturnPos, _float* fNewDist);


private :
	OPERATION				m_CurrentGizmoOperation = TRANSLATE;
	MODE					m_eMode = NORMAL;

	wstring					m_strModelName;
	_float4x4				m_matWorld;


public:
	static CMapObject*		Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*	Clone(void* _pArg) override;
	virtual void			Free() override;
};
END
