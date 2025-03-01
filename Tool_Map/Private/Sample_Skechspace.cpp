#include "stdafx.h"
#include "Sample_Skechspace.h"
#include "GameInstance.h"
#include "Controller_Model.h"
#include "3DModel.h"


CSample_Skechspace::CSample_Skechspace(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CModelObject(_pDevice, _pContext)
{
}

CSample_Skechspace::CSample_Skechspace(const CSample_Skechspace& _Prototype)
	: CModelObject(_Prototype)
{
}


HRESULT CSample_Skechspace::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	return S_OK;
}

HRESULT CSample_Skechspace::Initialize(void* _pArg)
{
	SAMPLE_SKSP_DESC* pDesc = static_cast<SAMPLE_SKSP_DESC*>(_pArg);
	pDesc->eStartCoord = COORDINATE_3D;
	pDesc->isCoordChangeEnable = false;
	pDesc->strShaderPrototypeTag_3D = TEXT("Prototype_Component_Shader_VtxMesh");
	pDesc->iShaderPass_3D = (_uint)PASS_VTXMESH::DEFAULT;
	pDesc->tTransform3DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);
	pDesc->tTransform3DDesc.vInitialScaling = _float3(1.0f, 1.0f, 1.0f);
	pDesc->tTransform3DDesc.fRotationPerSec = XMConvertToRadians(180.f);
	pDesc->tTransform3DDesc.fSpeedPerSec = 0.f;
	m_isPreView = pDesc->isPreview;

	__super::Initialize(_pArg);



	// 10. 11
	m_eSkspType = SKSP_DEFAULT;
	if (m_strModelPrototypeTag[COORDINATE_3D] == L"Desk_C04_Sketchspace_Hanging_Flags_01")
		m_eSkspType = SKSP_PLAG;
	if (m_strModelPrototypeTag[COORDINATE_3D] == L"Mug")
		m_eSkspType = SKSP_CUP;
	if (m_strModelPrototypeTag[COORDINATE_3D] == L"Desk_C06_StorageBox_Sketchspace_01")
		m_eSkspType = SKSP_STORAGE;
	if (m_strModelPrototypeTag[COORDINATE_3D] == L"Desk_C06_StorageBox_Sketchspace_02")
	{
		m_eSkspType = SKSP_STORAGE;
		m_isLeft = false;
	}


	Set_Position(XMVectorSet(2.f, 10.f, -17.3f, 1.f));

	return S_OK;
}

void CSample_Skechspace::Priority_Update(_float _fTimeDelta)
{
	__super::Priority_Update(_fTimeDelta);
}

void CSample_Skechspace::Update(_float _fTimeDelta)
{
	__super::Update(_fTimeDelta);
}

void CSample_Skechspace::Late_Update(_float _fTimeDelta)
{
	Register_RenderGroup(RG_3D, PRIORITY_3D::PR3D_GEOMETRY);

	__super::Update(_fTimeDelta);
}

HRESULT CSample_Skechspace::Render()
{


	switch (m_eSkspType)
	{

		break;
	case Map_Tool::SKSP_CUP:
		return Render_Cup();
	case Map_Tool::SKSP_TUB:
		return Render_Tub();
	case Map_Tool::SKSP_PLAG:
		return Render_Plag();
	case Map_Tool::SKSP_STORAGE:
		return Render_Storage();
	case Map_Tool::SKSP_NONE:
	case Map_Tool::SKSP_DEFAULT:
	default:
	{
		if (FAILED(Bind_ShaderResources_WVP()))
			return E_FAIL;
		COORDINATE eCoord = m_pControllerTransform->Get_CurCoord();
		CShader* pShader = m_pShaderComs[eCoord];
		_uint iShaderPass = m_iShaderPasses[eCoord];
		C3DModel* pModel = static_cast<C3DModel*>(m_pControllerModel->Get_Model(Get_CurCoord()));

		for (_uint i = 0; i < (_uint)pModel->Get_Meshes().size(); ++i)
		{
			_uint iShaderPass = (_uint)PASS_VTXMESH::DEFAULT;
			auto pMesh = pModel->Get_Mesh(i);
			_uint iMaterialIndex = pMesh->Get_MaterialIndex();

			if (m_isPreView)
			{

			}
			else
			{
				if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(pShader, "g_AlbedoTexture", TEXT("Target_2D"))))
					return E_FAIL;
			}

			pShader->Begin(iShaderPass);
			pMesh->Bind_BufferDesc();
			pMesh->Render();
		}
	}
		break;
	}


	return S_OK;
}

HRESULT CSample_Skechspace::Render_Plag()
{
	if (FAILED(Bind_ShaderResources_WVP()))
		return E_FAIL;
	COORDINATE eCoord = m_pControllerTransform->Get_CurCoord();
	CShader* pShader = m_pShaderComs[eCoord];
	_uint iShaderPass = m_iShaderPasses[eCoord];
	C3DModel* pModel = static_cast<C3DModel*>(m_pControllerModel->Get_Model(Get_CurCoord()));

	_bool iFlag = false;

	m_pShaderComs[COORDINATE_3D]->Bind_RawValue("g_iFlag", &iFlag, sizeof(_uint));

	for (_uint i = 0; i < (_uint)pModel->Get_Meshes().size(); ++i)
	{
		_uint iShaderPass = (_uint)PASS_VTXMESH::DEFAULT;
		auto pMesh = pModel->Get_Mesh(i);
		_uint iMaterialIndex = pMesh->Get_MaterialIndex();

		if (2 == i || 5 == i)
		{
			_float2 fStartUV = {};
			_float2 fEndUV = {};
			if (2 == i)
			{
				fStartUV = { -0.5f,0.f };
				fEndUV = { 0.f,1.f };
			}
			else if (5 == i)
			{
				fStartUV = { 0.5f,0.f };
				fEndUV = { 1.f,1.f };

			}
			if (FAILED(pShader->Bind_RawValue("g_fStartUV", &fStartUV, sizeof(_float2))))
				return E_FAIL;
			if (FAILED(pShader->Bind_RawValue("g_fEndUV", &fEndUV, sizeof(_float2))))
				return E_FAIL;
			iShaderPass = (_uint)PASS_VTXMESH::RENDERTARGET_MAPP;

			if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(pShader, "g_AlbedoTexture", TEXT("Target_2D"))))
				return E_FAIL;


		}
		else
		{

			if (FAILED(pModel->Bind_Material(pShader, "g_AlbedoTexture", i, aiTextureType_DIFFUSE, 0)))
			{
				int a = 0;
			}

		}

		pShader->Begin(iShaderPass);
		pMesh->Bind_BufferDesc();
		pMesh->Render();
	}
	return S_OK;
}

HRESULT CSample_Skechspace::Render_Cup()
{
	if (FAILED(Bind_ShaderResources_WVP()))
		return E_FAIL;
	COORDINATE eCoord = m_pControllerTransform->Get_CurCoord();
	CShader* pShader = m_pShaderComs[eCoord];
	_uint iShaderPass = m_iShaderPasses[eCoord];
	C3DModel* pModel = static_cast<C3DModel*>(m_pControllerModel->Get_Model(Get_CurCoord()));

	_bool iFlag = false;

	m_pShaderComs[COORDINATE_3D]->Bind_RawValue("g_iFlag", &iFlag, sizeof(_uint));

	for (_uint i = 0; i < (_uint)pModel->Get_Meshes().size(); ++i)
	{
		_uint iShaderPass = (_uint)PASS_VTXMESH::DEFAULT;
		auto pMesh = pModel->Get_Mesh(i);
		_uint iMaterialIndex = pMesh->Get_MaterialIndex();

			if (0 == i)
			{
				if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(pShader, "g_AlbedoTexture", TEXT("Target_2D"))))
					return E_FAIL;
			}
			else
			{

				if (FAILED(pModel->Bind_Material(pShader, "g_AlbedoTexture", i, aiTextureType_DIFFUSE, 0)))
				{
					int a = 0;
				}

			}

			pShader->Begin(iShaderPass);
			pMesh->Bind_BufferDesc();
			pMesh->Render();
	}
	return S_OK;
}

HRESULT CSample_Skechspace::Render_Tub()
{
	if (FAILED(Bind_ShaderResources_WVP()))
		return E_FAIL;
	COORDINATE eCoord = m_pControllerTransform->Get_CurCoord();
	CShader* pShader = m_pShaderComs[eCoord];
	_uint iShaderPass = m_iShaderPasses[eCoord];
	C3DModel* pModel = static_cast<C3DModel*>(m_pControllerModel->Get_Model(Get_CurCoord()));

	_bool iFlag = false;

	m_pShaderComs[COORDINATE_3D]->Bind_RawValue("g_iFlag", &iFlag, sizeof(_uint));

	for (_uint i = 0; i < (_uint)pModel->Get_Meshes().size(); ++i)
	{
		_uint iShaderPass = (_uint)PASS_VTXMESH::DEFAULT;
		auto pMesh = pModel->Get_Mesh(i);
		_uint iMaterialIndex = pMesh->Get_MaterialIndex();

		if (2 == i || 5 == i)
		{
			_float2 fStartUV = {};
			_float2 fEndUV = {};
			if (2 == i)
			{
				fStartUV = { -0.5f,0.f };
				fEndUV = { 0.f,1.f };
			}
			else if (5 == i)
			{
				fStartUV = { 0.5f,0.f };
				fEndUV = { 1.f,1.f };

			}
			if (FAILED(pShader->Bind_RawValue("g_fStartUV", &fStartUV, sizeof(_float2))))
				return E_FAIL;
			if (FAILED(pShader->Bind_RawValue("g_fEndUV", &fEndUV, sizeof(_float2))))
				return E_FAIL;
			iShaderPass = (_uint)PASS_VTXMESH::RENDERTARGET_MAPP;

			if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(pShader, "g_AlbedoTexture", TEXT("Target_2D"))))
				return E_FAIL;


		}
		else
		{

			if (FAILED(pModel->Bind_Material(pShader, "g_AlbedoTexture", i, aiTextureType_DIFFUSE, 0)))
			{
				int a = 0;
			}

		}

		pShader->Begin(iShaderPass);
		pMesh->Bind_BufferDesc();
		pMesh->Render();
	}
	return S_OK;
}

HRESULT CSample_Skechspace::Render_Storage()
{
	if (FAILED(Bind_ShaderResources_WVP()))
		return E_FAIL;
	COORDINATE eCoord = m_pControllerTransform->Get_CurCoord();
	CShader* pShader = m_pShaderComs[eCoord];
	_uint iShaderPass = m_iShaderPasses[eCoord];
	C3DModel* pModel = static_cast<C3DModel*>(m_pControllerModel->Get_Model(Get_CurCoord()));

	_bool iFlag = false;

	m_pShaderComs[COORDINATE_3D]->Bind_RawValue("g_iFlag", &iFlag, sizeof(_uint));

	for (_uint i = 0; i < (_uint)pModel->Get_Meshes().size(); ++i)
	{
		_uint iShaderPass = (_uint)PASS_VTXMESH::DEFAULT;
		auto pMesh = pModel->Get_Mesh(i);
		_uint iMaterialIndex = pMesh->Get_MaterialIndex();

		_float2 fStartUV = {};
		_float2 fEndUV = {};
		if (m_isLeft)
		{
		fStartUV = { 0.f,0.f };
		fEndUV = { 0.5f,1.f };
		}
		else 
		{
			fStartUV = { 0.5f,0.f };
			fEndUV = { 1.f,1.f };
		}
		iShaderPass = (_uint)PASS_VTXMESH::RENDERTARGET_MAPP;

		if (FAILED(pShader->Bind_RawValue("g_fStartUV", &fStartUV, sizeof(_float2))))
			return E_FAIL;
		if (FAILED(pShader->Bind_RawValue("g_fEndUV", &fEndUV, sizeof(_float2))))
			return E_FAIL;
		iShaderPass = (_uint)PASS_VTXMESH::RENDERTARGET_MAPP;

		if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(pShader, "g_AlbedoTexture", TEXT("Target_2D"))))
			return E_FAIL;


		pShader->Begin(iShaderPass);
		pMesh->Bind_BufferDesc();
		pMesh->Render();
	}
	return S_OK;
}


CSample_Skechspace* CSample_Skechspace::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CSample_Skechspace* pInstance = new CSample_Skechspace(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSample_Skechspace");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSample_Skechspace::Clone(void* _pArg)
{
	CSample_Skechspace* pInstance = new CSample_Skechspace(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CSample_Skechspace");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSample_Skechspace::Free()
{

	__super::Free();
}
