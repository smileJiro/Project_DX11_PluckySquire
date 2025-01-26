#pragma once
#include "Section.h"
BEGIN(Client)
class CMap_2D;
class CSection_2D final : public CSection
{
private:
	CSection_2D(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CSection_2D() = default;

public:
	HRESULT Initialize();

public:	/* Section_2D 기능. */
	// 1. Map_2D에 있는 RTV에 자기 자신의 Map Texture를 Copy하는 기능.
	// 2. Section의 Map_2D에 있는 SRV를 return하는 기능. (혹은 직접 바인딩을 하는 기능.)

public:
	HRESULT Register_RTV_ToTargetManager();
	HRESULT Register_RenderGroup_ToRenderer();

private:
	CMap_2D* m_pMap = nullptr;

private:
	HRESULT Ready_Map_2D();

public:
	static CSection_2D* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	void Free() override;
};

END