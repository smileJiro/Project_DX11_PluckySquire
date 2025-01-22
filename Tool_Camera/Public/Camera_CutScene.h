#pragma once

#include "Camera_Tool_Defines.h"
#include "Camera.h"

BEGIN(Engine)
class CCutScene_Sector;
END

BEGIN(Camera_Tool)

class CCamera_CutScene final : public CCamera
{
private:
	CCamera_CutScene(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CCamera_CutScene(const CCamera_CutScene& _Prototype);
	virtual ~CCamera_CutScene() = default;

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize(void* _pArg) override;
	virtual void				Priority_Update(_float _fTimeDelta) override;
	virtual void				Update(_float _fTimeDelta) override;
	virtual void				Late_Update(_float _fTimeDelta) override;

public:
	void						Set_NextCutScene(_wstring _wszCutSceneName);
	void						Add_CutScene(_wstring _wszCutSceneTag, vector<CCutScene_Sector*> _vecCutScene);

private:
	map<_wstring, vector<CCutScene_Sector*>>	m_CutScenes;
	vector<CCutScene_Sector*>*					m_pCurCutScene;

	_uint										m_iCurSectorNum = {};
	_uint										m_iCurSectorCount = {};

	_bool										m_isStartCutScene = { false };
	_bool										m_isFinishCutScene = { false };

private:
	void						Play_CutScene(_float _fTimeDelta);
	void						Change_Sector();

	vector<CCutScene_Sector*>*	Find_CutScene(_wstring _wszCutSceneName);

	void						Before_CutScene(_float _fTimeDelta);
	void						After_CutScene(_float _fTimeDelta);

public:
	static CCamera_CutScene*	Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*		Clone(void* _pArg);
	virtual void				Free() override;
};

END