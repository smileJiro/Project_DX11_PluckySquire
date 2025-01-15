#include "stdafx.h"
#include "CellContainor.h"
#include "EditableCell.h"
#include "NavigationVertex.h"
#include "GameInstance.h"
CCellContainor::CCellContainor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice,pContext)

{
}

CCellContainor::CCellContainor(const CCellContainor& Prototype)
	: CGameObject(Prototype)
	, m_WorldMatrix{ Prototype.m_WorldMatrix }
{
}

HRESULT CCellContainor::Initialize_Prototype()
{
	XMStoreFloat4x4(&m_WorldMatrix,XMMatrixIdentity());

	return S_OK;
}

HRESULT CCellContainor::Initialize(void* pArg)
{

	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../../EngineSDK/hlsl/Shader_Cell.hlsl"), VTXPOS::Elements, VTXPOS::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;

	return S_OK;
}

void CCellContainor::Priority_Update(_float fTimeDelta)
{
	for (auto iter = m_vecCellList.begin(); iter != m_vecCellList.end();)
	{
		if ((*iter)->Is_Dead())
			iter = m_vecCellList.erase(iter);
		else
		{
			(*iter)->Priority_Update(fTimeDelta);
			iter++;
		}
	}

	for (auto iter = m_vecVertexList.begin(); iter != m_vecVertexList.end();)
	{
		if ((*iter)->Is_Dead())
			iter = m_vecVertexList.erase(iter);
		else
		{
			(*iter)->Priority_Update(fTimeDelta);
			iter++;
		}
	}

}

void CCellContainor::Update(_float fTimeDelta)
{
	for (auto& pVertex : m_vecVertexList)
		pVertex->Update(fTimeDelta);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);
}
_bool	CCellContainor::Is_ContainCell(CNavigationVertex** pPoints)
{
	auto iter = find_if(m_vecCellList.begin(), m_vecCellList.end(), [&pPoints](CEditableCell* pCell)->_bool {
		return pCell->Equal(pPoints);
		});

	return iter != m_vecCellList.end();
}

void CCellContainor::Late_Update(_float fTimeDelta)
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
	for (auto& pCell : m_vecCellList)
	{
		if (nullptr != pCell)
		{
			vDefaultColor = { 0.f,1.f,0.f,1.f };
			vWireColor = { 1.f,1.f,1.f,1.f };
			//_uint iDefaultPass = 0;
			//
			switch (pCell->Get_State())
			{
			case 0 :
				break;
			case 1 :
				vWireColor = { 0.f,0.f,1.f,0.f };
				break;
			default:
				break;
			}

			if (pCell->Is_Picking())
				vDefaultColor = vWireColor = {1.f,0.f,0.f,0.f};

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

			if (m_bWireRender)
			{
				if (FAILED(m_pShader->Bind_RawValue("g_vColor", &vWireColor, sizeof(_float4))))
					return E_FAIL;
				m_pShader->Begin(0);
				//m_pShader->Begin(iWirePass);
				pCell->Wire_Render();
			}
		}
	}

	for (auto& pVertex : m_vecVertexList)
	{
		if (nullptr != pVertex)
			pVertex->Render();
	}


	return S_OK;
}


CNavigationVertex* CCellContainor::Picking_On_Vertex(const _float3& vRayPos, const _float3& vRayDir)
{
	_float	 fDist = 0.f, fNewDist = 0.f;
	CNavigationVertex* pReturnVertex = nullptr;
	for_each(m_vecVertexList.begin(), m_vecVertexList.end(), [this, &vRayPos, &vRayDir, &fDist, &fNewDist, &pReturnVertex]
	(CNavigationVertex* pVertex)
		{
			bool bRange = pVertex->Check_Picking(XMLoadFloat3(&vRayPos), XMLoadFloat3(&vRayDir), &fNewDist);
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

	for (auto& pCell : m_vecCellList)
		Safe_Release(pCell);

	for (auto& pVertex : m_vecVertexList)
		Safe_Release(pVertex);
}

HRESULT CCellContainor::Export(const _wstring& strFilePath, const _wstring& strFileName)
{
	_wstring strDefaultFilePath = strFilePath + strFileName + L".nchc";
	_wstring strCasheFilePath = strFilePath + strFileName + L".nhc";

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


	_uint iVertexCount = (_uint)m_vecVertexList.size();
	_uint iCellCount = (_uint)m_vecCellList.size();
	WriteFile(hChsheFile, &iVertexCount, sizeof(_uint), &dwByte, nullptr);
	WriteFile(hChsheFile, &iCellCount, sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0 ; i < (_uint)m_vecVertexList.size(); ++i)
	{
		m_vecVertexList[i]->Set_Index(i);
		WriteFile(hChsheFile, &m_vecVertexList[i]->Get_Pos(), sizeof(_float3), &dwByte, nullptr);
	}
	
	_uint iIdx = 0;
	_uint iState = 0;
	_float3 vPosition = {};
	for (auto& pCell : m_vecCellList)
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

HRESULT CCellContainor::Import(const _wstring& strFilePath, const _wstring& strFileName)
{
	All_Delete();
	_wstring strCasheFilePath = strFilePath + strFileName + L".nhc";


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
		m_vecVertexList.push_back(pVertex);
	}

	_uint				iIdx[CEditableCell::POINT_END] = {};
	CNavigationVertex*	VertexArr[CEditableCell::POINT_END] = {nullptr,};
	CEditableCell*		pCell = nullptr;
	_uint iState = 0;
	for (_uint i = 0; i < iCellCount; ++i)
	{
		ReadFile(hFile, &iState, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, &iIdx, sizeof(_uint) * 3, &dwByte, nullptr);

		for (_uint j = 0; j < CEditableCell::POINT_END; j++)
		{
			VertexArr[j] = m_vecVertexList[iIdx[j]];
		}
		pCell = CEditableCell::Create(m_pDevice, m_pContext, VertexArr, (_int)m_vecCellList.size());
		for (_uint j = 0; j < CEditableCell::POINT_END; j++)
			VertexArr[j]->Add_Cell(pCell);
		pCell->Set_State(iState);
		m_vecCellList.push_back(pCell);
	}

	CloseHandle(hFile);

	return S_OK;
}

CCellContainor* CCellContainor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCellContainor* pInstance = new CCellContainor(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCellContainor");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CCellContainor::Clone(void* pArg)
{
	CCellContainor* pInstance = new CCellContainor(*this);

	if (FAILED(pInstance->Initialize(pArg)))
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
