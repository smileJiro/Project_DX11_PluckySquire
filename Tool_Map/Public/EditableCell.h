#pragma once

#include "GameObject.h"
#include "Map_Tool_Defines.h"

BEGIN(Engine)

class CVIBuffer_Cell;

END


BEGIN(Map_Tool)

class CVIBuffer_CellLine;

class CNavigationVertex;

class CEditableCell final : public CGameObject
{
public:
	enum POINT { POINT_A, POINT_B, POINT_C, POINT_END };
	enum LINE { LINE_AB, LINE_BC, LINE_CA, LINE_END };
private:
	CEditableCell(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CEditableCell(const CEditableCell& _Prototype);
	virtual ~CEditableCell() = default;

public:
	CNavigationVertex* Get_Point(POINT _ePoint) const {
		return m_vPoints[_ePoint];
	}

	void Set_Neighbor(LINE _eLine, CEditableCell* _pNeighborCell) {
		m_iNeighborIndices[_eLine] = _pNeighborCell->m_iIndex;
	}

public:
	HRESULT						Initialize(CNavigationVertex** _pPoints, _int _iIndex);
	virtual void				Update(_float _fTimeDelta);

public :


	void						Update_Vertex();
	virtual _int				Get_Index() { return m_iIndex; }

	_bool						Is_Picking() { return m_isPicking; }
	void						Set_Picking(_bool _isPicking) { m_isPicking = _isPicking; }
	HRESULT						Create_Buffer();


	_bool						Equal(CNavigationVertex** _pPoints);

	void						Set_State(_uint _iState) { m_iCellState = _iState; }
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
	_bool							m_isPicking = false;
#ifdef _DEBUG
private:
	class CVIBuffer_Cell*			m_pVIBuffer = { nullptr };
	class CVIBuffer_CellLine*		m_pVIStrip = { nullptr };
#endif

public:
	static CEditableCell*	Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, CNavigationVertex** _pPoints, _int _iIndex);
	virtual CGameObject*	Clone(void* _pArg) override;
	virtual void			Free() override;
	virtual HRESULT			Cleanup_DeadReferences() override;
};

END