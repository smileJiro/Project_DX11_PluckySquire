#pragma once

#include "Level.h"

BEGIN(Map_Tool)

class CImguiLogger;

class CLevel_3D_Map_Tool final : public CLevel
{
private:
	CLevel_3D_Map_Tool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CLevel_3D_Map_Tool() = default;

public:
	virtual HRESULT			Initialize(CImguiLogger* _pLogger);
	virtual void			Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;

public:
	HRESULT					Ready_Lights();
	HRESULT					Ready_Layer_Camera(const _wstring& _strLayerTag, CGameObject* _pTarget);
	HRESULT					Ready_Layer_Player(const _wstring& _strLayerTag, CGameObject** _ppOut);
	HRESULT					Ready_Layer_TestTerrain(const _wstring& _strLayerTag);
private :
	class C3DMap_Tool_Manager* m_pToolManager = { nullptr };
	CImguiLogger* m_pLogger = { nullptr };

public:

	static CLevel_3D_Map_Tool* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, CImguiLogger* _pLogger);
	virtual void			Free() override;
};

END

