#ifndef Engine_Struct_h__
#define Engine_Struct_h__

#include "Engine_Typedef.h"

namespace Engine
{

	typedef struct 
	{
		HINSTANCE		hInst;
		HWND			hWnd;
		bool			isWindowed;
		unsigned int	iNumLevels;
		unsigned int	iViewportWidth;
		unsigned int	iViewportHeight;
		_uint			iStaticLevelID;
	}ENGINE_DESC;

	typedef struct
	{
		enum TYPE { TYPE_POINT, TYPE_DIRECTOINAL, TYPE_END };

		TYPE			eType;
		XMFLOAT4		vDirection;
		XMFLOAT4		vPosition;
		float			fRange;

		XMFLOAT4		vDiffuse;
		XMFLOAT4		vAmbient;
		XMFLOAT4		vSpecular;
	}LIGHT_DESC;

	typedef struct
	{
		XMFLOAT4		vDiffuse;
		XMFLOAT4		vAmbient;
		XMFLOAT4		vSpecular;
	}MATERIAL_DESC;

#pragma region Vertex
	typedef struct ENGINE_DLL tagVtxPosition
	{
		XMFLOAT3		vPosition;
		static const unsigned int					iNumElements = { 1 };
		static const D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];
	}VTXPOS;

	typedef struct ENGINE_DLL tagVtxPosition_Texture
	{
		XMFLOAT3		vPosition;
		XMFLOAT2		vTexcoord;

		static const unsigned int					iNumElements = { 2 };
		static const D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];
	}VTXPOSTEX;

	typedef struct ENGINE_DLL tagVtxPosition_Cube
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vTexcoord;

		static const unsigned int					iNumElements = { 2 };
		static const D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];
	}VTXCUBE;

	typedef struct ENGINE_DLL tagVtxNormal_Texture
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;

		static const unsigned int					iNumElements = { 3 };
		static const D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];
	}VTXNORTEX;

	typedef struct ENGINE_DLL tagVtxMesh
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;
		XMFLOAT3		vTangent;

		static const unsigned int					iNumElements = { 4 };
		static const D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];
	}VTXMESH;

	typedef struct ENGINE_DLL tagVtxAnimMesh
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;
		XMFLOAT3		vTangent;
		XMUINT4			vBlendIndices; /* 메쉬클래스에 저장된, 인덱스벡터의 인덱스를 저장할 것임. (메쉬의 m_BoneIndices : 자기자신 메쉬에게 영향을 주는 본들의 Model 클래스 m_Bones 상의 index값을 저장하고있음. )*/
		XMFLOAT4		vBlendWeights; /* 정점은 최대 4개의 Bone의 영향을 받는다. 그래서 int 4개이고, float4개인 Weight는 얼만큼 영향을 받는가에 대한 0 ~ 1 사이의 실수값. x,y,z,w 를 모두 더하면 1이 나옴.*/
		static const unsigned int					iNumElements = { 6 };
		static const D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];
	}VTXANIMMESH;



	/* Instancing */
	typedef struct ENGINE_DLL tagVtxPoint
	{
		XMFLOAT3		vPosition;
		XMFLOAT2		vSize;
	}VTXPOINT;
	typedef struct ENGINE_DLL tagVtxInstance
	{
		XMFLOAT4 vRight, vUp, vLook, vTranslation;
		XMFLOAT2 vLifeTime;
	}VTXINSTANCE;

	typedef struct ENGINE_DLL tagVtxRectInstance
	{
		static const unsigned int iNumElements = 7; /* VTXPOSTEX 의 Position, Texcoord  +  VTXINSTANCE의 vRight, vUp, vLook, vTranslation */
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	}VTXRECTINSTANCE;

	typedef struct ENGINE_DLL tagVtxPointInstance
	{
		static const unsigned int iNumElements = 7; /* VTXPOSTEX 의 Position, Texcoord  +  VTXINSTANCE의 vRight, vUp, vLook, vTranslation */
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	}VTXPOINTINSTANCE;

#pragma endregion // Vertex

	typedef struct tagKeyFrame
	{
		XMFLOAT3 vScale;
		XMFLOAT4 vRotation; // 쿼터니언을 사용하기 위해서.
		XMFLOAT3 vPosition;
		float	 fTrackPosition; // 애니메이션 트랙위에서의 포지션을 정의.
	}KEYFRAME;



#pragma region Binary 관련

	/* Binary_Manager Struct */
	typedef struct tagFbxMesh
	{
		char szName[MAX_PATH];
		unsigned int iMaterialIndex;
		unsigned int iNumVertices;
		vector<VTXANIMMESH> vecVertices;
		vector<unsigned int> vecBoneIndices;
		vector<XMFLOAT4X4> vecOffsetMatrix;
		unsigned int iNumFaces;
		unsigned int* pIndices; // 인덱스 개수만큼 동적할당한 포인터를 담아서 전달할거임. 저장은 당연히 _uint연속적으로 데이터에 넣어야겠지.
		unsigned int iNumBones;

	}FBX_MESH;
	typedef struct tagFbxMaterial
	{
		/* txt 파일을 읽으면서 수행할 로직. */
		/* 1. Material 텍스쳐 25개를 순회한다. */
		/* 2. 각 텍스쳐마다 장수를 확인한다. */
		/* 3. 0이면 vector 인덱스를 1 올리고, 0이 아니라면, 그 횟수로 반복문 수행. >>> max_path 만큼 문자열을 읽어 저장한다.*/
		/* 4. 이때 char* 타입 */
		vector<wstring> vecTextureFullPath[25];
	}FBX_MATERIAL;
	typedef struct tagFbxBone
	{
		char szName[MAX_PATH];
		_float4x4 TransformationMatrix;
		_int m_iParentBoneIndex;

	}FBX_BONE;
	typedef struct tagFbxChannel
	{
		char szName[MAX_PATH];
		unsigned int iNumKeyFrames;
		unsigned int iBoneIndex;
		vector<KEYFRAME> vecKeyFrames;
	}FBX_CHANNEL;
	typedef struct tagFbxAnimation
	{
		char szName[MAX_PATH];
		unsigned int iNumChannels;
		_float fDuration;
		_float fTickPerSec;
		vector<FBX_CHANNEL> vecChannels;

	}FBX_ANIMATION;
	typedef struct tagNonAnimFbx
	{
		/* 만약 더 최적화 한다면, 구조체를 안쓰고 바로 값넣어주기 or 벡터 다 빼고 포인터타입으로 IndexBuffer처럼 값얕복으로 전달.*/
		unsigned int iNumMeshes;
		unsigned int iNumMaterials;

		vector<FBX_MESH> vecMeshes;

		vector<FBX_MATERIAL> vecMaterials;
	}NONANIM_FBX;

	typedef struct tagAnimFbx
	{
		/* 만약 더 최적화 한다면, 구조체를 안쓰고 바로 값넣어주기 or 벡터 다 빼고 포인터타입으로 IndexBuffer처럼 값얕복으로 전달.*/
		unsigned int eModelType;
		unsigned int iNumDebugBones;
		unsigned int iNumMeshes;
		unsigned int iNumAnimations;
		unsigned int iNumMaterials;
		vector<FBX_BONE> vecBones;
		vector<FBX_MESH> vecMeshes;
		vector<FBX_ANIMATION> vecAnimations;
		vector<FBX_MATERIAL> vecMaterials;
	}ANIM_FBX;
#pragma endregion

	








}


#endif // Engine_Struct_h__
