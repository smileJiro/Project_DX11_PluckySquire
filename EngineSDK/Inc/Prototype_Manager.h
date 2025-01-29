#pragma once
/* Component */
#include "Shader.h"
#include "VIBuffer_Rect.h"
#include "VIBuffer_Rect_Vertical.h"
#include "VIBuffer_Terrain.h"
#include "VIBuffer_Cube.h"
#include "VIBuffer_Point_Particle.h"
#include "Controller_Transform.h"
#include "Texture.h"
#include "Ray.h"
#include "3DModel.h"
#include "Mesh.h"
#include "Actor_Dynamic.h"
#include "Actor_Static.h"
#include "VIBuffer_PxDebug.h"
#include "Controller_Model.h"
#include "Material.h"
#include "Bone.h"
#include "AnimEventGenerator.h"
#include "DebugDraw_For_Client.h"

/* Etc */
#include "Transform_2D.h"
#include "Transform_3D.h"
#include "CustomFont.h"
#include "RenderTarget.h"

/* Trigger */
#include "TriggerObject.h"

/* Camera */
#include "Camera_Free.h"
#include "CameraArm.h"
#include "CutScene_Sector.h"

/* Object */
#include "Particle_Sprite_Emitter.h"
#include "Particle_Mesh_Emitter.h"
#include "Particle_System.h"

/* 원형객체들을 레벨별로 관리하겠다. */
/* 특정 레벨에서 복제할 필요가 없는 원형들은 삭제하겠다 */
/* 지정한 객체를 복제하여 리턴한다. */

BEGIN(Engine)
class CPrototype_Manager final : public CBase
{
	
private:
	CPrototype_Manager();
	virtual ~CPrototype_Manager() = default;

public:
	HRESULT Initialize(_uint _iNumLevels, _uint _iExcludeLevelID);
	HRESULT Add_Prototype(_uint _iLevelID, const _wstring& _strPrototypeTag, class CBase* _pPrototype);
	class CBase* Clone_Prototype(Engine::PROTOTYPE _eType, _uint _iLevelID, const _wstring& _strPrototypeTag, void* _pArg); 
	class CBase* Find_Prototype(_uint _iLevelID, const _wstring& _strPrototypeTag); //바이너리때문에 걍 뻈음.
	HRESULT	Level_Exit(_uint _iLevelID);

private:
	_uint												m_iNumLevels = 0;
	_uint												m_iExcludeLevelID = 0;
	map<const _wstring, class CBase*>*					m_pPrototypes = nullptr; // 동적배열로 만들 것임. >> new PROTOTYPE[iNumLevels];
	typedef map<const _wstring, class CBase*> PROTOTYPE;

private:
	HRESULT Clear(_uint _iLevelID);
public:
	static CPrototype_Manager* Create(_uint _iNumLevels, _uint _iExcludeLevelID);
	virtual void Free();
};

END