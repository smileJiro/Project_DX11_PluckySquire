#pragma once

#include "Base.h"
#include "Map_Tool_Defines.h"
#include "MapObject.h"


BEGIN(Map_Tool)



class CModel_PackagingObject final : public CBase
{
private:
	typedef struct tagPackagingBone
	{
		_uint						iNumChildren;
		_uint						iNameSize;
		_char						szName[MAX_PATH];
		_float4x4					matTransformation;

	}BONE;

	typedef	struct tagPackagingAnimBone
	{
		_uint						iNameSize;
		_char						szName[MAX_PATH];
		_float4x4					matOffset;
		_uint						iNumVertices;

		//iVertexIdx, fWeight
		vector<pair<_uint, _float>> BoneInfos;
	} ANIMBONE;

	typedef struct tagPackagingMesh
	{
		_uint				iMaterialIndex;
		_uint				iNameSize;
		_char				szName[MAX_PATH];
		_uint				iNumVertices;
		_uint				iNumFaces;

		//NonANim
		vector<VTXMESH>		Vertices;
		_uint				iNumBones;
		vector<ANIMBONE>	BoneInfos;



		vector<_uint>		Indices;
	}MESH;

	typedef struct tagPackagingMaterial
	{
		_uint iNumSRVs[aiTextureType_UNKNOWN];
		vector<pair<_string, _uint>> SRVNames[aiTextureType_UNKNOWN];

	}MATERIAL;

	typedef struct tagPackagingChannel
	{
		_uint			iBoneNameSize;
		_char			szBoneName[MAX_PATH];
		_uint			iNumKeyFrames;
		vector<KEYFRAME> KeyFrames;

	}CHANNEL;

	typedef struct tagPackagingAnimation
	{
		_uint			iNameSize;
		_char			szName[MAX_PATH];
		_float			fDuration;
		_float			fTickPerSecond;
		_uint			iNumChannels;
		vector<CHANNEL>	Channels;

		// 1.26 김지완이 추가함
		_bool bLoop = false;
		_float fSpeedMagnifier = 1.f;
	}ANIMATION;



private:
	CModel_PackagingObject();
	virtual ~CModel_PackagingObject() = default;

public:
	HRESULT Import(const _string _strFIlePath);
	HRESULT Export(const _string _strFIlePath);

	HRESULT Set_Sync_Material(CMapObject* _pTargetObject);

private:
	HRESULT Import_Bone(ifstream& _inFile, _uint _iParentBoneIndex);
	HRESULT Import_Meshes(ifstream& _inFile);
	HRESULT Import_Materials(ifstream& _inFile, const _string _strFIlePath);
	HRESULT Import_Animations(ifstream& _inFile);
	
private:
	HRESULT Export_Bone(ofstream& _outFile, _uint _iParentBoneIndex);
	HRESULT Export_Meshes(ofstream& _outFile);
	HRESULT Export_Materials(ofstream& _outFile, const _string _strFIlePath);
	HRESULT Export_Animations(ofstream& _outFile);


public:
	static CModel_PackagingObject* Create(const _string _strFIlePath);
	virtual void Free() override;
private:
	_bool m_isAnimation;

	vector<BONE> m_Bones;
	_uint m_iNumMeshes;
	vector<MESH> m_Meshes;
	_uint m_iNumMaterials;
	vector<MATERIAL> m_Materials;
	_uint m_iNumAnimations;
	vector<ANIMATION> m_Animations;
};

END
