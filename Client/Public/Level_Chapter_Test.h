#pragma once
#include "Level.h"
BEGIN(Engine)
class CGameObject;
END

BEGIN(Client)
class CLevel_Chapter_Test final : public CLevel
{
private:
	CLevel_Chapter_Test(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CLevel_Chapter_Test() = default;

public:
	virtual HRESULT			Initialize(LEVEL_ID _eLevelID);
	virtual void			Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;

public:
	HRESULT					Ready_Lights();
	HRESULT					Ready_CubeMap(const _wstring& _strLayerTag);
	HRESULT					Ready_Layer_MainTable(const _wstring& _strLayerTag);
	HRESULT					Ready_Layer_Map();
	HRESULT					Ready_Layer_Camera(const _wstring& _strLayerTag, CGameObject* _pTarget);
	HRESULT					Ready_Layer_Player(const _wstring& _strLayerTag, CGameObject** _ppOut);
	HRESULT					Ready_Layer_TestTerrain(const _wstring& _strLayerTag);
	HRESULT					Ready_Layer_UI(const _wstring& _strLayerTag);
	HRESULT					Ready_Layer_NPC(const _wstring& _strLayerTag);
	HRESULT					Ready_Layer_Monster(const _wstring& _strLayerTag, CGameObject** _ppOut = nullptr);
	HRESULT					Ready_Layer_Effects(const _wstring& _strLayerTag);
	HRESULT					Ready_Layer_Effect2D(const _wstring& _strLayerTag);
private:
	void					Create_Arm(_uint _iCoordinateType, CGameObject* _pCamera, _float3 _vRotation, _float _fLength);

	HRESULT Map_Object_Create(_wstring _strFileName);

	LEVEL_ID				m_eLevelID;

public:
	static CLevel_Chapter_Test* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, LEVEL_ID _eLevelID);
	virtual void			Free() override;
};

END

