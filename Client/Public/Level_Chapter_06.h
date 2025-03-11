#pragma once
#include "Level.h"
BEGIN(Engine)
class CGameObject;
END

BEGIN(Client)
class CSpawner;
class CCandle;
class CCandleGame;
class CLevel_Chapter_06 final : public CLevel
{
private:
	CLevel_Chapter_06(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CLevel_Chapter_06() = default;

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
	HRESULT					Ready_Layer_Book(const _wstring& _strLayerTag);
	HRESULT					Ready_Layer_UI(const _wstring& _strLayerTag);
	HRESULT					Ready_Layer_Item(const _wstring& _strLayerTag);
	HRESULT					Ready_Layer_NPC(const _wstring& _strLayerTag);
	HRESULT					Ready_Layer_Monster(const _wstring& _strLayerTag, CGameObject** _ppOut = nullptr);
	HRESULT					Ready_Layer_Monster_Projectile(const _wstring& _strLayerTag, CGameObject** _ppOut = nullptr);
	HRESULT					Ready_Layer_Effects(const _wstring& _strLayerTag);
	HRESULT					Ready_Layer_Effects2D(const _wstring& _strLayerTag);
	HRESULT					Ready_Layer_Slippery();
	HRESULT					Ready_Layer_Defender();
private:
	void					Create_Arm(_uint _iCoordinateType, CGameObject* _pCamera, _float3 _vArm, _float _fLength);
	// Chapter6 나가기 이벤트 처리 전에 해 놓은 코드 (0310, 효림)
	void					Create_IntroTrigger();


	HRESULT					Map_Object_Create(_wstring _strFileName);

	LEVEL_ID				m_eLevelID;

	CCandle*				m_pCandle = nullptr;
	CCandleGame*			m_pCandleGame = nullptr;
public:
	static CLevel_Chapter_06* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, LEVEL_ID _eLevelID);
	virtual void			Free() override;
};

END

