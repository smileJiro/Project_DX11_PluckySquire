#pragma once

#include "Map_Tool_Defines.h"
#include "Base.h"
#include "2DModel.h"

BEGIN(Engine)
class CSpriteFrame;
END


BEGIN(Map_Tool)

class C2DMapObjectInfo final : public CBase
{
public :
	enum MAPOBJ_MODEL_TYPE
	{
		MODEL_ANIM,
		MODEL_NONANIM,
		MODEL_END,
	};

	enum MAPOBJ_ACTIVE_TYPE
	{
		ACTIVE_PATROL,
		ACTIVE_BREAKABLE,
		ACTIVE_ATTACKABLE,
		ACTIVE_DIALOG,
		ACTIVE_END
	};


	enum MAPOBJ_2D_COLLIDIER_TYPE
	{
		COLLIDER_AABB,
		COLLIDER_SQUARE,
		COLLIDER_END
	};

private:
	C2DMapObjectInfo();
	C2DMapObjectInfo(const C2DMapObjectInfo& Prototype);
	virtual ~C2DMapObjectInfo() = default;

public:
	HRESULT Initialize(json _InfoJson, _string* _arrModelTypeString, _string* _arrActiveTypeString, _string* _arrColliderTypeString);

	_float2			m_fTextureOffsetSize;

public :
	MAPOBJ_MODEL_TYPE			Get_ModelType() { return m_eModelType; };
	void						Set_ModelType(MAPOBJ_MODEL_TYPE _eType) { m_eModelType = _eType; };


	const _string&				Get_SearchTag() { return m_strSearchTag; };
	void						Set_SearchTag(const _string& _eType) { m_strSearchTag = _eType; };

	const _string&				Get_TextureName() { return m_strTextureName; };
	void						Set_TextureName(const _string& _eType) { m_strTextureName = _eType; };


	_bool						Is_ToolRendering() { return m_isToolRendering; };
	_bool						Is_ModelLoad() { return m_isModelCreate; };
	_bool						Is_Active() { return m_isActive; };
	_bool						Is_Collider() { return m_isCollider; };

	void						Set_Active(_bool _isActive) { m_isActive = _isActive; }
	void						Set_Collider(_bool _isCollider) { m_isCollider = _isCollider; }

	MAPOBJ_ACTIVE_TYPE			Get_ActiveType() { return m_eActiveType; };
	void						Set_ActiveType(MAPOBJ_ACTIVE_TYPE _eType) { m_eActiveType = _eType; };

	MAPOBJ_2D_COLLIDIER_TYPE	Get_ColliderType() { return m_eColliderType; };
	void						Set_ColliderType(MAPOBJ_2D_COLLIDIER_TYPE _eType) { m_eColliderType = _eType; };
	
	ID3D11ShaderResourceView*	Get_SRV(_float2* _pReturnSize = nullptr);
	CTexture*					Get_Texture(){ return m_pTexture; }

private :

	CGameInstance*				m_pGameInstance = { nullptr };
	Engine::C2DModel*			m_pModel = { nullptr };
	CTexture*					m_pTexture = { nullptr };

	_string						m_strSearchTag = "";
	_string						m_strTextureName = "";


	_bool						m_isActive		= false;
	_bool						m_isCollider	= false;

	_bool						m_isModelCreate	= false;
	_bool						m_isToolRendering = false;

	MAPOBJ_MODEL_TYPE			m_eModelType;
	MAPOBJ_ACTIVE_TYPE			m_eActiveType;
	MAPOBJ_2D_COLLIDIER_TYPE	m_eColliderType;


public:
	static C2DMapObjectInfo* Create(json _InfoJson, _string* _arrModelTypeString, _string* _arrActiveTypeString, _string* _arrColliderTypeString);
	static C2DMapObjectInfo* Create();
	virtual void Free() ;

};

END