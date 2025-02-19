#pragma once
#include "Section_2D.h"
BEGIN(Client)

class CSection_2D_Narration final : public CSection_2D
{
public :
	typedef struct tagSection2DDesc : public CSection_2D::SECTION_2D_DESC
	{
		_wstring				strTextureName;
	}SECTION_2D_NARRATION_DESC;

private:
	CSection_2D_Narration(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CSection_2D_Narration() = default;

public:
	HRESULT						Initialize(SECTION_2D_DESC* _pDesc, _uint _iPriorityKey);
	HRESULT						Import(json _SectionJson, _uint _iPriorityKey);
public:

	virtual HRESULT				Section_AddRenderGroup_Process() override;
	virtual HRESULT				Add_GameObject_ToSectionLayer(CGameObject* _pGameObject, _uint _iLayerIndex) override;
	virtual HRESULT				Remove_GameObject_ToSectionLayer(CGameObject* _pGameObject) override;


	// 맵 연결 생각안해놨는데, 일단 해보자.

public:
	static CSection_2D_Narration* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, _uint _iPriorityKey, SECTION_2D_DESC* _pDesc);
	static CSection_2D_Narration* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, _uint _iPriorityKey, json _SectionJson);
	void Free() override;
};

END