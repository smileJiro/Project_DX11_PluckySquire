#pragma once
#include "Level.h"

/* 로딩 화면을 구성하기 위한 객체(배경, 로딩 바 , 폰트 등)를 생성*/
/* 다음 레벨에 필요한 자원을 로드하는 역할을 하는 Loader 객체를 생성*/


BEGIN(Client)

class CLoader;
class CUI_Icon;
class CLevel_Loading final : public CLevel
{
private:
	CLevel_Loading(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CLevel_Loading() = default;

public:
	virtual HRESULT Initialize(LEVEL_ID _eNextLevelID);
	virtual void Update(_float _fTimeDelta) override; 
	virtual HRESULT Render() override;

private:
	LEVEL_ID m_eNextLevelID = LEVEL_END;
	CLoader* m_pLoader = nullptr;

	class CLogo_BackGround* m_pBackGround = { nullptr };

private:
	HRESULT Ready_Layer_BackGround(const _wstring& strLayerTag);

public:
	static CLevel_Loading* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, LEVEL_ID _eNextLevelID);
	virtual void Free() override;
};

END

