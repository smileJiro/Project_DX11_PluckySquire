#pragma once
#include "Section_2D.h"
BEGIN(Client)
class CSection_2D_PlayMap abstract: public CSection_2D
{
public :
	enum SECTION_2D_PLAYMAP_STYLE
	{
		NONE = 0x00,
		COMBAT = 0x01,
		STORY = 0x02,
		EVENT = 0x04,
	};

public :
	typedef struct tagSection2DDesc : public CSection_2D::SECTION_2D_DESC
	{

	}SECTION_2D_PLAYMAP_DESC;

protected:
	CSection_2D_PlayMap(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, SECTION_2D_PLAY_TYPE _ePlayType, SECTION_2D_RENDER_TYPE _eRenderType);
	virtual ~CSection_2D_PlayMap() = default;


	virtual void						Set_WorldTexture(ID3D11Texture2D* _pTexture) override;

public:
	virtual HRESULT						Initialize(void* _pDesc);
	virtual HRESULT						Ready_Objects(void* _pDesc);

public:
	virtual HRESULT						Add_GameObject_ToSectionLayer(CGameObject* _pGameObject, _uint _iLayerIndex) override;
	virtual HRESULT						Section_AddRenderGroup_Process() override;

	virtual _uint						Get_MonsterCount() { return m_iMonsterCount; }
	virtual void						Add_MonsterCount() { m_iMonsterCount++; }
	virtual void						Reduce_MonsterCount() { m_iMonsterCount--; }



private :	
	_uint m_iMonsterCount = 0;

public:
	void Free() override;
};

END