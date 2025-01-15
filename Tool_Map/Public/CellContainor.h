#pragma once
#include "GameObject.h"
#include "Map_Tool_Defines.h"

BEGIN(Engine)
class CShader;
END


BEGIN(Map_Tool)

class CNavigationVertex;
class CEditableCell;

class CCellContainor final :
	public CGameObject
{
private:
	CCellContainor(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CCellContainor(const CCellContainor& _Prototype);
	virtual ~CCellContainor() = default;

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize(void* _pArg) override;
	virtual void				Priority_Update(_float _fTimeDelta) override;
	virtual void				Update(_float _fTimeDelta) override;
	virtual void				Late_Update(_float _fTimeDelta) override;
	virtual HRESULT				Render() override;
	CNavigationVertex*			Picking_On_Vertex(const _float3& _vRayPos, const _float3& _vRayDir);

	vector<CEditableCell*>&		Get_CellList() { return m_CellLists; }

	_uint						Get_CellSize() { return (_uint) m_CellLists.size(); }
	_uint						Get_VertexSize() { return (_uint)m_VertexLists.size();
	}
	void						Add_Cell(CEditableCell* _pCell) { m_CellLists.push_back(_pCell); }
	void						Add_Vertex(CNavigationVertex* _pVertex) { m_VertexLists.push_back(_pVertex); }
	
	_bool						Is_ContainCell(CNavigationVertex** _pPoints);

	_bool						Is_WireRender() { return m_isWireRender; }
	void						Set_WireRender(_bool _isWireRender) { m_isWireRender = _isWireRender; }

	void						All_Delete();


	HRESULT						Export(const _wstring& _strFilePath, const _wstring& _strFileName);
	HRESULT						Import(const _wstring& _strFilePath, const _wstring& _strFileName);

private:
	CShader*						m_pShader = { nullptr };
	vector<CEditableCell*>			m_CellLists;
	vector<CNavigationVertex*>		m_VertexLists;
	_float4x4						m_WorldMatrix;
	_bool							m_isWireRender;

public:
	static CCellContainor* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;

	virtual HRESULT Cleanup_DeadReferences() override;
};

END