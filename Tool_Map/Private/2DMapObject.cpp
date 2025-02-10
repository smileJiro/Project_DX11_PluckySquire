#include "stdafx.h"
#include "2DMapObject.h"
#include "Engine_Defines.h"
#include "GameInstance.h"
#include "Collider.h"
//#include "Coll.h"


C2DMapObject::C2DMapObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CModelObject(pDevice, pContext)
{
}

C2DMapObject::C2DMapObject(const C2DMapObject& Prototype)
	: CModelObject(Prototype)
{
}

HRESULT C2DMapObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT C2DMapObject::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	MAPOBJ_2D_DESC* pDesc = static_cast<MAPOBJ_2D_DESC*>(pArg);
	m_fRenderTargetSize = pDesc->fRenderTargetSize;

	_float2 fRatio = { m_fRenderTargetSize.x / DEFAULT_SIZE_BOOK2D_X, m_fRenderTargetSize.y / DEFAULT_SIZE_BOOK2D_Y };
	
	m_strKey = pDesc->strProtoTag;
	
	
	m_fDefaultPosition = pDesc->fDefaultPosition;
	if (pDesc->isLoad)
	{
		m_fDefaultPosition.x *= fRatio.x;
		m_fDefaultPosition.y *= -fRatio.y;
	}




	if (nullptr == pDesc->pInfo)
		m_strModelName = L"None_Model";
	else if (pDesc->isLoad)
		m_strModelName = m_strKey = StringToWstring(pDesc->pInfo->Get_ModelName());
	else
		m_strModelName = StringToWstring(pDesc->pInfo->Get_ModelName());

	pDesc->Build_2D_Model(
		LEVEL_TOOL_2D_MAP,
		m_strModelName,
		L"Prototype_Component_Shader_VtxPosTex");

	pDesc->Build_2D_Transform(m_fDefaultPosition);

	

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	XMStoreFloat4x4(&m_ProjMatrix,
		XMMatrixOrthographicLH((_float)m_fRenderTargetSize.x,
			m_fRenderTargetSize.y, 0.0f, 1.0f));

	return S_OK;
}

void C2DMapObject::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void C2DMapObject::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void C2DMapObject::Late_Update(_float fTimeDelta)
{
	Register_RenderGroup(RG_2D, PR2D_BOOK_SECTION);
	__super::Late_Update(fTimeDelta);
}

HRESULT C2DMapObject::Render()
{
	HRESULT hr = __super::Render();
#ifdef _DEBUG
	if (m_pColliderCom)
		m_pColliderCom->Render();
#endif // _DEBUG

	return hr;
}

void C2DMapObject::Set_OffsetPos(_float2 _fPos)
{
	m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fDefaultPosition.x + (_fPos.x), m_fDefaultPosition.y + (-_fPos.y), 0.f, 1.f));
}

_bool C2DMapObject::IsCursor_In(_float2 _fCursorPos)
{
	_float2 fConvertRenderTargetSize = {};
	fConvertRenderTargetSize.x = g_iWinSizeX;
	fConvertRenderTargetSize.y = m_fRenderTargetSize.y * ((_float)g_iWinSizeX / (_float)m_fRenderTargetSize.x);

	_float fOffX = (g_iWinSizeX - fConvertRenderTargetSize.x) * 0.5f;
	_float fOffY = (g_iWinSizeY - fConvertRenderTargetSize.y) * 0.5f;

	_float fRelativeX = (_fCursorPos.x - fOffX) / fConvertRenderTargetSize.x;
	_float fRelativeY = (_fCursorPos.y - fOffY) / fConvertRenderTargetSize.y;

	_float2 fRealPos = { fRelativeX * m_fRenderTargetSize.x , fRelativeY * m_fRenderTargetSize.y };
	//fRealPos.x -= m_fRenderTargetSize.x * 0.5f;
	//fRealPos.y = -fRealPos.y + m_fRenderTargetSize.x * 0.5f;
	//_vector fPosition = Get_FinalPosition();
	//_float fPosX = XMVectorGetX(fPosition) + (m_fRenderTargetSize.x * 0.5f);
	//_float fPosY = ((-XMVectorGetY(fPosition)) + (m_fRenderTargetSize.y * 0.5f));
	//
	
	
	CModel* pModel = Get_Model(COORDINATE_2D);
	if (nullptr == pModel)
		return false;
	C2DModel* p2DModel = static_cast<C2DModel*>(pModel);

	_float4x4 matWorld = {};
	
	_matrix matResult = *p2DModel->Get_CurrentSpriteTransform();

	_matrix matRatioScalling = XMMatrixScaling((_float)RATIO_BOOK2D_X, (_float)RATIO_BOOK2D_Y, 1.f);

	matResult = matResult * matRatioScalling * XMLoadFloat4x4(&m_WorldMatrices[COORDINATE_2D]);

	XMStoreFloat4x4(&matWorld, matResult);

	_float fPosX = matWorld.m[3][0] + m_fRenderTargetSize.x * 0.5f;
	_float fPosY = -matWorld.m[3][1] + m_fRenderTargetSize.y * 0.5f;

	_float2 fScale = _float2(XMVectorGetX(XMVector3Length(XMLoadFloat3((_float3*)&matWorld.m[0]))),
		XMVectorGetX(XMVector3Length(XMLoadFloat3((_float3*)&matWorld.m[1]))));


	//_float3 fScale = Get_FinalScale(); // 태웅 : 함수 이름이 바껴서 수정했음. 혹시나 문제가있다면 이걸수정하시오 트랜스폼컨트롤러 get_scale 로
	//fScale.x *= RATIO_BOOK2D_X;
	//fScale.y *= RATIO_BOOK2D_Y;
;	_float fLeft = fPosX - fScale.x / 2.f;
	_float fRight = fPosX + fScale.x / 2.f;
	_float fTop = fPosY - fScale.y / 2.f;
	_float fBottom = fPosY + fScale.y / 2.f;
	if (ContainWstring(m_strKey, L"rock"))
	{
		int a = 1;
	}
	return (fRealPos.x >= fLeft && fRealPos.x <= fRight &&
		fRealPos.y >= fTop && fRealPos.y <= fBottom);
}

HRESULT C2DMapObject::Export(HANDLE hFile)
{

	_vector vPos = Get_FinalPosition();

	DWORD		dwByte(0);
	_uint		iModelIndex = nullptr != m_pModelInfo ? m_pModelInfo->Get_ModelIndex() : 0;
	_float2		fPos = { XMVectorGetX(vPos), XMVectorGetY(vPos) };
	_bool		isOverride = false;

	WriteFile(hFile, &iModelIndex, sizeof(_uint), &dwByte, nullptr);
	WriteFile(hFile, &fPos, sizeof(_float2), &dwByte, nullptr);
	WriteFile(hFile, &isOverride, sizeof(_bool), &dwByte, nullptr);

	return S_OK;
}

HRESULT C2DMapObject::Import(HANDLE hFile, vector<C2DMapObjectInfo*>& _ModelInfos)
{
	MAPOBJ_2D_DESC Desc = {};

	m_isModelLoad = true;

	DWORD		dwByte(0);
	_uint		iModelIndex = 0;
	_float2		fPos = { };
	_bool		isOverride = false;
	ReadFile(hFile, &iModelIndex, sizeof(_uint), &dwByte, nullptr);
	ReadFile(hFile, &fPos, sizeof(_float2), &dwByte, nullptr);
	ReadFile(hFile, &isOverride, sizeof(_bool), &dwByte, nullptr);

	m_fDefaultPosition = fPos;
	m_fRenderTargetSize = { (_float)RTSIZE_BOOK2D_X ,(_float)RTSIZE_BOOK2D_Y };

	m_pModelInfo = _ModelInfos[iModelIndex];
	m_strModelName = StringToWstring(_ModelInfos[iModelIndex]->Get_ModelName());
	Desc.Build_2D_Model(
	LEVEL_TOOL_2D_MAP,
		StringToWstring(m_pModelInfo->Get_ModelName()),
		L"Prototype_Component_Shader_VtxPosTex");

	Desc.Build_2D_Transform(m_fDefaultPosition);


	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	XMStoreFloat4x4(&m_ProjMatrix, 
		XMMatrixOrthographicLH((_float)m_fRenderTargetSize.x,
			m_fRenderTargetSize.y, 0.0f, 1.0f));




	if (m_pModelInfo->Is_Collider())
	{

		_float2 fPos = m_pModelInfo->Get_Collider_Offset_Pos();

		fPos.x *= RATIO_BOOK2D_X;
		fPos.y *= RATIO_BOOK2D_Y;
		fPos.y *= -1.f;


		CModel* pModel = m_pControllerModel->Get_Model(COORDINATE_2D);

		C2DModel* p2DModel = static_cast<C2DModel*>(pModel);
		const _matrix* matLocal = p2DModel->Get_CurrentSpriteTransform();

		if (nullptr != matLocal)
		{
			_float2 fSibalOffset;
			fSibalOffset.x = (*matLocal).r[3].m128_f32[0] * RATIO_BOOK2D_X;
			fSibalOffset.y = (*matLocal).r[3].m128_f32[1] * RATIO_BOOK2D_Y;

			//_matrix matPos = XMMatrixTranslation(fPos.x, fPos.y, 1.f);
			//matPos *= *matLocal;

			fPos.x += fSibalOffset.x;
			fPos.y += fSibalOffset.y;
		}

		if (CCollider::AABB_2D == m_pModelInfo->Get_ColliderType())
		{
			_float2 fExtent = m_pModelInfo->Get_Collider_Extent();
			fExtent.x *= RATIO_BOOK2D_X;
			fExtent.y *= RATIO_BOOK2D_Y;
			fExtent.x *= 0.5f;
			fExtent.y *= 0.5f;

			CCollider_AABB::COLLIDER_AABB_DESC AABBDesc = {};
			AABBDesc.pOwner = this;
			AABBDesc.vExtents = fExtent;
			AABBDesc.vScale = { 1.0f, 1.0f };
			AABBDesc.vOffsetPosition = fPos;
			AABBDesc.isBlock = !m_isActive;
			if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
				TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &AABBDesc)))
				return E_FAIL;
		}
		else if (CCollider::CIRCLE_2D == m_pModelInfo->Get_ColliderType())
		{
			_float fRadius = m_pModelInfo->Get_Collider_Radius();
			fRadius *= RATIO_BOOK2D_X;

			/* Test 2D Collider */
			CCollider_Circle::COLLIDER_CIRCLE_DESC CircleDesc = {};
			CircleDesc.pOwner = this;
			CircleDesc.fRadius = fRadius;
			CircleDesc.vScale = { 1.0f, 1.0f };
			CircleDesc.vOffsetPosition = fPos;
			CircleDesc.isBlock = !m_isActive;
			if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Circle"),
				TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &CircleDesc)))
				return E_FAIL;
		}
	}




	return S_OK;
}

_vector C2DMapObject::Get_FinalPosition() const
{
	_vector vPos = __super::Get_FinalPosition();
	return vPos;
}

HRESULT C2DMapObject::Update_Model_Index()
{
	if (nullptr == m_pModelInfo)
		return S_OK;


	if (m_pModelInfo->Is_Delete())
	{
		m_pModelInfo = nullptr;
		m_isModelLoad = false;
	}


	return S_OK;
}


C2DMapObject* C2DMapObject::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	C2DMapObject* pInstance = new C2DMapObject(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		//MSG_BOX("Failed to Created : C2DMapObject");
		Safe_Release(pInstance);
	}

	return pInstance;
}

C2DMapObject* C2DMapObject::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HANDLE _hFile, vector<C2DMapObjectInfo*>& _ModelInfos)
{
	C2DMapObject* pInstance = new C2DMapObject(pDevice, pContext);

	if (FAILED(pInstance->Import(_hFile, _ModelInfos)))
	{
		MSG_BOX("Failed to Load : C2DMapObject");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* C2DMapObject::Clone(void* pArg)
{
	C2DMapObject* pInstance = new C2DMapObject(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		//MSG_BOX("Failed to Cloned : C2DMapObject");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void C2DMapObject::Free()
{
	__super::Free();

}

HRESULT C2DMapObject::Cleanup_DeadReferences()
{
	return S_OK;
}
