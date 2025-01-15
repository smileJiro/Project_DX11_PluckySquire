#include "stdafx.h"
#include "NavigationVertex.h"
#include "GameInstance.h"
#include "EditableCell.h"
#include "imgui/gizmo/ImGuizmo.h"
CNavigationVertex::CNavigationVertex(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject(pDevice,pContext)
{
}

CNavigationVertex::CNavigationVertex(const CNavigationVertex& Prototype)
	:CGameObject(Prototype)
{
}

HRESULT CNavigationVertex::Initialize(const _float3& vVerTexPos)
{
	XMStoreFloat4x4(&m_matGizmoWorld, XMMatrixIdentity());

	XMStoreFloat4(&m_vColorArr[MODE::SET],DirectX::Colors::HotPink);
	XMStoreFloat4(&m_vColorArr[MODE::NORMAL],DirectX::Colors::LimeGreen);
	XMStoreFloat4(&m_vColorArr[MODE::PICKING_VERTEX],DirectX::Colors::Coral);
	XMStoreFloat4(&m_vColorArr[MODE::PICKING_FIRST],DirectX::Colors::Red);
	XMStoreFloat4(&m_vColorArr[MODE::PICKING_SECOND],DirectX::Colors::Green);
	XMStoreFloat4(&m_vColorArr[MODE::PICKING_THIRD],DirectX::Colors::Blue);

	m_vPos = vVerTexPos;
	memcpy(&m_matGizmoWorld.m[3],&m_vPos,sizeof(_float3));
	CBounding_Sphere::BOUND_SPHERE_DESC ColliderDesc = {};
	ColliderDesc.vCenter = { m_vPos };
	ColliderDesc.fRadius = 0.5f;

	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pCollider), &ColliderDesc)))
		return E_FAIL;

	CGameObject::GAMEOBJECT_DESC Desc = {};

	__super::Initialize(&Desc);

	return S_OK;
}

void CNavigationVertex::Priority_Update(_float fTimeDelta)
{
	for (auto iter = m_vecIncludeCell.begin(); iter != m_vecIncludeCell.end();)
	{
		if ((*iter)->Is_Dead())
			iter = m_vecIncludeCell.erase(iter);
		else
			iter++;
	}
}

void CNavigationVertex::Update(_float fTimeDelta)
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
		m_pCollider->Update(m_pTransformCom->Get_WorldMatrix());

}

void CNavigationVertex::Late_Update(_float fTimeDelta)
{
}

HRESULT CNavigationVertex::Render()
{
	if (m_pCollider)
		m_pCollider->Render(XMLoadFloat4(&m_vColorArr[m_eMode]));
	return S_OK;
}

bool CNavigationVertex::Check_Picking(_fvector vRayPos, _fvector vRayDir, _float* fNewDist)
{
	_matrix			matWorld;
	_vector			vLocalRayPos = vRayPos;
	_vector			vLocalRayDir = vRayDir;
	matWorld = m_pTransformCom->Get_WorldMatrix_Inverse();

	vLocalRayPos = XMVector3TransformCoord(vLocalRayPos, matWorld);
	vLocalRayDir = XMVector3TransformNormal(vLocalRayDir, matWorld);
	vLocalRayDir = XMVector3Normalize(vLocalRayDir);

	if (m_pCollider)
	{
		return m_pCollider->Intersect(vRayPos, vRayDir, fNewDist);
	}


	return false;
}

void CNavigationVertex::Update_Pos()
{
	memcpy(&m_vPos, &m_matGizmoWorld.m[3], sizeof(_float3));
	m_pCollider->Set_Center(m_vPos);
	for (auto& pCell : m_vecIncludeCell)
		pCell->Update_Vertex();

}

void CNavigationVertex::Delete_Vertex()
{
	m_pGameInstance->Promise_Event(CEvent::Generator(CEvent::OBJECT_DISTROY)->Push_Object((CGameObject*)this));
	for (auto& pCell : m_vecIncludeCell)
		m_pGameInstance->Promise_Event(CEvent::Generator(CEvent::OBJECT_DISTROY)->Push_Object((CGameObject*)pCell));
}


CNavigationVertex* CNavigationVertex::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3& vVerTexPos)
{
	CNavigationVertex* pInstance = new CNavigationVertex(pDevice, pContext);

	if (FAILED(pInstance->Initialize(vVerTexPos)))
	{
		MSG_BOX("Failed to Created : CNavigationVertex");
		Safe_Release(pInstance);
	}

	return pInstance;
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
