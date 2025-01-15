#pragma once
#include "Level.h"
#include "EffectTool_Defines.h"

BEGIN(EffectTool)

class CLevel_EffectLoading final : public CLevel
{
private:
	CLevel_EffectLoading(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CLevel_EffectLoading() = default;

public:
	virtual HRESULT Initialize(LEVEL_ID _eNextLevelID);
	virtual void Update(_float _fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	LEVEL_ID m_eNextLevelID = LEVEL_END;
	class	CEffectToolLoader* m_pLoader = nullptr;

public:
	static CLevel_EffectLoading* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, LEVEL_ID _eNextLevelID);
	virtual void Free() override;
};

END
