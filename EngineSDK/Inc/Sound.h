#pragma once
#include "Base.h"

BEGIN(Engine)
class ENGINE_DLL CSound final : public CBase
{
private:
	CSound();
	virtual ~CSound() = default;

public:
	HRESULT Initialize(const _wstring& _strFilePath, LPDIRECTSOUND8 _pSoundDevice);

	_bool Update_DelayTime(_float _fTimeDelta);
public:
	void Play_Sound(_bool _isLoop = false);
	void Play_BGM(_float _fStartTrackPosition, _bool _isLoop = false);
	void Stop_Sound(_bool _isReset = false);

public: /* 볼륨 범위(0 ~100) */
	void Set_Volume(_float _fVolume);
	_float Get_Volume() const { return m_fVolume; }

public:
	// Get
	_float Get_TrackPostion();
	const _wstring& Get_SoundTag() { return m_strSoundTag; }
	const _wstring& Get_SoundFilePath() { return m_strRelativePath; }
	_bool Is_Playing();
	_bool Is_Delay() const { return m_isDelay; }
	// Set
	void Set_TrackPosition(_float _fTrackPosition);
	void Set_SoundTag(const _wstring& _strSoundTag) { m_strSoundTag = _strSoundTag; }
	void Set_SoundFilePath(const _wstring& _strRelativePath) { m_strRelativePath = _strRelativePath; }
	void Set_DelayTime(_float _fDelayTime) { m_vDelayTime.x = _fDelayTime; }
	void Set_Delay(_bool _isDelay) { m_isDelay = _isDelay; }
private:
	LPDIRECTSOUNDBUFFER		m_pSoundBuffer = nullptr;
	DSBUFFERDESC			m_tBuffInfo = {};
	_float					m_fVolume = 0.0f;
	int						m_iDecibel = 0;

	wstring					m_strSoundTag; // 리소스 키
	wstring					m_strRelativePath; // 리소스 상대경로

private:
	_float2					m_vDelayTime = { 0.0f, 0.0f };

	_bool					m_isDelay = false;
private:
	HRESULT Load_WaveSound(const _wstring& _strSoundFilePath, LPDIRECTSOUND8 _pSoundDevice);
	_int Convert_VolumeToDecibel(_float _fVolume);

public:
	static CSound* Create(const _wstring& _strSoundFilePath, LPDIRECTSOUND8 _pSoundDevice);
	virtual void Free() override;
};
END
