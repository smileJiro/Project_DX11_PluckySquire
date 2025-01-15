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
	CCellContainor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCellContainor(const CCellContainor& Prototype);
	virtual ~CCellContainor() = default;

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize(void* pArg) override;
	virtual void				Priority_Update(_float fTimeDelta) override;
	virtual void				Update(_float fTimeDelta) override;
	virtual void				Late_Update(_float fTimeDelta) override;
	virtual HRESULT				Render() override;
	CNavigationVertex*			Picking_On_Vertex(const _float3& vRayPos, const _float3& vRayDir);

	vector<CEditableCell*>&		Get_CellList() { return m_vecCellList; }

	_uint						Get_CellSize() { return (_uint)m_vecCellList.size(); }
	_uint						Get_VertexSize() { return (_uint)m_vecVertexList.size(); }
	void						Add_Cell(CEditableCell* pCell) { m_vecCellList.push_back(pCell); }
	void						Add_Vertex(CNavigationVertex* pVertex) { m_vecVertexList.push_back(pVertex); }
	
	_bool						Is_ContainCell(CNavigationVertex** pPoints);

	_bool						Is_WireRender() { return m_bWireRender; }
	void						Set_WireRender(_bool bWireRender) { m_bWireRender = bWireRender; }

	void						All_Delete();


	HRESULT						Export(const _wstring& strFilePath, const _wstring& strFileName);
	HRESULT						Import(const _wstring& strFilePath, const _wstring& strFileName);

private:
	CShader*						m_pShader = { nullptr };
	vector<CEditableCell*>			m_vecCellList;
	vector<CNavigationVertex*>		m_vecVertexList;
	_float4x4						m_WorldMatrix;
	_bool							m_bWireRender;

public:
	static CCellContainor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END