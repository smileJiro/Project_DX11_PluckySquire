#include "stdafx.h"
#include "NavigationVertex.h"
#include "GameInstance.h"
#include "EditableCell.h"
#include "gizmo/ImGuizmo.h"
CNavigationVertex::CNavigationVertex(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CGameObject(_pDevice,_pContext)
{
}

CNavigationVertex::CNavigationVertex(const CNavigationVertex& _Prototype)
	:CGameObject(_Prototype)
{
}

HRESULT CNavigationVertex::Initialize(const _float3& _vVerTexPos)
{

	CGameObject::GAMEOBJECT_DESC Desc = {};
	Desc.eStartCoord = COORDINATE_3D;
	Desc.iCurLevelID = LEVEL_TOOL_MAP;
	Desc.isCoordChangeEnable = false;
	__super::Initialize(&Desc);

	XMStoreFloat4x4(&m_matGizmoWorld, XMMatrixIdentity());

	XMStoreFloat4(&m_vColorArr[MODE::SET], DirectX::Colors::HotPink);
	XMStoreFloat4(&m_vColorArr[MODE::NORMAL], DirectX::Colors::LimeGreen);
	XMStoreFloat4(&m_vColorArr[MODE::PICKING_VERTEX], DirectX::Colors::Coral);
	XMStoreFloat4(&m_vColorArr[MODE::PICKING_FIRST], DirectX::Colors::Red);
	XMStoreFloat4(&m_vColorArr[MODE::PICKING_SECOND], DirectX::Colors::Green);
	XMStoreFloat4(&m_vColorArr[MODE::PICKING_THIRD], DirectX::Colors::Blue);

	m_vPos = _vVerTexPos;
	memcpy(&m_matGizmoWorld.m[3],&m_vPos,sizeof(_float3));
	CBounding_Sphere::BOUND_SPHERE_DESC ColliderDesc = {};
	//ColliderDesc.vCenterPos = { m_vPos };
	Set_Position(XMLoadFloat3(&m_vPos));
	ColliderDesc.fRadius = 0.5f;

	if (FAILED(Add_Component(LEVEL_TOOL_MAP, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pCollider), &ColliderDesc)))
		return E_FAIL;



	return S_OK;
}

void CNavigationVertex::Priority_Update(_float _fTimeDelta)
{
	for (auto iter = m_vecIncludeCell.begin(); iter != m_vecIncludeCell.end();)
	{
		if ((*iter)->Is_Dead())
			iter = m_vecIncludeCell.erase(iter);
		else
			iter++;
	}
}

void CNavigationVertex::Update(_float _fTimeDelta)
{
	if (m_eMode == MODE::PICKING_VERTEX)
	{
		RECT rect{};
		GetWindowRect(g_hWnd,&rect);
		ImGuizmo::SetRect((_float)rect.left, (_float)rect.top, (_float)g_iWinSizeX, (_float)g_iWinSizeY);

		//AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, TRUE);

		_float4x4 fMatView = m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW);
		_float4x4 fMatProj = m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ);
		ImGuizmo::Manipulate(fMatView.m[0],
			fMatProj.m[0], ImGuizmo::OPERATION::TRANSLATE,
			ImGuizmo::LOCAL,
			m_matGizmoWorld.m[0]);
		ImGuizmo::IsUsing();
		
		if (!XMVector3Equal(XMLoadFloat3((_float3*)m_matGizmoWorld.m[3]), XMLoadFloat3(&m_vPos)))
		{
			Update_Pos();
		}
	}


	if (m_pCollider)
		m_pCollider->Update(Get_WorldMatrix());

}

void CNavigationVertex::Late_Update(_float _fTimeDelta)
{
}

HRESULT CNavigationVertex::Render()
{
	if (m_pCollider)
		m_pCollider->Render();
	return S_OK;
}

bool CNavigationVertex::Check_Picking(_fvector _vRayPos, _fvector _vRayDir, _float* _fNewDist)
{
	_matrix			matWorld;
	_vector			vLocalRayPos = _vRayPos;
	_vector			vLocalRayDir = _vRayDir;
	matWorld = XMMatrixInverse(nullptr, Get_WorldMatrix());

	vLocalRayPos = XMVector3TransformCoord(vLocalRayPos, matWorld);
	vLocalRayDir = XMVector3TransformNormal(vLocalRayDir, matWorld);
	vLocalRayDir = XMVector3Normalize(vLocalRayDir);

	if (m_pCollider)
	{
		return m_pCollider->Intersect_Ray(_vRayPos, _vRayDir, _fNewDist);
	}

	return false;
}

void CNavigationVertex::Update_Pos()
{
	memcpy(&m_vPos, &m_matGizmoWorld.m[3], sizeof(_float3));
	Set_Position(XMLoadFloat3(&m_vPos));
	//m_pCollider->Set_OffsetMatrix(XMLoadFloat4x4(&m_matGizmoWorld));
	//m_pCollider->Set_Center(m_vPos);
	for (auto& pCell : m_vecIncludeCell)
		pCell->Update_Vertex();

}

void CNavigationVertex::Delete_Vertex()
{
	OBJECT_DESTROY(this);
	for (auto& pCell : m_vecIncludeCell)
		OBJECT_DESTROY(pCell);
}


CNavigationVertex* CNavigationVertex::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _float3& _vVerTexPos)
{
	CNavigationVertex* pInstance = new CNavigationVertex(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize(_vVerTexPos)))
	{
		MSG_BOX("Failed to Created : CNavigationVertex");
		Safe_Release(pInstance);
	}

	return pInstance;
}

HRESULT CNavigationVertex::Cleanup_DeadReferences()
{
	return S_OK;
}


void CNavigationVertex::Free()
{
	Safe_Release(m_pCollider);

	__super::Free();
}

CGameObject* CNavigationVertex::Clone(void* pArg)
{
	CNavigationVertex* pInstance = new CNavigationVertex(*this);

	return pInstance;
}
