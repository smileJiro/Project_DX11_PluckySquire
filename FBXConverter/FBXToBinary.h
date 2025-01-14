#pragma once

typedef struct Keyframe
{
	_float			fTrackPosition = { 0 };
}KEYFRAME;
typedef struct  TransformKeyframe : public KEYFRAME
{
	_float3		vScale;
	_float4		vRotation;
	_float3		vPosition;
}TRANSFORM_KEYFRAME;

//typedef struct TransformKeyframe : public KEYFRAME
//{
//	_float3		vScale = { 1,1,1 };
//	_float4		vRotation = { 0,0,0,0 };
//	_float3		vPosition = { 0,0,0 };
//
//}TRANSFORM_KEYFRAME;
class CFbxToBinary
{
public:
	enum TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_END };

public:
	CFbxToBinary();
	~CFbxToBinary();

	HRESULT FbxToBinary(const string& inFilePath);
	//HRESULT BinaryToModel(const string& file, TYPE type = TYPE_NONANIM);

private:
	HRESULT Write_Meshe(const aiMesh* pAIMesh, ofstream& outFile);
	HRESULT Write_Material(const aiMaterial* pAIMaterial, ofstream& outFile);
	HRESULT Write_Bone(const aiNode* pAIBone, ofstream& outFile);
	HRESULT Write_Animation(const aiAnimation* pAIAnim, ofstream& outFile);
private:
	Assimp::Importer			m_Importer;
	const aiScene* m_pAIScene = { nullptr };
	bool m_bAnim = { false };
};
