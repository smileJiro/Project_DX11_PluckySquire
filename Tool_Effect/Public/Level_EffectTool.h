#pragma once
#include "Level.h"
#include "EffectTool_Defines.h"

BEGIN(Engine)
class CParticle_System;
class CTexture;
END

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
	HRESULT					Ready_Layer_Effect(const _wstring& _strLayerTag);
	HRESULT					Ready_Layer_TestTerrain(const _wstring& _strLayerTag);
	HRESULT					Ready_SkyBox(const _wstring& _strLayerTag);

private:
	class CParticle_System*							m_pNowItem = { nullptr };
	vector<class CParticle_System*>					m_ParticleSystems;

private:
	_char							m_szTexturePath[MAX_PATH] = "../Bin/Resources/Textures/Effects/";
	
	class CTexture*					m_pNowTexture = { nullptr };

	map<const _wstring, class CTexture*>			m_Textures;


private:
	void					Update_Particle_Tool(_float _fTimeDelta);
	void					Tool_System_List();
	void					Tool_Adjust_System(_float _fTimeDelta);
	void					Tool_Texture();

	HRESULT					Load_Textures(const _char* _szExtension);
	HRESULT					Save_All();
	HRESULT					Load_All(const _char* _szPath);

public:
	static CLevel_EffectTool* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void			Free() override;
};

END