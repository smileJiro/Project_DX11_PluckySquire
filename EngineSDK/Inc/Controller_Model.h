#pragma once
#include "Base.h"
#include "3DModel.h"
/* 2D Model(CVIBuffer_Rect) 와 3D Model(CModel) 을 관리하는 클래스. */
/* 현재 Object의 Coordinate State에 따라, 기능을 실행, 제한 등 컨트롤을 하는 클래스 */
/* Transform에 비해 상대적으로 공통 된 기능이 없다시피 함. PlayAnimation, SetupAnimation 정도? */

BEGIN(Engine)
class CGameInstance;

class CModel;
class CShader;
class CVIBuffer_Rect;
class CTexture;
class ENGINE_DLL CController_Model : public CBase
{
public:
	typedef struct tagControllerModelDesc
	{
		 COORDINATE eStartCoord = COORDINATE_LAST;
		_bool isCoordChangeEnable = false;

		_uint iCurLevelID;
		_uint i2DModelPrototypeLevelID;
		_uint i3DModelPrototypeLevelID;
		wstring wstr2DModelPrototypeTag;
		wstring wstr3DModelPrototypeTag;

		C3DModel::MODEL3D_DESC tModel3DDesc{};
	}CON_MODEL_DESC;

protected:
	CController_Model(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CController_Model() = default;

public:
	HRESULT			Initialize(CON_MODEL_DESC* _pDesc);

	HRESULT			Render(CShader* _Shader, _uint _iShaderPass);
	HRESULT			Render_Default(CShader* _pShader, _uint _iShaderPass);	// Material Binding을 하지 않는 Rendering, 따로 Bind를 해야합니다.

public:
	HRESULT			Change_Coordinate(COORDINATE _eCoordinate);


public: /* 2D, 3D */
	void			Play_Animation(_float fTimeDelta, _bool _bReverse = false);
	void			Register_OnAnimEndCallBack(const function<void(COORDINATE,_uint)>& fCallback) { m_listAnimEndCallBack.push_back(fCallback); }
	void			Invoke_OnAnimEndCallBack(COORDINATE _eCoord, _uint iAnimIdx);
	//Get
	CModel*			Get_Model(COORDINATE _eCoord) {return m_ModelComs[_eCoord];}

	//Set
	void Set_AnimationLoop(COORDINATE _eCoord, _uint iIdx, _bool bIsLoop);
	void Set_Animation(_uint iIdx, _bool _bReverse = false);
	void Switch_Animation(_uint iIdx, _bool _bReverse = false);
	void To_NextAnimation();

	HRESULT			Change_Model(COORDINATE _eCoord, _uint _eProtoLevel, const _wstring _strModelTag);




public:
	HRESULT Binding_TextureIndex_To_3D(_uint _iIndex, _uint _eTextureType = aiTextureType_DIFFUSE, _uint _iMaterialIndex = 0);
	_uint Get_TextureIndex_To_3D(_uint _eTextureType = aiTextureType_DIFFUSE , _uint _iMaterialIndex = 0);
private:
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;
	CGameInstance* m_pGameInstance = nullptr;

protected:

	CModel* m_ModelComs[COORDINATE_LAST] = {};
	COORDINATE				m_eCurCoord = COORDINATE_LAST;
	_bool					m_isCoordChangeEnable = false;


	list<function<void(COORDINATE,_uint)>> m_listAnimEndCallBack;
private:
	HRESULT					Ready_Models(CON_MODEL_DESC* _pDesc);

public:
	static CController_Model* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, CON_MODEL_DESC* _pDesc);
	virtual void Free() override;
};
END

