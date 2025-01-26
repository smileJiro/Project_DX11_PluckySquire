#pragma once
#include "Component.h"
BEGIN(Engine)
class CShader;
class CAnimation;
class CAnimEventGenerator;
class ENGINE_DLL CModel :
    public CComponent
{
public:
	enum ANIM_TYPE { NONANIM, ANIM, LAST, };
protected:
	CModel(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CModel(const CModel& _Prototype);
	virtual ~CModel() = default;
public:
	virtual HRESULT			Render(CShader* _Shader, _uint _iShaderPass)abstract;
	virtual _bool Play_Animation(_float fTimeDelta) {return false;}
	virtual void To_NextAnimation()abstract;

	virtual void Set_AnimationLoop(_uint iIdx, _bool bIsLoop) abstract;
	virtual void Set_Animation(_uint iIdx) abstract;
	virtual void Switch_Animation(_uint iIdx) abstract;
	void Set_PlayingAnim(_bool bPlaying) { m_bPlayingAnim = bPlaying; }
	void Set_AnimEventGenerator(CAnimEventGenerator* _pAnimEventGenerator);

	virtual _uint Get_AnimCount() abstract;
	virtual _uint Get_CurrentAnimIndex() abstract;
	virtual _float Get_CurrentAnimProgeress() abstract;
	virtual CAnimation* Get_Animation(_uint iAnimIndex) abstract;
	ANIM_TYPE Get_AnimType() { return m_eAnimType; }

	_bool Is_AnimModel() { return m_eAnimType == ANIM_TYPE::ANIM; };
protected:
	ANIM_TYPE				m_eAnimType = ANIM_TYPE::LAST;
	_bool m_bPlayingAnim = true;

public:
	virtual void Free() override;
};

NLOHMANN_JSON_SERIALIZE_ENUM(CModel::ANIM_TYPE, {
	{CModel::ANIM_TYPE::NONANIM, "NONANIM"},
	{CModel::ANIM_TYPE::ANIM, "ANIM"},
	});
END