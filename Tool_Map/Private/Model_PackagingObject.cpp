#include "stdafx.h"
#include "Model_PackagingObject.h"
#include "Controller_Model.h"
#include "3DModel.h"
#include "Material.h"

CModel_PackagingObject::CModel_PackagingObject()
{
}

HRESULT CModel_PackagingObject::Import(const _string _strFIlePath)
{
	std::ifstream inFile(_strFIlePath, std::ios::binary);
	if (!inFile) 
		return E_FAIL;

	inFile.read(reinterpret_cast<char*>(&m_isAnimation), 1);

	if (FAILED(Import_Bone(inFile, -1)))
		return E_FAIL;

	if (FAILED(Import_Meshes(inFile)))
		return E_FAIL;

	if (FAILED(Import_Materials(inFile, _strFIlePath)))
		return E_FAIL;

	if (FAILED(Import_Animations(inFile)))
		return E_FAIL;

	inFile.close();

	return S_OK;
}

HRESULT CModel_PackagingObject::Export(const _string _strFIlePath)
{

	std::ofstream outFile(_strFIlePath, std::ios::binary);
	if (!outFile) 
		return E_FAIL;

	outFile.write(reinterpret_cast<char*>(&m_isAnimation), 1);

	if (FAILED(Export_Bone(outFile, -1)))
		return E_FAIL;

	if (FAILED(Export_Meshes(outFile)))
		return E_FAIL;

	if (FAILED(Export_Materials(outFile, _strFIlePath)))
		return E_FAIL;

	if (FAILED(Export_Animations(outFile)))
		return E_FAIL;


	outFile.close();

	return S_OK;
}

HRESULT CModel_PackagingObject::Import_Bone(ifstream& _inFile, _uint iParentBoneIndex)
{
	BONE tBone = {};

	_inFile.read((char*)&tBone.iNameSize, sizeof(_uint));
	_inFile.read((char*)&tBone.szName, tBone.iNameSize);
	_inFile.read(reinterpret_cast<char*>(&tBone.matTransformation), sizeof(_float4x4));
	_inFile.read(reinterpret_cast<char*>(&tBone.iNumChildren), sizeof(_uint));

	m_Bones.push_back(tBone);
	iParentBoneIndex = (_uint)m_Bones.size() - 1;


	for (_uint i = 0; i < tBone.iNumChildren; ++i)
		Import_Bone(_inFile, iParentBoneIndex);

	return S_OK;
}

HRESULT CModel_PackagingObject::Import_Meshes(ifstream& _inFile)
{
	_inFile.read(reinterpret_cast<char*>(&m_iNumMeshes), sizeof(_uint));
	m_Meshes.resize(m_iNumMeshes);
	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		MESH& tMesh = m_Meshes[i];
		_inFile.read(reinterpret_cast<char*>(&tMesh.iMaterialIndex), sizeof(_uint));
		_inFile.read(reinterpret_cast<char*>(&tMesh.iNameSize), sizeof(_uint));
		_inFile.read(tMesh.szName, tMesh.iNameSize);
		_inFile.read(reinterpret_cast<char*>(&tMesh.iNumVertices), sizeof(_uint));
		_inFile.read(reinterpret_cast<char*>(&tMesh.iNumFaces), sizeof(_uint));


		#pragma region VERTEX
		if (!m_isAnimation)
		{
			#pragma region NONANIM
			tMesh.Vertices.resize(tMesh.iNumVertices);
			for (size_t i = 0; i < tMesh.iNumVertices; i++)
			{
				_inFile.read(reinterpret_cast<char*>(&tMesh.Vertices[i].vPosition), sizeof(_float3));
				_inFile.read(reinterpret_cast<char*>(&tMesh.Vertices[i].vNormal), sizeof(_float3));
				_inFile.read(reinterpret_cast<char*>(&tMesh.Vertices[i].vTexcoord), sizeof(_float2));
				_inFile.read(reinterpret_cast<char*>(&tMesh.Vertices[i].vTangent), sizeof(_float3));
			}
#pragma endregion
		}
		else 
		{
			#pragma region ANIM
			tMesh.Vertices.resize(tMesh.iNumVertices);
			for (size_t i = 0; i < tMesh.iNumVertices; i++)
			{
				_inFile.read(reinterpret_cast<char*>(&tMesh.Vertices[i].vPosition), sizeof(_float3));
				_inFile.read(reinterpret_cast<char*>(&tMesh.Vertices[i].vNormal), sizeof(_float3));
				_inFile.read(reinterpret_cast<char*>(&tMesh.Vertices[i].vTexcoord), sizeof(_float2));
				_inFile.read(reinterpret_cast<char*>(&tMesh.Vertices[i].vTangent), sizeof(_float3));
			}


			_inFile.read(reinterpret_cast<char*>(&tMesh.iNumBones), sizeof(_uint));
			tMesh.BoneInfos.resize(tMesh.iNumBones);
			for (_uint curMeshBoneIdx = 0; curMeshBoneIdx < tMesh.iNumBones; curMeshBoneIdx++)
			{
				ANIMBONE& tAnimBone = tMesh.BoneInfos[i];
				_inFile.read(reinterpret_cast<char*>(&tAnimBone.iNameSize), sizeof(_uint));
				_inFile.read(tAnimBone.szName, tAnimBone.iNameSize);
				_inFile.read(reinterpret_cast<char*>(&tAnimBone.matOffset), sizeof(_float4x4));

				_inFile.read(reinterpret_cast<char*>(&tAnimBone.iNumVertices), sizeof(_uint));
				tAnimBone.BoneInfos.resize(tAnimBone.iNumVertices);
				for (_uint curBoneVertex = 0; curBoneVertex < tAnimBone.iNumVertices; curBoneVertex++)
				{
					_inFile.read(reinterpret_cast<char*>(&tAnimBone.BoneInfos[curBoneVertex].first), sizeof(_uint));
					_inFile.read(reinterpret_cast<char*>(&tAnimBone.BoneInfos[curBoneVertex].second), sizeof(_float));
				}
			}
#pragma endregion
		}
#pragma endregion

		#pragma region INDEX
		tMesh.Indices.resize(tMesh.iNumFaces * 3);
		_uint		iNumIndices = { 0 };
		for (_uint i = 0; i < tMesh.iNumFaces; ++i)
		{
			_inFile.read(reinterpret_cast<char*>(&tMesh.Indices[iNumIndices++]), sizeof(_uint));
			_inFile.read(reinterpret_cast<char*>(&tMesh.Indices[iNumIndices++]), sizeof(_uint));
			_inFile.read(reinterpret_cast<char*>(&tMesh.Indices[iNumIndices++]), sizeof(_uint));
		}
#pragma endregion

	}
	vector<pair<vector<vector<pair<_string,_uint>>>, _uint>> m_Materials;
	return S_OK;
}

HRESULT CModel_PackagingObject::Import_Materials(ifstream& _inFile, const _string _strFIlePath)
{
	_inFile.read(reinterpret_cast<char*>(&m_iNumMaterials), sizeof(_uint));
	//cout << m_iNumMaterials << endl;
	m_Materials.resize(m_iNumMaterials);
	for (_uint i = 0; i < m_iNumMaterials; i++)
	{
		for (_uint texIdx = 1; texIdx < aiTextureType_UNKNOWN; texIdx++)
		{
			
			_inFile.read(reinterpret_cast<char*>(&m_Materials[i].iNumSRVs[texIdx]), sizeof(_uint));
			if (0 >= m_Materials[i].iNumSRVs[texIdx])
				continue;

			vector<pair<_string, _uint>>& SRVNames = m_Materials[i].SRVNames[texIdx];

			SRVNames.resize(m_Materials[i].iNumSRVs[texIdx]);

			for (_uint iSRVIndex = 0; iSRVIndex < m_Materials[i].iNumSRVs[texIdx]; iSRVIndex++)
			{
				_char szTextureName[MAX_PATH];
				_inFile.read((char*)&SRVNames[iSRVIndex].second, sizeof(_uint));
				_inFile.read((char*)&szTextureName, SRVNames[iSRVIndex].second);
				szTextureName[SRVNames[iSRVIndex].second] = '\0';
				SRVNames[iSRVIndex].first = szTextureName;
			}
		}
	}

	return S_OK;
}

HRESULT CModel_PackagingObject::Import_Animations(ifstream& _inFile)
{
	_inFile.read(reinterpret_cast<char*>(&m_iNumAnimations), sizeof(_uint));
	m_Animations.resize(m_iNumAnimations);
	for (_uint i = 0; i < m_iNumAnimations; i++)
	{
		ANIMATION& tAnim = m_Animations[i];

		_inFile.read((char*)&tAnim.iNameSize, sizeof(_uint));
		_inFile.read((char*)&tAnim.szName, tAnim.iNameSize);


		_double dValue = 0.0;
		_inFile.read(reinterpret_cast<char*>(&dValue), sizeof(double));
		tAnim.fDuration = (_float)dValue;
		_inFile.read(reinterpret_cast<char*>(&dValue), sizeof(double));
		tAnim.fTickPerSecond = (_float)dValue;
		//Loop - 1.26 김지완이 추가함
		_inFile.read(reinterpret_cast<char*>(&tAnim.bLoop), sizeof(_bool));
		//SpeedMagnifier- 1.26 김지완이 추가함
		_inFile.read(reinterpret_cast<char*>(&tAnim.fSpeedMagnifier), sizeof(_float));


		_inFile.read(reinterpret_cast<char*>(&tAnim.iNumChannels), sizeof(_uint));

		tAnim.Channels.resize(tAnim.iNumChannels);

		for (size_t iChannelIndx = 0; iChannelIndx < tAnim.iNumChannels; iChannelIndx++)
		{
			CHANNEL& tChannel = tAnim.Channels[iChannelIndx];
			_inFile.read((char*)&tChannel.iBoneNameSize, sizeof(_uint));
			_inFile.read((char*)&tChannel.szBoneName, tChannel.iBoneNameSize);
			_inFile.read(reinterpret_cast<char*>(&tChannel.iNumKeyFrames), sizeof(_uint));
			tChannel.KeyFrames.resize(tChannel.iNumKeyFrames);
			for (_uint iKeyframeIndex = 0; iKeyframeIndex < tChannel.iNumKeyFrames; iKeyframeIndex++)
			{
				_inFile.read(reinterpret_cast<char*>(&tChannel.KeyFrames[iKeyframeIndex]), sizeof(KEYFRAME));
			}

		}
	}

	return S_OK;
}


HRESULT CModel_PackagingObject::Export_Bone(ofstream& _OutFile, _uint iParentBoneIndex)
{
	//BONE tBone = m_Bones[iParentBoneIndex];
	for (auto& tBone : m_Bones)
	{
		_OutFile.write((char*)&tBone.iNameSize, sizeof(_uint));
		_OutFile.write((char*)&tBone.szName, tBone.iNameSize);
		_OutFile.write(reinterpret_cast<char*>(&tBone.matTransformation), sizeof(_float4x4));
		_OutFile.write(reinterpret_cast<char*>(&tBone.iNumChildren), sizeof(_uint));
	}
	//tBone.
	////m_Bones.push_back(tBone);
	//iParentBoneIndex = (_uint)m_Bones.size() - 1;


	//for (_uint i = 0; i < tBone.iNumChildren; ++i)
	//	Export_Bone(_OutFile, iParentBoneIndex);

	return S_OK;
}

HRESULT CModel_PackagingObject::Export_Meshes(ofstream& _OutFile)
{
	_OutFile.write(reinterpret_cast<char*>(&m_iNumMeshes), sizeof(_uint));
	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		MESH& tMesh = m_Meshes[i];
		_OutFile.write(reinterpret_cast<char*>(&tMesh.iMaterialIndex), sizeof(_uint));
		_OutFile.write(reinterpret_cast<char*>(&tMesh.iNameSize), sizeof(_uint));
		_OutFile.write(tMesh.szName, tMesh.iNameSize);
		_OutFile.write(reinterpret_cast<char*>(&tMesh.iNumVertices), sizeof(_uint));
		_OutFile.write(reinterpret_cast<char*>(&tMesh.iNumFaces), sizeof(_uint));


#pragma region VERTEX
		if (!m_isAnimation)
		{
#pragma region NONANIM
			for (size_t i = 0; i < tMesh.iNumVertices; i++)
			{
				_OutFile.write(reinterpret_cast<char*>(&tMesh.Vertices[i].vPosition), sizeof(_float3));
				_OutFile.write(reinterpret_cast<char*>(&tMesh.Vertices[i].vNormal), sizeof(_float3));
				_OutFile.write(reinterpret_cast<char*>(&tMesh.Vertices[i].vTexcoord), sizeof(_float2));
				_OutFile.write(reinterpret_cast<char*>(&tMesh.Vertices[i].vTangent), sizeof(_float3));
			}
#pragma endregion
		}
		else
		{
#pragma region ANIM
			for (_uint i = 0; i < tMesh.iNumVertices; i++)
			{
				_OutFile.write(reinterpret_cast<char*>(&tMesh.Vertices[i].vPosition), sizeof(_float3));
				_OutFile.write(reinterpret_cast<char*>(&tMesh.Vertices[i].vNormal), sizeof(_float3));
				_OutFile.write(reinterpret_cast<char*>(&tMesh.Vertices[i].vTexcoord), sizeof(_float2));
				_OutFile.write(reinterpret_cast<char*>(&tMesh.Vertices[i].vTangent), sizeof(_float3));

			}


			_OutFile.write(reinterpret_cast<char*>(&tMesh.iNumBones), sizeof(_uint));
			tMesh.BoneInfos.resize(tMesh.iNumBones);
			for (_uint curMeshBoneIdx = 0; curMeshBoneIdx < tMesh.iNumBones; curMeshBoneIdx++)
			{
				ANIMBONE& tAnimBone = tMesh.BoneInfos[i];
				_OutFile.write(reinterpret_cast<char*>(&tAnimBone.iNameSize), sizeof(_uint));
				_OutFile.write(tAnimBone.szName, tAnimBone.iNameSize);
				_OutFile.write(reinterpret_cast<char*>(&tAnimBone.matOffset), sizeof(_float4x4));

				_OutFile.write(reinterpret_cast<char*>(&tAnimBone.iNumVertices), sizeof(_uint));
				tAnimBone.BoneInfos.resize(tAnimBone.iNumVertices);
				for (_uint curBoneVertex = 0; curBoneVertex < tAnimBone.iNumVertices; curBoneVertex++)
				{
					_OutFile.write(reinterpret_cast<char*>(&tAnimBone.BoneInfos[curBoneVertex].first), sizeof(_uint));
					_OutFile.write(reinterpret_cast<char*>(&tAnimBone.BoneInfos[curBoneVertex].second), sizeof(_float));
				}
			}
#pragma endregion
		}
#pragma endregion

#pragma region INDEX
		_uint		iNumIndices = { 0 };
		for (_uint i = 0; i < tMesh.iNumFaces; ++i)
		{
			_OutFile.write(reinterpret_cast<char*>(&tMesh.Indices[iNumIndices++]), sizeof(_uint));
			_OutFile.write(reinterpret_cast<char*>(&tMesh.Indices[iNumIndices++]), sizeof(_uint));
			_OutFile.write(reinterpret_cast<char*>(&tMesh.Indices[iNumIndices++]), sizeof(_uint));
		}
#pragma endregion

	}
	vector<pair<vector<vector<pair<_string, _uint>>>, _uint>> m_Materials;
	return S_OK;
}

HRESULT CModel_PackagingObject::Export_Materials(ofstream& _OutFile, const _string _strFIlePath)
{
	_OutFile.write(reinterpret_cast<char*>(&m_iNumMaterials), sizeof(_uint));
	//cout << m_iNumMaterials << endl;
	for (_uint i = 0; i < m_iNumMaterials; i++)
	{
		for (_uint texIdx = 1; texIdx < aiTextureType_UNKNOWN; texIdx++)
		{

			_OutFile.write(reinterpret_cast<char*>(&m_Materials[i].iNumSRVs[texIdx]), sizeof(_uint));
			vector<pair<_string, _uint>>& SRVNames = m_Materials[i].SRVNames[texIdx];

			for (_uint iSRVIndex = 0; iSRVIndex < m_Materials[i].iNumSRVs[texIdx]; iSRVIndex++)
			{
				_OutFile.write((char*)&SRVNames[iSRVIndex].second, sizeof(_uint));
				_OutFile.write((char*)SRVNames[iSRVIndex].first.c_str(), SRVNames[iSRVIndex].second);
			}
		}
	}

	return S_OK;
}

HRESULT CModel_PackagingObject::Export_Animations(ofstream& _OutFile)
{
	_OutFile.write(reinterpret_cast<char*>(&m_iNumAnimations), sizeof(_uint));
	for (_uint i = 0; i < m_iNumAnimations; i++)
	{
		ANIMATION& tAnim = m_Animations[i];

		_OutFile.write((char*)&tAnim.iNameSize, sizeof(_uint));
		_OutFile.write((char*)&tAnim.szName, tAnim.iNameSize);

		_double dValue = 0.0;
		_OutFile.write(reinterpret_cast<char*>(&dValue), sizeof(_double));
		tAnim.fDuration = (_float)dValue;
		_OutFile.write(reinterpret_cast<char*>(&dValue), sizeof(_double));
		tAnim.fTickPerSecond = (_float)dValue;
		//Loop - 1.26 김지완이 추가함
		_OutFile.write(reinterpret_cast<char*>(&tAnim.bLoop), sizeof(_bool));
		//SpeedMagnifier- 1.26 김지완이 추가함
		_OutFile.write(reinterpret_cast<char*>(&tAnim.fSpeedMagnifier), sizeof(_float));

		_OutFile.write(reinterpret_cast<char*>(&tAnim.iNumChannels), sizeof(_uint));

		for (size_t iChannelIndx = 0; iChannelIndx < tAnim.iNumChannels; iChannelIndx++)
		{
			CHANNEL& tChannel = tAnim.Channels[iChannelIndx];
			_OutFile.write((char*)&tChannel.iBoneNameSize, sizeof(_uint));
			_OutFile.write((char*)&tChannel.szBoneName, tChannel.iBoneNameSize);
			_OutFile.write(reinterpret_cast<char*>(&tChannel.iNumKeyFrames), sizeof(_uint));
			for (_uint iKeyframeIndex = 0; iKeyframeIndex < tChannel.iNumKeyFrames; iKeyframeIndex++)
			{
				_OutFile.write(reinterpret_cast<char*>(&tChannel.KeyFrames[iKeyframeIndex]), sizeof(KEYFRAME));
			}

		}
	}

	return S_OK;
}




HRESULT CModel_PackagingObject::Set_Sync_Material(CMapObject* _pTargetObject)
{
	auto pController= _pTargetObject->Get_ModelController();
	
	if (nullptr == pController)
		return E_FAIL;
	CModel* _pModel = pController->Get_Model(COORDINATE_3D);
	if (nullptr == _pModel)
		return E_FAIL;

	C3DModel* pModel = static_cast<C3DModel*>(_pModel);

	auto TargetMaterials = pModel->Get_Materials();

	for (_uint iMaterialIndex = 0; iMaterialIndex < m_iNumMaterials; ++iMaterialIndex)
	{
		if (iMaterialIndex >= TargetMaterials.size())
			break;

		MATERIAL& tSourMaterial = m_Materials[iMaterialIndex];

		for (_uint iTexTypeIndex = 1; iTexTypeIndex < aiTextureType_UNKNOWN; ++iTexTypeIndex)
		{
			tSourMaterial.SRVNames[iTexTypeIndex].clear();
			_uint iTexIndex = 0;
			while (true)
			{
				auto pTextureName = TargetMaterials[iMaterialIndex]->Find_Name((aiTextureType)iTexTypeIndex, iTexIndex);
				if (pTextureName == nullptr)
					break;
				else
				{ 
					_string strTargetTexName = WstringToString(*pTextureName);
					tSourMaterial.SRVNames[iTexTypeIndex].push_back(make_pair(strTargetTexName, (_uint)strTargetTexName.size()));
				}

				iTexIndex++;
			}
			tSourMaterial.iNumSRVs[iTexTypeIndex] = iTexIndex;
		}
	
	}

	//깃허브 코파일럿
	return S_OK;

}



CModel_PackagingObject* CModel_PackagingObject::Create(const _string _strFIlePath)
{
	CModel_PackagingObject* pInstance = new CModel_PackagingObject();

	if (FAILED(pInstance->Import(_strFIlePath)))
	{
		MSG_BOX("CModel_PackagingObject :: Import Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CModel_PackagingObject::Free()
{
	m_Animations.clear();

	m_Bones.clear();

	m_Materials.clear();

	m_Meshes.clear();
}
