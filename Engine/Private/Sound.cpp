#include "Sound.h"

CSound::CSound()
{
}

HRESULT CSound::Initialize(const _wstring& _strFilePath, LPDIRECTSOUND8 _pSoundDevice)
{
    if (nullptr == _pSoundDevice)
        return E_FAIL;

    // 확장자 이름 구별하기
    wchar_t szExt[10] = { 0 };
    _wsplitpath_s(_strFilePath.c_str(), nullptr, 0, nullptr, 0, nullptr, 0, szExt, 10);

    if (!wcscmp(szExt, L".wav")) // WAV 파일 로드
    {
        if (FAILED(Load_WaveSound(_strFilePath, _pSoundDevice)))
            return E_FAIL;
    }
    else
        return E_FAIL; // WAV 확장자만 받는다 일단은, 추후 다른 확장자 사용하고싶다면, 여기서 추가하면 돼.


    m_strRelativePath = _strFilePath;

    return S_OK;
}

_bool CSound::Update_DelayTime(_float _fTimeDelta)
{
    m_vDelayTime.y += _fTimeDelta;
    if (m_vDelayTime.x <= m_vDelayTime.y)
    {
        m_vDelayTime.y = 0.0f;
        return true;
    }
        
    return false;
}

void CSound::Play_Sound(_bool _isLoop)
{
    m_vDelayTime.y = 0.0f; // 혹시 모르니

    m_pSoundBuffer->SetCurrentPosition(0);
    m_pSoundBuffer->Stop();
    if (_isLoop)
        m_pSoundBuffer->Play(0, 0, DSBPLAY_LOOPING);
    else
        m_pSoundBuffer->Play(0, 0, 0);

    /*m_pSoundBuffer->GetStatus(&status);
        if(status != DSBSTATUS_PLAYING)*/ // 재생중이 아닐 때 체크하고 재생하는 재생하는 코드

}

void CSound::Play_BGM(_float _fStartTrackPosition, _bool _isLoop)
{
    Set_TrackPosition(_fStartTrackPosition);

    if (_isLoop)
        m_pSoundBuffer->Play(0, 0, DSBPLAY_LOOPING);
    else
        m_pSoundBuffer->Play(0, 0, 0);
}

void CSound::Stop_Sound(_bool _isReset)
{
    m_pSoundBuffer->Stop();

    if (_isReset)
        m_pSoundBuffer->SetCurrentPosition(0);
}

void CSound::Set_Volume(_float _fVolume)
{
    m_iDecibel = Convert_VolumeToDecibel(_fVolume);

    m_pSoundBuffer->SetVolume(m_iDecibel);
}

_float CSound::Get_TrackPostion()
{
    DWORD dwPlayPosition = 0;
    DWORD dwWritePosition = 0;
    m_pSoundBuffer->GetCurrentPosition(&dwPlayPosition, &dwWritePosition);
    _float fTrackPosition = (_float(dwPlayPosition) / (_float)m_tBuffInfo.dwBufferBytes) * 100.0f;

    return fTrackPosition;
}

_bool CSound::Is_Playing()
{
    DWORD pStatus;
    m_pSoundBuffer->GetStatus(&pStatus);

    return pStatus & DSBSTATUS_PLAYING;
}

void CSound::Set_TrackPosition(_float _fTrackPosition)
{
    Stop_Sound(true);

    DWORD dwBytes = (DWORD)((_fTrackPosition / 100.f) * (float)m_tBuffInfo.dwBufferBytes);

    if(FAILED(m_pSoundBuffer->SetCurrentPosition(dwBytes)))
        assert(nullptr);
}

HRESULT CSound::Load_WaveSound(const _wstring& _strSoundFilePath, LPDIRECTSOUND8 _pSoundDevice)
{
    HMMIO	hFile; // File Handle

    wstring strFilePath = _strSoundFilePath;

    //CreateFile
    hFile = mmioOpen((wchar_t*)_strSoundFilePath.c_str(), NULL, MMIO_READ);//wave파일을 연다.

    if (nullptr == hFile)
    {
        MessageBox(NULL, L"사운드 리소스 경로에 파일 없음", L"사운드 로딩 실패", MB_OK);
        return E_FAIL;
    }

    //Chunk 청크 구조체, 문자열로 색인을 인식해서 WaveFormat 및 버퍼선언정보를 읽어온다.
    MMCKINFO	pParent;
    memset(&pParent, 0, sizeof(pParent));
    pParent.fccType = mmioFOURCC('W', 'A', 'V', 'E');
    mmioDescend(hFile, &pParent, NULL, MMIO_FINDRIFF);

    MMCKINFO	pChild;
    memset(&pChild, 0, sizeof(pChild));
    pChild.ckid = mmioFOURCC('f', 'm', 't', ' ');
    mmioDescend(hFile, &pChild, &pParent, MMIO_FINDCHUNK);

    WAVEFORMATEX	wft;
    memset(&wft, 0, sizeof(wft));
    mmioRead(hFile, (char*)&wft, sizeof(wft));

    mmioAscend(hFile, &pChild, 0);
    pChild.ckid = mmioFOURCC('d', 'a', 't', 'a');
    mmioDescend(hFile, &pChild, &pParent, MMIO_FINDCHUNK);



    memset(&m_tBuffInfo, 0, sizeof(DSBUFFERDESC));
    m_tBuffInfo.dwBufferBytes = pChild.cksize;
    m_tBuffInfo.dwSize = sizeof(DSBUFFERDESC);
    m_tBuffInfo.dwFlags = DSBCAPS_STATIC | DSBCAPS_LOCSOFTWARE | DSBCAPS_CTRLVOLUME;
    m_tBuffInfo.lpwfxFormat = &wft;


    if (FAILED(_pSoundDevice->CreateSoundBuffer(&m_tBuffInfo, &m_pSoundBuffer, NULL)))
        return E_FAIL;

    void* pWrite1 = NULL;
    void* pWrite2 = NULL;
    DWORD dwlength1, dwlength2;

    m_pSoundBuffer->Lock(0, pChild.cksize, &pWrite1, &dwlength1
        , &pWrite2, &dwlength2, 0L);

    if (pWrite1 != nullptr)
        mmioRead(hFile, (char*)pWrite1, dwlength1);
    if (pWrite2 != nullptr)
        mmioRead(hFile, (char*)pWrite2, dwlength2);

    m_pSoundBuffer->Unlock(pWrite1, dwlength1, pWrite2, dwlength2);

    mmioClose(hFile, 0);

    // 초기 음량 절반으로 설정
    Set_Volume(50.f);

    return S_OK;
}

_int CSound::Convert_VolumeToDecibel(_float _fVolume)
{
    if (_fVolume > 100.f)
        _fVolume = 100.f;
    else if (_fVolume <= 0.f)
        _fVolume = 0.00001f;

    // 1 ~ 100 사이값을 데시벨 단위로 변경
    int iVolume = (LONG)(-2000.0 * log10(100.f / _fVolume));

    if (iVolume < -10000)
        iVolume = -10000;

    return  iVolume;
}

CSound* CSound::Create(const _wstring& _strSoundFilePath, LPDIRECTSOUND8 _pSoundDevice)
{
    CSound* pInstance = new CSound();

    if (FAILED(pInstance->Initialize(_strSoundFilePath, _pSoundDevice)))
    {
        MSG_BOX("Sound Create Failed");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CSound::Free()
{
    Safe_Release(m_pSoundBuffer);
}
