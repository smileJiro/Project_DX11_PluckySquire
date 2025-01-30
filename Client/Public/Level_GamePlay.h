#pragma once
#include "Level.h"
BEGIN(Engine)
class CGameObject;
END

BEGIN(Client)
class CLevel_GamePlay final : public CLevel
{
private:
	CLevel_GamePlay(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CLevel_GamePlay() = default;

public:
	virtual HRESULT			Initialize() override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;

public:
	HRESULT					Ready_Lights();
	HRESULT					Ready_Layer_Camera(const _wstring& _strLayerTag, CGameObject* _pTarget);
	HRESULT					Ready_Layer_Player(const _wstring& _strLayerTag, CGameObject** _ppOut);
	HRESULT					Ready_Layer_TestTerrain(const _wstring& _strLayerTag);
	HRESULT					Ready_Layer_UI(const _wstring& _strLayerTag);
	HRESULT					Ready_Layer_NPC(const _wstring& _strLayerTag);
	HRESULT					Ready_Layer_Monster(const _wstring& _strLayerTag, CGameObject** _ppOut = nullptr);

private:
	void					Create_Arm();

public:
	static CLevel_GamePlay* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void			Free() override;
};

END

