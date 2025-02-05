#pragma once
#include "PartObject.h"
BEGIN(Engine)
class CVIBuffer_Rect;
class CModel;
class CController_Model;
class ENGINE_DLL CModelObject : public CPartObject
{
public:
	typedef struct tagModelObjectDesc : public CPartObject::PARTOBJECT_DESC
	{
		_uint iModelPrototypeLevelID_2D;
		_uint iModelPrototypeLevelID_3D;
		_wstring strModelPrototypeTag_2D;
		_wstring strModelPrototypeTag_3D;

		_wstring strShaderPrototypeTag_2D;
		_wstring strShaderPrototypeTag_3D;

		// 2D ShaderPass
		_uint iShaderPass_2D = {};
		// 3D ShaderPass
		_uint iShaderPass_3D = {};
		_float fFrustumCullingRange = 2.0f;

		_uint iRenderGroupID_3D = 0;
		_uint iPriorityID_3D = 0;


	#pragma region Build Method
	void Build_2D_Model(_uint _iLevelID, const 
		_wstring _strProtoModelTag, 
		const _wstring _strProtoShaderTag,
		_uint _iShaderPass = 0, 
		_bool _isCoordChangeEnable = false)
	{
		isCoordChangeEnable = _isCoordChangeEnable;
		eStartCoord = COORDINATE_2D;
		iCurLevelID = _iLevelID;
		iModelPrototypeLevelID_2D = _iLevelID;
		strModelPrototypeTag_2D = _strProtoModelTag;
		strShaderPrototypeTag_2D = _strProtoShaderTag;
		iShaderPass_2D = _iShaderPass;
	}

	void Build_3D_Model(_uint _iLevelID, const _wstring _strProtoModelTag, 
		const _wstring _strProtoShaderTag,
		_uint _iShaderPass = 0, 
		_bool _isCoordChangeEnable = false)
	{
		isCoordChangeEnable = _isCoordChangeEnable;
		eStartCoord = COORDINATE_3D;
		iCurLevelID = _iLevelID;
		iModelPrototypeLevelID_3D = _iLevelID;
		strModelPrototypeTag_3D = _strProtoModelTag;
		strShaderPrototypeTag_3D = _strProtoShaderTag;
		iShaderPass_3D = _iShaderPass;
	}
	#pragma endregion



	}MODELOBJECT_DESC;

protected:
	CModelObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CModelObject(const CModelObject& _Prototype);
	virtual ~CModelObject() = default;

public:
	virtual HRESULT Initialize_Prototype(); // 프로토 타입 전용 Initialize
	virtual HRESULT Initialize(void* _pArg); // 초기화 시 필요한 매개변수를 void* 타입으로 넘겨준다.
	virtual void			Priority_Update(_float _fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;
	virtual HRESULT Render_Shadow() { return S_OK; }
	virtual HRESULT			Render() override;
	virtual HRESULT				Change_Coordinate(COORDINATE _eCoordinate, _float3* _pNewPosition = nullptr) override;

public:
	_bool Is_PickingCursor_Model(_float2 _fCursorPos, _float& _fDst);
	_bool Is_PickingCursor_Model_Test(_float2 _fCursorPos, _float& _fDst);
	//const _float4x4* Find_BoneMatrix(const _char* _pBoneName) const;
	void Register_OnAnimEndCallBack(const function<void(COORDINATE,_uint)>& fCallback);

public:
	// Get
	_uint					Get_ShaderPassIndex(COORDINATE _eCurCoord) { return m_iShaderPasses[_eCurCoord]; }
	CModel*					Get_Model(COORDINATE _eCoord);

	// Set
	void					Set_AnimationLoop(COORDINATE _eCoord, _uint iIdx, _bool bIsLoop);
	void					Set_Animation(_uint iIdx);
	void					Switch_Animation(_uint iIdx);
	void					To_NextAnimation();

	void					Change_TextureIdx(_uint _iIndex, _uint _eTextureType, _uint _iMaterialIndex = aiTextureType_DIFFUSE);
	_uint					Get_TextureIdx(_uint _eTextureType, _uint _iMaterialIndex = aiTextureType_DIFFUSE);
	void					Set_PlayingAnim(_bool _bPlaying);
	void					Set_ReverseAnimation(_bool _bReverse) { m_bReverseAnimation = _bReverse; }
	_bool					Is_ReverseAnimation() { return m_bReverseAnimation; }

	void					Change_RenderGroup(COORDINATE _eCoord, _uint _iGroupKey, _uint _iPriorityKey)
	{ 
		if (COORDINATE_2D == _eCoord)
		{
			m_iRenderGroupID_2D = _iGroupKey;
			m_iPriorityID_2D = _iPriorityKey;
		}
		else if(COORDINATE_3D == _eCoord)
		{
			m_iRenderGroupID_3D = _iGroupKey;
			m_iPriorityID_3D = _iPriorityKey;
		}
	}


protected:
	CController_Model*		m_pControllerModel = nullptr;

	CShader*				m_pShaderComs[COORDINATE_LAST] = {};
	_uint					m_iShaderPasses[COORDINATE_LAST] = {};
	_wstring				m_strModelPrototypeTag[COORDINATE_LAST];

	_float					m_fFrustumCullingRange = 0.0f;
	_uint					m_iRenderGroupID_2D = 0;
	_uint					m_iPriorityID_2D = 0;
	_uint					m_iRenderGroupID_3D = 0;
	_uint					m_iPriorityID_3D = 0;
	_bool					m_bPlayingAnim = true;	
	_bool					m_bReverseAnimation = false;
protected:
	virtual HRESULT			Bind_ShaderResources_WVP();


protected:
	HRESULT					Ready_Components(MODELOBJECT_DESC* _pDesc);


public:
	static CModelObject* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;

#ifdef _DEBUG
public:
	HRESULT Imgui_Render_ObjectInfos() override;
#endif // _DEBUG
};
END
