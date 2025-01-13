#pragma once
#include "Base.h"

BEGIN(Engine)
class CSound;

class CSound_Manager final : public CBase
{
public:
	enum SFX_TYPE { SFX_TEST, SFX_LAST };
	enum BGM_TYPE { BGM_TEST, BGM_LAST };
private:
	CSound_Manager();
	virtual ~CSound_Manager() = default;


public:
	HRESULT			Initialize(HWND hWnd);
	void			Update(_float _fTimeDelta);
public:
	LPDIRECTSOUND8	Get_SoundDevice() const { return m_pSoundDevice; }

public:
	HRESULT			Load_BGM(const wstring& strBGMTag, const wstring& strBGMFilePath);
	void			Start_BGM(const wstring& strBGMTag, _float _fVolume = 20.f);
	void			Stop_BGM(); // 일시정지
	void			End_BGM(); // 완전 재생 종료

	_float			Get_BGMTrackPosition(const wstring& strBGMTag);
	_bool			Is_BGMPlaying(const wstring& strBGMTag);
	_float			Get_BGMVolume();

	void			Set_BGMVolume(_float _fVolume);

public:
	HRESULT			Load_SFX(const wstring& strSFXTag, const wstring& strSFXFilePath);
	void			Start_SFX(const wstring& strSFXTag, _float _fVolume = 70.f, _bool _isLoop = false);
	void			Start_SFX_Delay(const wstring& strSFXTag, _float _fDelayTime, _float _fVolume = 70.f, _bool _isLoop = false);
	void			Stop_SFX(const wstring& strSFXTag); // 일시정지 
	void			End_SFX(const wstring& strSFXTag); // 완전 재생 종료
	HRESULT			Pulling_SFX(_int _iNumPullings, const wstring& strSFXTag, vector<CSound*>* _pPullingSFXs);
	_float			Get_SFXTrackPosition(const wstring& strSFXTag);
	_bool			Is_SFXPlaying(const wstring& strSFXTag);
	_float			Get_SFXVolume(const wstring& strSFXTag);

	void			Set_SFXVolume(const wstring& strSFXTag, _float _fVolume);

private:
	LPDIRECTSOUND8	m_pSoundDevice = nullptr;	// 사운드카드 대표 객체
	map<_wstring, CSound*> m_BGMs;
	map<_wstring, vector<CSound*>> m_SFXs;

	vector<pair<CSound*, _bool>> m_DelaySFXs;

	CSound* m_pCurPlayBGM = nullptr;

private:
	CSound* Find_BGM(const wstring& strBGMTag);
	vector <CSound*>* Find_SFX(const wstring& strSFXTag);

public:
	static CSound_Manager* Create(HWND _hWnd);
	virtual void Free();
};
END
