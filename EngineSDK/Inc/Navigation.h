#pragma once
#include "Component.h"

/* 1. Component 구조로 Navigation을 사용하는 Object들이 각각 소유한다. */
/* 2. Navigation이 소유한 Cell 자체는 원본 객체 생성 시 1 회 생성 후 얕은 복사한다. */
/* 3. Navigation은 각각의 오브젝트의 현재 위치를 체크하고, 이동 요청 발생 시, 갈 수있는 길인지 아닌지에 대한 판단을 수행한다. */


BEGIN(Engine)
class CCell;
class CShader;
class ENGINE_DLL CNavigation final :  public CComponent
{
public:
	typedef struct tagNavigationDesc
	{
		_int	iStartIndex = { -1 }; /*  Object의 시작 Cell을 지정해준다. Nav는 Component 기반 */
	}NAVIGATION_DESC;
private:
	CNavigation(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CNavigation(const CNavigation& _Prototype);
	virtual ~CNavigation() = default;
public:
	virtual HRESULT			Initialize_Prototype(const _tchar* _pNavigationDataFile);
	virtual HRESULT			Initialize(void* _pArg);
	virtual void			Update(_fmatrix _vMapWorldmatrix);
public:
	void					Setting_CurCellIndex(_fvector _vOwnerWorldPos);
public:
	/* Navigation을 소유한 Object의 WorldPos를 기반으로 검사를 수행한다. */
	_bool					Is_Move(_fvector _vOwnerWorldPos, _float3* _pOutLineNormal = nullptr);
	_vector					Adjust_CellHeight(_fvector _vPos);

#ifdef _DEBUG

public:
	virtual HRESULT Render();
private:
	CShader*				m_pShader = nullptr;
#endif // _DEBUG

public:
	// Get
	_int Get_CurCellIndex() const { return m_iCurCellIndex; }

	// Set
	void Set_CurCellIndex(_int _iCurCellIndex) { m_iCurCellIndex = _iCurCellIndex; }
private:
	vector<CCell*>			m_Cells;
	static _float4x4		m_WorldMatrix; /* Terrain 혹은 Map 객체에서 매 프레임 업데이트 해 줄 매트릭스, 해당 매트릭스를 모든 Object는 공유받는다. (static) */
	_int					m_iCurCellIndex = -1; /* -1은 초기값이자, 추후 이웃하는 Cell이 없는 경우 -1로 표현한다. */

private:
	HRESULT					SetUp_Neighbor();
	_float					Compute_Height(_int _iCurCellIndex, _fvector _vPos);

public:
	static CNavigation* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _tchar* _pNavigationDataFile);
	virtual CComponent* Clone(void* _pArg) override;
	virtual void Free() override;
};
END
