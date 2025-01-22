#pragma once
#include "Level.h"
BEGIN(Engine)
class CModel;
class CTestTerrain;
END

BEGIN(AnimTool)
class CCamera_Target;
class CTestModelObject;

class CLevel_AnimTool :
	public CLevel
{
public:
	enum LOADMODEL_TYPE {
		LOAD_3D,
		LOAD_2D,
		LOAD_RAW2D,
		LOAD_LAST
	};;
private:
	CLevel_AnimTool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CLevel_AnimTool() = default;

public:
	virtual HRESULT			Initialize() override;
	virtual void			Update(_float _fTimeDelta) override;

	virtual HRESULT			Render() override;
	void Update_Imgui();
	void Set_Animation(_uint _iAnimIdx, _bool _bLoop = true);
private:
	HRESULT					Ready_Lights();
	HRESULT					Ready_Layer_TestTerrain(const _wstring& _strLayerTag);
	HRESULT					Create_Camera(const _wstring& _strLayerTag,CGameObject* _pTarget);
	HRESULT					Load_Model(LOADMODEL_TYPE _eType, wstring _wstrPath);

	HRESULT					Export_Model(const wstring& _wstrPath);
	HRESULT					Copy_Textures(CTestModelObject* _pModel, std::filesystem::path& _wstrSrcPath, std::filesystem::path& _wstrDstPath);
private:
	//모델 로딩 관련
	CTestTerrain* m_pTestTerrain = nullptr;

	std::filesystem::path					m_szLoadedPath;
	CTestModelObject* m_pTestModelObj = nullptr;
	CCamera_Target* m_pTargetCam = nullptr;


	_float m_fDefault3DCamFovY = XMConvertToRadians(60.f);
	_float2 m_fDefault2DCamSize = {};
	_float m_f3DZoomSpeed = 5.f;
	_float m_f2DZoomSpeed = 10.f;
	_float m_fZoomMultiplier = 1.f;

	_bool m_bExportTextures = false;

public:
	static CLevel_AnimTool* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void			Free() override;
};

END