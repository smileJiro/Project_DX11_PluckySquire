#pragma once

#include "Map_Tool_Defines.h"
#include "UIObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END


BEGIN(Map_Tool)

class C2DTile_RenderObject final : public CUIObject
{
public :
	typedef struct tagTileInfo
	{
		_uint iWidthSIze = 0;
		_uint iHeightSIze = 0;
		_uint iWidthCount = 0;
		_uint iHeightCount = 0;
		ID3D11ShaderResourceView* pTexture;
		_wstring	strTextureName;
	}TILE_INFO;
private:
	C2DTile_RenderObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	C2DTile_RenderObject(const C2DTile_RenderObject& Prototype);
	virtual ~C2DTile_RenderObject() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;


	HRESULT	Set_TileTexture(const TILE_INFO& _tTile_Info);
	HRESULT	Set_Index(_uint _iTileIndex, _int _iTextureIndex);

private:
	CShader* m_pShader = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };

	vector<_int> m_TextureIndexs;
	_uint m_iIndexCount = 0;
	
	_uint m_iWidthSIze = 0;
	_uint m_iHeightSIze = 0;
	_uint m_iWidthCount = 0;
	_uint m_iHeightCount = 0;


private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();




public:
	static C2DTile_RenderObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;



	// CUIObject을(를) 통해 상속됨
	virtual HRESULT Cleanup_DeadReferences() override;

};

END