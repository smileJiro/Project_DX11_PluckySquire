#pragma once
#include "Section_2D_PlayMap.h"
BEGIN(Client)
class CSection_2D_PlayMap_Sksp final : public CSection_2D_PlayMap
{
private:
	CSection_2D_PlayMap_Sksp(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CSection_2D_PlayMap_Sksp() = default;

public:
	HRESULT								Initialize(SECTION_2D_PLAYMAP_DESC* _pDesc, _uint _iPriorityKey);
	virtual HRESULT						Import(json _SectionJson, _uint _iPriorityKey) override;

public:
	virtual HRESULT						Add_GameObject_ToSectionLayer(CGameObject* _pGameObject, _uint _iLayerIndex) override;
	virtual HRESULT						Section_AddRenderGroup_Process() override;

public:
	static CSection_2D_PlayMap_Sksp* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, _uint _iPriorityKey, json _SectionJson);
	void Free() override;
};

END