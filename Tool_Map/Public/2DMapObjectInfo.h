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

private:
	C2DMapObjectInfo();
	C2DMapObjectInfo(const C2DMapObjectInfo& Prototype);
	virtual ~C2DMapObjectInfo() = default;

public:
	HRESULT Initialize(json _InfoJson);
	HRESULT Export(json& _OutputJson);

	_float2			m_fTextureOffsetSize;

public :
	_uint						Get_ModelType() { return m_eModelType; };
	void						Set_ModelType(_uint _eType) { m_eModelType = _eType; };


	const _string&				Get_SearchTag() { return m_strSearchTag; };
	void						Set_SearchTag(const _string& _eType) { m_strSearchTag = _eType; };

	const _string&				Get_ModelName() { return m_strModelName; };
	void						Set_ModelName(const _string& _eType) { m_strModelName = _eType; };


	_bool						Is_ToolRendering() { return m_isToolRendering; };
	_bool						Is_ModelLoad() { return m_isModelCreate; };
	_bool						Is_Active() { return m_isActive; };
	_bool						Is_Collider() { return m_isCollider; };
	_bool						Is_Sorting() { return m_isSorting; };
	_bool						Is_BackGround() { return m_isBackGround; };

	void						Set_Active(_bool _isActive) { m_isActive = _isActive; }
	void						Set_Collider(_bool _isCollider) { m_isCollider = _isCollider; }
	void						Set_Sorting(_bool _isSorting) { m_isSorting = _isSorting; }
	void						Set_BackGround(_bool _isSorting) { m_isBackGround = _isSorting; }

	_uint						Get_ActiveType() { return m_eActiveType; };
	void						Set_ActiveType(_uint _eType) { m_eActiveType = _eType; };

	_uint						Get_ColliderType() { return m_eColliderType; };
	void						Set_ColliderType(_uint _eType) { m_eColliderType = _eType; };
	
	ID3D11ShaderResourceView*	Get_SRV(_float2* _pReturnSize = nullptr);
	CTexture*					Get_Texture(){ return m_pTexture; }

	_uint						Get_ModelIndex() { return m_iModelIndex; }
	void						Set_ModelIndex(_uint _iIndex) { m_iModelIndex = _iIndex; }
	


	_float2						Get_Collider_Offset_Pos(){ return m_fColliderOffsetPos;}
	_float2						Get_Collider_Extent() { return m_fColliderExtent; }
	_float						Get_Collider_Radius(){ return m_fColliderRadius;}


	void						Set_Collider_Offset_Pos(_float2 _fValue) { m_fColliderOffsetPos = _fValue; }
	void						Set_Collider_Extent(_float2 _fValue) { m_fColliderExtent = _fValue; }
	void						Set_Collider_Radius(_float _fValue) { m_fColliderRadius = _fValue; }




	_float2						Get_Sorting_Pos() { return m_fSortingPosition; }
	void						Set_Sorting_Pos(_float2 _fValue) { m_fSortingPosition = _fValue; }



	_float2						Get_Size();


	_bool						Is_Delete() { return m_isDelete; };
	void						Set_Delete(_bool _isDelete) { m_isDelete = _isDelete; }

	void						Set_Model(C2DModel* _pModel);

private :

	CGameInstance*				m_pGameInstance = { nullptr };
	Engine::C2DModel*			m_pModel = { nullptr };
	CTexture*					m_pTexture = { nullptr };

	_uint						m_iModelIndex = 0;

	// DATA -----------------
	_string						m_strSearchTag = "";
	_string						m_strModelName = "";


	_bool						m_isActive		= false;
	_bool						m_isCollider	= false;
	_bool						m_isBackGround	= false;

	_bool						m_isModelCreate	= false;
	_bool						m_isToolRendering = false;

	_uint						m_eModelType = 0;
	_uint						m_eActiveType = 0;
	_uint						m_eColliderType = 0;

	_bool						m_isSorting = false;
	_float2						m_fSortingPosition = {};

	_float2						m_fColliderOffsetPos = {};
	_float2						m_fColliderExtent = {};
	_float						m_fColliderRadius = {};
	
	
	_bool						m_isDelete = false; 
	
	
	// DATA -----------------


public:
	static C2DMapObjectInfo* Create(json _InfoJson);
	static C2DMapObjectInfo* Create();
	virtual void Free() ;

};

END