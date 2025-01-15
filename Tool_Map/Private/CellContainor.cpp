#include "stdafx.h"
#include "CellContainor.h"
#include "EditableCell.h"
#include "NavigationVertex.h"
#include "GameInstance.h"

CCellContainor::CCellContainor(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CGameObject(_pDevice,_pContext)

{
}

CCellContainor::CCellContainor(const CCellContainor& _Prototype)
	: CGameObject(_Prototype)
	, m_WorldMatrix{ _Prototype.m_WorldMatrix }
{
}

HRESULT CCellContainor::Initialize_Prototype()
{
	XMStoreFloat4x4(&m_WorldMatrix,XMMatrixIdentity());

	return S_OK;
}

HRESULT CCellContainor::Initialize(void* _pArg)
{

	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../../EngineSDK/hlsl/Shader_Cell.hlsl"), VTXPOS::Elements, VTXPOS::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;

	return S_OK;
}

void CCellContainor::Priority_Update(_float _fTimeDelta)
{
	for (auto iter = m_CellLists.begin(); iter != m_CellLists.end();)
	{
		if ((*iter)->Is_Dead())
			iter = m_CellLists.erase(iter);
		else
		{
			(*iter)->Priority_Update(_fTimeDelta);
			iter++;
		}
	}

	for (auto iter = m_VertexLists.begin(); iter != m_VertexLists.end();)
	{
		if ((*iter)->Is_Dead())
			iter = m_VertexLists.erase(iter);
		else
		{
			(*iter)->Priority_Update(_fTimeDelta);
			iter++;
		}
	}

}

void CCellContainor::Update(_float _fTimeDelta)
{
	for (auto& pVertex : m_VertexLists)
		pVertex->Update(_fTimeDelta);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);
}
_bool	CCellContainor::Is_ContainCell(CNavigationVertex** pPoints)
{
	auto iter = find_if(m_CellLists.begin(), m_CellLists.end(), [&pPoints](CEditableCell* pCell)->_bool {
		return pCell->Equal(pPoints);
		});

	return iter != m_CellLists.end();
}

void CCellContainor::Late_Update(_float _fTimeDelta)
{
}

HRESULT CCellContainor::Render()
{
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;


	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	_float4 vDefaultColor = {};
	_float4 vWireColor = {};
	for (auto& pCell : m_CellLists)
	{
		if (nullptr != pCell)
		{
			vDefaultColor = { 0.f,1.f,0.f,1.f };
			vWireColor = { 1.f,1.f,1.f,1.f };
			//_uint iDefaultPass = 0;
			//
			switch (pCell->Get_State())
			{
			case 0:
				break;
			case 1:
				vWireColor = { 0.f,0.f,1.f,0.f };
				break;
			default:
				break;
			}

			if (pCell->Is_Picking())
				vDefaultColor = vWireColor = { 1.f,0.f,0.f,0.f };

			//
			//
			//_uint iWirePass = 1;
			//if (pCell->Is_Picking())
			//	iDefaultPass = iWirePass = 2;
			if (FAILED(m_pShader->Bind_RawValue("g_vColor", &vDefaultColor, sizeof(_float4))))
				return E_FAIL;
			m_pShader->Begin(0);
			//m_pShader->Begin(iDefaultPass);
			pCell->Render();

			if (m_isWireRender)
			{
				if (FAILED(m_pShader->Bind_RawValue("g_vColor", &vWireColor, sizeof(_float4))))
					return E_FAIL;
				m_pShader->Begin(0);
				//m_pShader->Begin(iWirePass);
				pCell->Wire_Render();
			}
		}
	}

	for (auto& pVertex : m_VertexLists)
	{
		if (nullptr != pVertex)
			pVertex->Render();
	}


	return S_OK;
}


CNavigationVertex* CCellContainor::Picking_On_Vertex(const _float3& _vRayPos, const _float3& _vRayDir)
{
	_float	 fDist = 0.f, fNewDist = 0.f;
	CNavigationVertex* pReturnVertex = nullptr;
	for_each(m_VertexLists.begin(), m_VertexLists.end(), [this, &_vRayPos, &_vRayDir, &fDist, &fNewDist, &pReturnVertex]
	(CNavigationVertex* pVertex)
		{
			bool bRange = pVertex->Check_Picking(XMLoadFloat3(&_vRayPos), XMLoadFloat3(&_vRayDir), &fNewDist);
			if (bRange && (fNewDist < fDist || fDist == 0.f))
			{
				pReturnVertex = pVertex;
				fDist = fNewDist;
			}
		});

	return pReturnVertex;
}



void CCellContainor::All_Delete()
{
	for (auto& pCell : m_CellLists)
		Safe_Release(pCell);

	for (auto& pVertex : m_VertexLists)
		Safe_Release(pVertex);
}

HRESULT CCellContainor::Export(const _wstring& _strFilePath, const _wstring& _strFileName)
{
	_wstring strDefaultFilePath = _strFilePath + _strFileName + L".nchc";
	_wstring strCasheFilePath = _strFilePath + _strFileName + L".nhc";

	HANDLE	hFile = CreateFile(strDefaultFilePath.c_str(),
		GENERIC_WRITE,
		NULL,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	HANDLE	hChsheFile = CreateFile(strCasheFilePath.c_str(),
		GENERIC_WRITE,
		NULL,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);


	if (INVALID_HANDLE_VALUE == hFile || INVALID_HANDLE_VALUE == hChsheFile)
		return E_FAIL;

	_ulong				dwByte = {};


	_uint iVertexCount = (_uint)m_VertexLists.size();
	_uint iCellCount = (_uint)m_CellLists.size();
	WriteFile(hChsheFile, &iVertexCount, sizeof(_uint), &dwByte, nullptr);
	WriteFile(hChsheFile, &iCellCount, sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < (_uint)m_VertexLists.size(); ++i)
	{
		m_VertexLists[i]->Set_Index(i);
		WriteFile(hChsheFile, &m_VertexLists[i]->Get_Pos(), sizeof(_float3), &dwByte, nullptr);
	}

	_uint iIdx = 0;
	_uint iState = 0;
	_float3 vPosition = {};
	for (auto& pCell : m_CellLists)
	{
		iState = pCell->Get_State();
		WriteFile(hChsheFile, &iState, sizeof(_uint), &dwByte, nullptr);
		WriteFile(hFile, &iState, sizeof(_uint), &dwByte, nullptr);
		for (_uint i = 0; i < CEditableCell::POINT_END; ++i)
		{
			auto pPoint = pCell->Get_Point(CEditableCell::POINT(i));
			iIdx = pPoint->Get_Index();
			vPosition = pPoint->Get_Pos();
			WriteFile(hChsheFile, &iIdx, sizeof(_uint), &dwByte, nullptr);
			WriteFile(hFile, &vPosition, sizeof(_float3), &dwByte, nullptr);
		}
	}



	CloseHandle(hFile);
	CloseHandle(hChsheFile);

	return S_OK;
}

HRESULT CCellContainor::Import(const _wstring& _strFilePath, const _wstring& _strFileName)
{
	All_Delete();
	_wstring strCasheFilePath = _strFilePath + _strFileName + L".nhc";


	HANDLE	hFile = CreateFile(strCasheFilePath.c_str(),
		GENERIC_READ,
		NULL,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	_ulong				dwByte = {};


	_uint iVertexCount = 0;
	_uint iCellCount = 0;
	ReadFile(hFile, &iVertexCount, sizeof(_uint), &dwByte, nullptr);
	ReadFile(hFile, &iCellCount, sizeof(_uint), &dwByte, nullptr);

	_float3 vPosition = {};
	CNavigationVertex* pVertex = nullptr;
	for (_uint i = 0; i < iVertexCount; ++i)
	{
		ReadFile(hFile, &vPosition, sizeof(_float3), &dwByte, nullptr);
		pVertex = CNavigationVertex::Create(m_pDevice, m_pContext, vPosition);
		m_VertexLists.push_back(pVertex);
	}

	_uint				iIdx[CEditableCell::POINT_END] = {};
	CNavigationVertex* VertexArr[CEditableCell::POINT_END] = { nullptr, };
	CEditableCell* pCell = nullptr;
	_uint iState = 0;
	for (_uint i = 0; i < iCellCount; ++i)
	{
		ReadFile(hFile, &iState, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, &iIdx, sizeof(_uint) * 3, &dwByte, nullptr);

		for (_uint j = 0; j < CEditableCell::POINT_END; j++)
		{
			VertexArr[j] = m_VertexLists[iIdx[j]];
		}
		pCell = CEditableCell::Create(m_pDevice, m_pContext, VertexArr, (_int)m_CellLists.size());
		for (_uint j = 0; j < CEditableCell::POINT_END; j++)
			VertexArr[j]->Add_Cell(pCell);
		pCell->Set_State(iState);
		m_CellLists.push_back(pCell);
	}

	CloseHandle(hFile);

	return S_OK;
}

CCellContainor* CCellContainor::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CCellContainor* pInstance = new CCellContainor(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCellContainor");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CCellContainor::Clone(void* _pArg)
{
	CCellContainor* pInstance = new CCellContainor(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CCellContainor");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCellContainor::Free()
{
	Safe_Release(m_pShader);
	All_Delete();

	__super::Free();
}

HRESULT CCellContainor::Cleanup_DeadReferences()
{
	return E_NOTIMPL;
}
