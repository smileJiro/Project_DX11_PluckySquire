#pragma once
#include "Level.h"
#include "EffectTool_Defines.h"

BEGIN(EffectTool)

class CLevel_EffectTool : public CLevel
{
private:
	CLevel_EffectTool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CLevel_EffectTool() = default;

public:
	virtual HRESULT			Initialize() override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;

public:
	HRESULT					Ready_Lights();
	HRESULT					Ready_Layer_Camera(const _wstring& _strLayerTag);
	HRESULT					Ready_Layer_TestTerrain(const _wstring& _strLayerTag);

public:
	static CLevel_EffectTool* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void			Free() override;
};

END