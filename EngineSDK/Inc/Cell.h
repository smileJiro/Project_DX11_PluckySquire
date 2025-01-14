#pragma once
#include "Base.h"

BEGIN(Engine)
class CVIBuffer_Cell;
class ENGINE_DLL CCell : public CBase
{
public:
	enum POINT { POINT_A, POINT_B, POINT_C, POINT_LAST };
	enum LINE { LINE_AB, LINE_BC, LINE_CA , LINE_LAST };
protected:
	CCell(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CCell() = default;

public:
	HRESULT Initialize(const _float3* _pPoints, _int _iIndex, _uint _eCellOption); /* 셀 생성 시, 자기자신을 구성하는 정점 3개와 본인 셀 인덱스를 부여받는다. */

public:
	_bool Is_In(_fvector _vLocalPos, _int& iNeighborIndex, _float3* _pOutLineNormal = nullptr);
	_bool Search_StartCell(_fvector _vLocalPos);
	/* 매개변수로 들어온 정점 두개가 나에게 겹치는 부분이 있는지 검사를 수행하는 함수, 이웃을 찾기 위함. */
	_bool Compare_Points(const _float3& _SourPoint, const _float3& _DestPoint);
	/* 매개변수로 들어온 포지션의 xz 값을 가지고 나의 평면위의 존재하기위한 y 값을 리턴. */
	_float Compute_Height(_fvector _vPos);

public: // Get 
	const _float3& Get_Point(POINT _ePoint) const { return m_vPoints[_ePoint]; }
	_uint Get_CellOption() const { return m_eOption; }
public: // Set
	void Set_NeighborIndex(LINE _eLine, CCell* _pNeighborCell) { m_iNeighborIndices[_eLine] = _pNeighborCell->m_iIndex; }
	void Set_CellOption(_uint _eCellOption) { m_eOption = _eCellOption; }
#ifdef _DEBUG
public:
	HRESULT Render();
protected:
	CVIBuffer_Cell* m_pVIBuffer = nullptr;
#endif
protected:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

protected:
	_int							m_iIndex = -1;
	_uint							m_eOption = 0;
	_float3							m_vPoints[POINT_LAST] = {};
	/* 이웃 인덱스 부여 규칙 : 자기 자신의 이웃한 삼각형이 존재하지 않는다면 : -1 */
	_int							m_iNeighborIndices[LINE_LAST] = { -1, -1, -1 };

public:
	static CCell* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _float3* _pPoints, _int _iIndex, _uint _eCellOption = 0);
	virtual void Free() override;
};
END
