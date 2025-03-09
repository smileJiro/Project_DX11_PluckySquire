#pragma once
#include "Level.h"
BEGIN(Engine)
class CGameObject;
END

BEGIN(Client)
class CSpawner;
class CLevel_Chapter_02 final : public CLevel
{
private:
	CLevel_Chapter_02(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CLevel_Chapter_02() = default;

public:
	virtual HRESULT			Initialize(LEVEL_ID _eLevelID);
	virtual void			Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;

public:
	HRESULT					Ready_Lights();
	HRESULT					Ready_CubeMap(const _wstring& _strLayerTag);
	HRESULT					Ready_Layer_MainTable(const _wstring& _strLayerTag);
	HRESULT					Ready_Layer_Map();
	HRESULT					Ready_Layer_Spawner(const _wstring& _strLayerTag);
	HRESULT					Ready_Layer_Camera(const _wstring& _strLayerTag, CGameObject* _pTarget);
	HRESULT					Ready_Layer_Player(const _wstring& _strLayerTag, CGameObject** _ppOut);
	HRESULT					Ready_Layer_Book(const _wstring& _strLayerTag);
	HRESULT					Ready_Layer_UI(const _wstring& _strLayerTag);
	HRESULT					Ready_Layer_Item(const _wstring& _strLayerTag);
	HRESULT					Ready_Layer_NPC(const _wstring& _strLayerTag);
	HRESULT					Ready_Layer_Monster(const _wstring& _strLayerTag, CGameObject** _ppOut = nullptr);
	HRESULT					Ready_Layer_Monster_Projectile(const _wstring& _strLayerTag, CGameObject** _ppOut = nullptr);
	HRESULT					Ready_Layer_Effects(const _wstring& _strLayerTag);
	HRESULT					Ready_Layer_Effects2D(const _wstring& _strLayerTag);
	HRESULT					Ready_Layer_Domino(const _wstring& _strLayerTag);
	HRESULT					Ready_Layer_LunchBox(const _wstring& _strLayerTag);
	HRESULT					Ready_Layer_RayShape(const _wstring& _strLayerTag);
	HRESULT					Ready_Layer_Hand(const _wstring& _strLayerTag);
	HRESULT					Ready_Layer_Draggable(const _wstring& _strLayerTag);
	HRESULT					Ready_Layer_MapGimmick(const _wstring& _strLayerTag);
	HRESULT					Ready_Layer_RoomDoor(const _wstring& _strLayerTag);
private:
	void					Create_Arm(_uint _iCoordinateType, CGameObject* _pCamera, _float3 _vArm, _float _fLength);
 
	HRESULT					Map_Object_Create(_wstring _strFileName);

	LEVEL_ID				m_eLevelID;

public:
	static CLevel_Chapter_02* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, LEVEL_ID _eLevelID);
	virtual void			Free() override;
};

END

