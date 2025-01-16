#pragma once
#include "PartObject.h"
BEGIN(Engine)
class CVIBuffer_Rect;
class CModel;
class CController_Model;
class ENGINE_DLL CModelObject : public CPartObject
{
public:
	typedef struct tagModelObjectDesc : public CPartObject::PARTOBJECT_DESC
	{
		_uint iModelPrototypeLevelID_2D;
		_uint i3DModelPrototypeLevelID;
		_wstring strModelPrototypeTag_2D;
		_wstring strModelPrototypeTag_3D;

		_wstring strShaderPrototypeTag_2D;
		_wstring strShaderPrototypeTag_3D;

		// 2D ShaderPass
		_uint iShaderPass_2D = {};
		// 3D ShaderPass
		_uint iShaderPass_3D = {};
	}MODELOBJECT_DESC;

protected:
	CModelObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CModelObject(const CModelObject& _Prototype);
	virtual ~CModelObject() = default;

public:
	virtual HRESULT Initialize_Prototype(); // 프로토 타입 전용 Initialize
	virtual HRESULT Initialize(void* _pArg); // 초기화 시 필요한 매개변수를 void* 타입으로 넘겨준다.
	virtual void			Late_Update(_float _fTimeDelta) override;
	virtual HRESULT Render_Shadow() { return S_OK; }
	virtual HRESULT			Render() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT				Change_Coordinate(COORDINATE _eCoordinate, const _float3& _vPosition) override;

public:
	_bool Is_PickingCursor_Model(_float2 _fCursorPos, _float& _fDst);
	//const _float4x4* Find_BoneMatrix(const _char* _pBoneName) const;

public:
	// Get
	_uint Get_ShaderPassIndex(COORDINATE _eCurCoord) { return m_iShaderPasses[_eCurCoord]; }
	
	// Set
	void Set_AnimationLoop(_uint iIdx, _bool bIsLoop);
	void Set_Animation(_uint iIdx);
	void Switch_Animation(_uint iIdx);

protected:
	CController_Model*		m_pControllerModel = nullptr;
	_float4x4				m_ViewMatrix{}, m_ProjMatrix{}; /* 2D 렌더링 전용 VP */

	CShader*				m_pShaderComs[COORDINATE_LAST] = {};
	_uint					m_iShaderPasses[COORDINATE_LAST] = {};
protected:
	virtual HRESULT			Bind_ShaderResources_WVP();


protected:
	HRESULT					Ready_Components(MODELOBJECT_DESC* _pDesc);


public:
	static CModelObject* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;

#ifdef _DEBUG
public:
	HRESULT Imgui_Render_ObjectInfos() override;
#endif // _DEBUG
};
END
