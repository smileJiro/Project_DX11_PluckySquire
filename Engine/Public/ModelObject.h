#pragma once
#include "PartObject.h"
BEGIN(Engine)
class CVIBuffer_Rect;
class C3DModel;
class ENGINE_DLL CModelObject abstract : public CPartObject
{
public:
	typedef struct tagModelObjectDesc : public CPartObject::PARTOBJECT_DESC
	{
		_wstring strShaderPrototypeTag_2D;
		_wstring strShaderPrototypeTag_3D;
		_wstring strModelPrototypeTag;

		_uint iShaderPass_2D = {};
		_uint iShaderPass_3D = {};
	}MODELOBJECT_DESC;

protected:
	CModelObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CModelObject(const CModelObject& _Prototype);
	virtual ~CModelObject() = default;

public:
	virtual HRESULT Initialize_Prototype(); // 프로토 타입 전용 Initialize
	virtual HRESULT Initialize(void* _pArg); // 초기화 시 필요한 매개변수를 void* 타입으로 넘겨준다.
	virtual HRESULT Render_Shadow() { return S_OK; }

public:
	//const _float4x4* Find_BoneMatrix(const _char* _pBoneName) const;

public:
	// Get
	
	// Set

protected:
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;
	C3DModel*					m_pModelCom = nullptr;
	_float4x4				m_ViewMatrix{}, m_ProjMatrix{}; /* 2D 렌더링 전용 VP */

	CShader*				m_pShaderComs[COORDINATE_LAST] = {};
	_uint					m_iShaderPasses[COORDINATE_LAST] = {};

protected:
	virtual HRESULT			Bind_ShaderResources_WVP();
	virtual HRESULT			Render_2D();
	virtual HRESULT			Render_3D();

private:
	HRESULT					Ready_Components(MODELOBJECT_DESC* _pDesc);


public:
	virtual void Free() override;
};
END
