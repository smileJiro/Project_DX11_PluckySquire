#pragma once

#include "GameObject.h"
#include "Map_Tool_Defines.h"

BEGIN(Engine)

class CVIBuffer_Cell;
class CVIBuffer_CellLine;

END


BEGIN(Map_Tool)


class CNavigationVertex;

class CEditableCell final : public CGameObject
{
public:
	enum POINT { POINT_A, POINT_B, POINT_C, POINT_END };
	enum LINE { LINE_AB, LINE_BC, LINE_CA, LINE_END };
private:
	CEditableCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEditableCell(const CEditableCell& Prototype);
	virtual ~CEditableCell() = default;

public:
	CNavigationVertex* Get_Point(POINT ePoint) const {
		return m_vPoints[ePoint];
	}

	void Set_Neighbor(LINE eLine, CEditableCell* pNeighborCell) {
		m_iNeighborIndices[eLine] = pNeighborCell->m_iIndex;
	}

public:
	HRESULT						Initialize(CNavigationVertex** pPoints, _int iIndex);
	virtual void				Update(_float fTimeDelta);

public :


	void						Update_Vertex();
	virtual _int				Get_Index() { return m_iIndex; }

	_bool						Is_Picking() { return m_bPicking; }
	void						Set_Picking(_bool bPicking) { m_bPicking = bPicking; }
	HRESULT						Create_Buffer();


	_bool						Equal(CNavigationVertex** pPoints);

	void						Set_State(_uint iState) { m_iCellState = iState; }
	_uint						Get_State() { return m_iCellState; }



#ifdef _DEBUG
public:
	HRESULT						Render();
	HRESULT						Wire_Render();
#endif


private:
	_uint							m_iCellState = { 0 };
	_uint							m_iIndex = { 0 };
	CNavigationVertex*				m_vPoints[POINT_END] = {};
	_int							m_iNeighborIndices[LINE_END] = { -1, -1, -1 };
	_bool							m_bPicking = false;
#ifdef _DEBUG
private:
	class CVIBuffer_Cell* m_pVIBuffer = { nullptr };
	class CVIBuffer_CellLine* m_pVIStrip = { nullptr };
#endif

public:
	static CEditableCell* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CNavigationVertex** pPoints, _int iIndex);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END