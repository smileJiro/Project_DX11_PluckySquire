#pragma once
#include "ModelObject.h"
BEGIN(Client)
class CButterGrump_Body final : public CModelObject
{
public:
	enum MESHINDEX 
	{	MESH_TONGUE = 4, 
		MESH_LEFTEYE,
		MESH_RIGHTEYE,

		MESH_LAST
	};
private:
	CButterGrump_Body(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CButterGrump_Body(const CButterGrump_Body& _Prototype);
	virtual ~CButterGrump_Body() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* _pArg) override;
	virtual HRESULT Render() override;

protected:
	HRESULT					Ready_Components(MODELOBJECT_DESC* _pDesc);

public:
	void Set_HitRenderDesc(MESHINDEX _eMeshIndex, pair<_bool, _float2> _pair) { m_vecHitRender[_eMeshIndex] = _pair; }
	vector<pair<_bool, _float2>> m_vecHitRender; 
public:
	static CButterGrump_Body* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;
};

END