#pragma once
#include "Section.h"
BEGIN(Client)
class CMap_2D;
class CSection_2D final : public CSection
{
private:
	CSection_2D();
	virtual ~CSection_2D() = default;

public:
	HRESULT Initialize() override;

public:	/* Section_2D 기능. */
	// 1. Map_2D에 있는 RTV에 자기 자신의 Map Texture를 Copy하는 기능.
	// 2. Section의 Map_2D에 있는 SRV를 return하는 기능. (혹은 직접 바인딩을 하는 기능.)

private:
	CMap_2D* m_pMap = nullptr;

private: /* Initialize Method */
	// 1. Map_2D 객체를 생성하고, 해당 객체의 SRV를 Target_Manager에 추가.
	HRESULT Register_RTV_ToTargetManager();
	// 2. SectionName과 동일한 RenderGroup Renderer에 추가.
	HRESULT Register_RenderGroup_ToRenderer();


public:
	static CSection_2D* Create();
	void Free() override;
};

END