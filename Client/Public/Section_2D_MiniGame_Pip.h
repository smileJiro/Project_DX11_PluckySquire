#pragma once
#include "Section_2D_MiniGame.h"

BEGIN(Client)
class CSneak_Tile;
class CSection_2D_MiniGame_Pip final : public CSection_2D_MiniGame
{
public:
	enum SECTION_PIP_RENDERGROUP
	{
		SECTION_PIP_TILE,
		SECTION_PIP_MAPOBJECT,
		SECTION_PIP_MAPOBJECT_2,
		SECTION_PIP_MOVEOBJECT,

	};

public:
	typedef struct tagSection2DDesc : public CSection_2D_MiniGame::SECTION_2D_MINIGAME_DESC
	{
	}SECTION_2D_MINIGAME_PIP_DESC;

private:
	CSection_2D_MiniGame_Pip(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CSection_2D_MiniGame_Pip() = default;


public:
	virtual HRESULT						Initialize(void* _pDesc);
	virtual HRESULT						Ready_Objects(void* _pDesc) override;

public:
	virtual HRESULT						Add_GameObject_ToSectionLayer(CGameObject* _pGameObject, _uint _iLayerIndex) override;
	virtual HRESULT						Section_AddRenderGroup_Process() override;


public:
	static CSection_2D_MiniGame_Pip* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* _pDesc);
	void Free() override;
};

END