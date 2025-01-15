#include "stdafx.h"
#include "EditableCell.h"
#include "VIBuffer_Cell.h"
#include "VIBuffer_CellLine.h"
#include "NavigationVertex.h"
#include "GameInstance.h"

CEditableCell::CEditableCell(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CGameObject(_pDevice,_pContext)
{
}

CEditableCell::CEditableCell(const CEditableCell& _Prototype)
	: CGameObject(_Prototype)
{
}
HRESULT CEditableCell::Initialize(CNavigationVertex** _pPoints, _int _iIndex)
{
	for (_uint i = 0; i < 3; i++)
	{
		if (nullptr != _pPoints[i])
			m_vPoints[i] = _pPoints[i];
	}

	m_iIndex = _iIndex;

#ifdef _DEBUG
	Create_Buffer();
#endif

	return S_OK;
}
void CEditableCell::Update(_float _fTimeDelta)
{
	__super::Update(_fTimeDelta);
}
void CEditableCell::Update_Vertex()
{
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pVIStrip);
	Create_Buffer();

}
HRESULT CEditableCell::Create_Buffer()
{
	Safe_Release(m_pVIBuffer);



	_float3 fVertexArr[] = {
		m_vPoints[0]->Get_Pos(),
		m_vPoints[1]->Get_Pos(),
		m_vPoints[2]->Get_Pos(),
	};
	m_pVIBuffer = CVIBuffer_Cell::Create(m_pDevice, m_pContext, fVertexArr);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;


	// 점 1당, 5개니까, 18개.
	// 점 1당, 3개면, 12개.
	_float3 fVertexArr2[18] = {
	};

	_float fLef = 0.2f;
	for (_uint i = 0; i < LINE_END; i++)
	{
		fLef = 0.1f;
		fVertexArr2[i * 3 + i] = fVertexArr[i];
		for (_uint j = 0; j < 3; j++)
		{

			XMStoreFloat3(&fVertexArr2[i * 3 + i + 1 + j],
				XMVectorLerp(XMLoadFloat3(&fVertexArr[(i + 1) % LINE_END]), XMLoadFloat3(&fVertexArr[(i + 2) % LINE_END]), fLef)
			);
			fLef += 0.3f;
		}
	}


	m_pVIStrip = CVIBuffer_CellLine::Create(m_pDevice, m_pContext, fVertexArr2);
	if (nullptr == m_pVIStrip)
		return E_FAIL;
	return S_OK;

}


_bool	CEditableCell::Equal(CNavigationVertex** _pPoints)
{
	_uint iboolCount = 0;
	for (auto pVertex : m_vPoints)
	{
		for (_uint i = 0; i < POINT_END; i++)
		{
			if(pVertex == _pPoints[i])
			iboolCount++;
		}
	}
	return iboolCount >= POINT_END;
}


#ifdef _DEBUG

HRESULT CEditableCell::Render()
{
	m_pVIBuffer->Bind_BufferDesc();

	m_pVIBuffer->Render();


	return S_OK;
}
HRESULT CEditableCell::Wire_Render()
{
	m_pVIStrip->Bind_BufferDesc();

	m_pVIStrip->Render();

	return S_OK;
}
#endif

CEditableCell* CEditableCell::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, CNavigationVertex** _pPoints, _int _iIndex)
{
	CEditableCell* pInstance = new CEditableCell(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize(_pPoints, _iIndex)))
	{
		MSG_BOX("Failed to Created : CEditableCell");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CEditableCell::Clone(void* _pArg)
{
	CEditableCell* pInstance = new CEditableCell(*this);

	return pInstance;
}

void CEditableCell::Free()
{
	__super::Free();

#ifdef _DEBUG
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pVIStrip);
#endif

	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}

HRESULT CEditableCell::Cleanup_DeadReferences()
{
	return S_OK;
}
