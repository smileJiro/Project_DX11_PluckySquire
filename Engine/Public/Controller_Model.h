#pragma once
#include "Base.h"
/* 2D Model(CVIBuffer_Rect) 와 3D Model(CModel) 을 관리하는 클래스. */
/* 현재 Object의 Coordinate State에 따라, 기능을 실행, 제한 등 컨트롤을 하는 클래스 */
/* Transform에 비해 상대적으로 공통 된 기능이 없다시피 함. PlayAnimation, SetupAnimation 정도? */

BEGIN(Engine)
class CGameInstance;

class C3DModel;
class C2DModel;
class CShader;
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

	}CON_MODEL_DESC;

private:
	CController_Model(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CController_Model() = default;

public:
	HRESULT			Initialize(CON_MODEL_DESC* _pDesc);

	HRESULT			Render(_uint iMeshIndex = 0);

public:
	HRESULT			Change_Coordinate(COORDINATE _eCoordinate);


public: /* 2D, 3D */
	void			Play_Animation() {};
	void			Setup_Animation() {};

public: /* 2D */

public: /* 3D */

public:

private:
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;
	CGameInstance* m_pGameInstance = nullptr;

private:
	C2DModel* m_p2DModelCom = nullptr;
	C3DModel* m_p3DModelCom = nullptr;

	COORDINATE				m_eCurCoord = COORDINATE_LAST;
	_bool					m_isCoordChangeEnable = false;

private:
	HRESULT					Ready_Models(CON_MODEL_DESC* _pDesc);

public:
	static CController_Model* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, CON_MODEL_DESC* _pDesc);
	virtual void Free() override;
};
END

