#include "pch.h"

#include "FBXToBinary.h"

CFbxToBinary::CFbxToBinary()
{
}

CFbxToBinary::~CFbxToBinary()
{
}



HRESULT CFbxToBinary::FbxToBinary(const string& inFilePath)
{
	_uint		iFlag = { aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast };


	m_pAIScene = m_Importer.ReadFile(inFilePath, iFlag);

	m_bAnim = m_pAIScene->HasAnimations();
	
	if (m_bAnim == false)
	{
	 
		iFlag |= aiProcess_PreTransformVertices;

		m_pAIScene = m_Importer.ReadFile(inFilePath, iFlag);
	}

	//Mimic 모델의 경우 무조건 m_bAnim을 true로 할 것.
	//m_bAnim = true;

	if (0 == m_pAIScene)
		return E_FAIL;

	_char		szDrive[MAX_PATH] = "";
	_char		szDirectory[MAX_PATH] = "";
	_char		szFileName[MAX_PATH] = "";
	_splitpath_s(inFilePath.c_str(), szDrive, MAX_PATH, szDirectory, MAX_PATH, szFileName, MAX_PATH, nullptr, 0);
	_char		szNewFileName[MAX_PATH] = "";
	strcpy_s(szNewFileName, szDrive);
	strcat_s(szNewFileName, szDirectory);
	strcat_s(szNewFileName, szFileName);
	strcat_s(szNewFileName, ".model");
	std::ofstream outFile(szNewFileName, std::ios::binary);
	if (!outFile) {
		std::cerr << "파일을 열 수 없습니다." << std::endl;
	}
	outFile.write(reinterpret_cast<const char*>(&m_bAnim), 1);

	cout << "---------Bones-------------------" << endl;
	if (FAILED(Write_Bone(m_pAIScene->mRootNode, outFile)))
		return E_FAIL;



	cout << "---------Meshes-------------------" << endl;
	outFile.write(reinterpret_cast<const char*>(&m_pAIScene->mNumMeshes), sizeof(_uint));
	//cout << m_pAIScene->mNumMeshes << endl;
	for (size_t meshIdx = 0; meshIdx < m_pAIScene->mNumMeshes; meshIdx++)
	{
		if (FAILED(Write_Meshe(m_pAIScene->mMeshes[meshIdx], outFile)))
			return E_FAIL;

	}

	cout << "---------Materials-------------------" << endl;
	_uint iNumMaterials = m_pAIScene->mNumMaterials;
	outFile.write(reinterpret_cast<const char*>(&iNumMaterials), sizeof(_uint));
	//cout << iNumMaterials << endl;
	auto textures = m_pAIScene->mTextures;
	for (size_t i = 0; i < iNumMaterials; i++)
	{
		if (FAILED(Write_Material(m_pAIScene->mMaterials[i], outFile)))
			return E_FAIL;
	}

	cout << "---------Animations-------------------" << endl;
	outFile.write(reinterpret_cast<const char*>(&m_pAIScene->mNumAnimations), sizeof(_uint));
	//cout << m_pAIScene->mNumAnimations << endl;
	for (size_t i = 0; i < m_pAIScene->mNumAnimations; i++)
	{
		if (FAILED(Write_Animation(m_pAIScene->mAnimations[i], outFile)))
			return E_FAIL;
	}

	outFile.close();
	return S_OK;
}

HRESULT CFbxToBinary::Write_Meshe(const aiMesh* pAIMesh, ofstream& outFile)
{
	outFile.write(reinterpret_cast<const char*>(&pAIMesh->mMaterialIndex), sizeof(_uint));
	//cout << pAIMesh->mMaterialIndex << endl;
	outFile.write(reinterpret_cast<const char*>(&pAIMesh->mName.length), sizeof(_uint));
	//cout << pAIMesh->mName.length << endl;
	outFile.write(pAIMesh->mName.data, pAIMesh->mName.length);
	//cout << pAIMesh->mName.data << endl;
	outFile.write(reinterpret_cast<const char*>(&pAIMesh->mNumVertices), sizeof(_uint));
	//cout << pAIMesh->mNumVertices << endl;
	outFile.write(reinterpret_cast<const char*>(&pAIMesh->mNumFaces), sizeof(_uint));
	//cout << pAIMesh->mNumFaces << endl;
	for (size_t i = 0; i < pAIMesh->mNumVertices; i++)
	{
		outFile.write(reinterpret_cast<const char*>(&pAIMesh->mVertices[i]), sizeof(_float3));
		//cout << pAIMesh->mVertices[i].x << " " << pAIMesh->mVertices[i].y << " " << pAIMesh->mVertices[i].z << endl;
		outFile.write(reinterpret_cast<const char*>(&pAIMesh->mNormals[i]), sizeof(_float3));
		//cout << pAIMesh->mNormals[i].x << " " << pAIMesh->mNormals[i].y << " " << pAIMesh->mNormals[i].z << endl;
		outFile.write(reinterpret_cast<const char*>(&pAIMesh->mTextureCoords[0][i]), sizeof(_float2));		//cout << pAIMesh->mTextureCoords[0][i].x << " " << pAIMesh->mTextureCoords[0][i].y << endl;
		outFile.write(reinterpret_cast<const char*>(&pAIMesh->mTangents[i]), sizeof(_float3));
		//cout << pAIMesh->mTangents[i].x << " " << pAIMesh->mTangents[i].y << " " << pAIMesh->mTangents[i].z << endl;
	}
	if (m_bAnim)
	{
		outFile.write(reinterpret_cast<const char*>(&pAIMesh->mNumBones), sizeof(_uint));
		//cout << pAIMesh->mNumBones << endl;

		for (size_t i = 0; i < pAIMesh->mNumBones; i++)
		{
			aiBone* pAIBone = pAIMesh->mBones[i];

			outFile.write(reinterpret_cast<const char*>(&pAIBone->mName.length), sizeof(_uint));
			//cout << pAIBone->mName.length << endl;
			outFile.write(pAIBone->mName.data, pAIBone->mName.length);
			//cout << pAIBone->mName.data << endl;

			outFile.write(reinterpret_cast<const char*>(&pAIBone->mOffsetMatrix), sizeof(_float4x4));
			//cout << pAIBone->mOffsetMatrix.a1 << " " << pAIBone->mOffsetMatrix.a2 << " " << pAIBone->mOffsetMatrix.a3 << " " << pAIBone->mOffsetMatrix.a4 << endl;
			//cout << pAIBone->mOffsetMatrix.b1 << " " << pAIBone->mOffsetMatrix.b2 << " " << pAIBone->mOffsetMatrix.b3 << " " << pAIBone->mOffsetMatrix.b4 << endl;
			//cout << pAIBone->mOffsetMatrix.c1 << " " << pAIBone->mOffsetMatrix.c2 << " " << pAIBone->mOffsetMatrix.c3 << " " << pAIBone->mOffsetMatrix.c4 << endl;
			//cout << pAIBone->mOffsetMatrix.d1 << " " << pAIBone->mOffsetMatrix.d2 << " " << pAIBone->mOffsetMatrix.d3 << " " << pAIBone->mOffsetMatrix.d4 << endl;

			outFile.write(reinterpret_cast<const char*>(&pAIBone->mNumWeights), sizeof(_uint));
			//cout << pAIBone->mNumWeights << endl;
			for (size_t j = 0; j < pAIBone->mNumWeights; j++)
			{
				outFile.write(reinterpret_cast<const char*>(&pAIBone->mWeights[j].mVertexId), sizeof(_uint));
				//cout << pAIBone->mWeights[j].mVerte xId << endl;
				outFile.write(reinterpret_cast<const char*>(&pAIBone->mWeights[j].mWeight), sizeof(_float));
				//cout << pAIBone->mWeights[j].mWeight << endl;
			}
		}

	}


	for (size_t i = 0; i < pAIMesh->mNumFaces; i++)
	{
		outFile.write(reinterpret_cast<const char*>(&pAIMesh->mFaces[i].mIndices[0]), sizeof(_uint));
		//cout << pAIMesh->mFaces[i].mIndices[0] << endl;
		outFile.write(reinterpret_cast<const char*>(&pAIMesh->mFaces[i].mIndices[1]), sizeof(_uint));
		//cout << pAIMesh->mFaces[i].mIndices[1] << endl;
		outFile.write(reinterpret_cast<const char*>(&pAIMesh->mFaces[i].mIndices[2]), sizeof(_uint));
		//cout << pAIMesh->mFaces[i].mIndices[2] << endl;
	}
	return S_OK;
}
//55, 44
HRESULT CFbxToBinary::Write_Material(const aiMaterial* pAIMaterial, ofstream& outFile)
{
	for (size_t texTypeIdx = 1; texTypeIdx < AI_TEXTURE_TYPE_MAX; texTypeIdx++)
	{
		_uint numTextures = pAIMaterial->GetTextureCount(aiTextureType(texTypeIdx));
		outFile.write(reinterpret_cast<const char*>(&numTextures), sizeof(_uint));
		//cout << numTextures << endl;
		for (size_t i = 0; i < numTextures; i++)
		{
			aiString strPath;
			pAIMaterial->GetTexture(aiTextureType(texTypeIdx), i, &strPath);
			outFile.write(reinterpret_cast<const char*>(&strPath.length), sizeof(ai_uint32));
			//cout << strPath.length << endl;
			outFile.write(strPath.C_Str(), strPath.length);
			cout << strPath.C_Str() << endl;
		}

	}

	return S_OK;
}

HRESULT CFbxToBinary::Write_Bone(const aiNode* pAINode, ofstream& outFile)
{

	outFile.write(reinterpret_cast<const char*>(&pAINode->mName.length), sizeof(_uint));
	//cout << pAINode->mName.length << endl;
	outFile.write(pAINode->mName.data, pAINode->mName.length);
	cout << pAINode->mName.data << endl;

	//bool bBiiboard = false;
	//outFile.write(reinterpret_cast<const char*>(&bBiiboard), 1);
	outFile.write(reinterpret_cast<const char*>(&pAINode->mTransformation), sizeof(_float4x4));
	//cout << pAINode->mTransformation.a1 << " " << pAINode->mTransformation.a2 << " " << pAINode->mTransformation.a3 << " " << pAINode->mTransformation.a4 << endl;
	//cout << pAINode->mTransformation.b1 << " " << pAINode->mTransformation.b2 << " " << pAINode->mTransformation.b3 << " " << pAINode->mTransformation.b4 << endl;
	//cout << pAINode->mTransformation.c1 << " " << pAINode->mTransformation.c2 << " " << pAINode->mTransformation.c3 << " " << pAINode->mTransformation.c4 << endl;
	//cout << pAINode->mTransformation.d1 << " " << pAINode->mTransformation.d2 << " " << pAINode->mTransformation.d3 << " " << pAINode->mTransformation.d4 << endl;
	outFile.write(reinterpret_cast<const char*>(&pAINode->mNumChildren), sizeof(_uint));
	cout << pAINode->mNumChildren << endl;
	for (size_t i = 0; i < pAINode->mNumChildren; ++i)
	{
		Write_Bone(pAINode->mChildren[i], outFile);
	}
	return S_OK;
}

HRESULT CFbxToBinary::Write_Animation(const aiAnimation* pAIAnim, ofstream& outFile)
{
	outFile.write(reinterpret_cast<const char*>(&pAIAnim->mName.length), sizeof(_uint));
	//cout << pAIAnim->mName.length << endl;
	outFile.write(pAIAnim->mName.data, pAIAnim->mName.length);
	cout << pAIAnim->mName.data << endl;

	outFile.write(reinterpret_cast<const char*>(&pAIAnim->mDuration), sizeof(double));
	//cout << pAIAnim->mDuration << endl;
	outFile.write(reinterpret_cast<const char*>(&pAIAnim->mTicksPerSecond), sizeof(double));
	//cout << pAIAnim->mTicksPerSecond << endl;

	outFile.write(reinterpret_cast<const char*>(&pAIAnim->mNumChannels), sizeof(_uint));
	cout << pAIAnim->mNumChannels << endl;

	for (size_t i = 0; i < pAIAnim->mNumChannels; i++)
	{
		aiNodeAnim* pAIChannel = pAIAnim->mChannels[i];
		outFile.write(reinterpret_cast<const char*>(&pAIChannel->mNodeName.length), sizeof(_uint));
		//cout << pAINodeAnim->mNodeName.length << endl;
		outFile.write(pAIChannel->mNodeName.data, pAIChannel->mNodeName.length);
		//cout << pAINodeAnim->mNodeName.data << endl;

		_uint m_iNumKeyFrames = max(pAIChannel->mNumScalingKeys, pAIChannel->mNumRotationKeys);

		m_iNumKeyFrames = max(m_iNumKeyFrames, pAIChannel->mNumPositionKeys);
		outFile.write(reinterpret_cast<const char*>(&m_iNumKeyFrames), sizeof(_uint));

		_float3			vScale, vPosition;
		_float4			vRotation;
		for (size_t i = 0; i < m_iNumKeyFrames; i++)
		{
			TRANSFORM_KEYFRAME		KeyFrame = {};

			if (i < pAIChannel->mNumScalingKeys)
			{
				memcpy(&vScale, &pAIChannel->mScalingKeys[i].mValue, sizeof(_float3));
				KeyFrame.fTrackPosition = (_float)pAIChannel->mScalingKeys[i].mTime;
			}
			if (i < pAIChannel->mNumRotationKeys)
			{
				vRotation.x = pAIChannel->mRotationKeys[i].mValue.x;
				vRotation.y = pAIChannel->mRotationKeys[i].mValue.y;
				vRotation.z = pAIChannel->mRotationKeys[i].mValue.z;
				vRotation.w = pAIChannel->mRotationKeys[i].mValue.w;
				KeyFrame.fTrackPosition = (_float)pAIChannel->mRotationKeys[i].mTime;
			}
			if (i < pAIChannel->mNumPositionKeys)
			{
				memcpy(&vPosition, &pAIChannel->mPositionKeys[i].mValue, sizeof(_float3));
				KeyFrame.fTrackPosition =(_float) pAIChannel->mPositionKeys[i].mTime;
			}

			KeyFrame.vScale = vScale;
			KeyFrame.vRotation = vRotation;
			KeyFrame.vPosition = vPosition;

			outFile.write(reinterpret_cast<const char*>(&KeyFrame), sizeof(TRANSFORM_KEYFRAME));

		}


	}
	return S_OK;
}

