#include "Sound_Manager.h"
#include "Sound.h"
#include "GameInstance.h"

CSound_Manager::CSound_Manager()
    : m_pGameInstance(CGameInstance::GetInstance())
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CSound_Manager::Initialize(HWND hWnd)
{
    if (FAILED(DirectSoundCreate8(nullptr, &m_pSoundDevice, nullptr)))
        return E_FAIL;

    if (FAILED(m_pSoundDevice->SetCooperativeLevel(hWnd, DISCL_EXCLUSIVE)))
        return E_FAIL;

    return S_OK;
}

void CSound_Manager::Update(_float _fTimeDelta)
{
    //if (true == m_DelaySFXs.empty())
    //    return;

    auto iter = m_DelaySFXs.begin();

    for (; iter != m_DelaySFXs.end(); )
    {
        _bool isPlay = false;
        isPlay = iter->first->Update_DelayTime(_fTimeDelta);
        if (true == isPlay)
        {
            iter->first->Play_Sound(iter->second);
            iter->first->Set_Delay(false);
            Safe_Release(iter->first);
            iter = m_DelaySFXs.erase(iter);

            //if (true == m_DelaySFXs.empty())
            //    break;
        }
        else
        {
            ++iter;
        }
    }

    for (auto iter = m_UpdateSounds.begin(); iter != m_UpdateSounds.end(); )
    {
        _int iResult = (*iter)->Update_SoundVolume(_fTimeDelta);

        if (1 == iResult)
            iter = m_UpdateSounds.erase(iter);
        else if (2 == iResult)
        {
            (*iter)->Stop_Sound(true);
            iter = m_UpdateSounds.erase(iter);
        }
        else
            ++iter;

    }

}

void CSound_Manager::Start_BGM(const wstring& strBGMTag, _float _fVolume)
{
    CSound* pBGM = Find_BGM(strBGMTag);
    if (nullptr == pBGM)
        return;

    if (nullptr != m_pCurPlayBGM)
    {
        End_BGM();
    }

    m_pCurPlayBGM = pBGM;
    Safe_AddRef(m_pCurPlayBGM);
    m_pCurPlayBGM->Play_BGM(0.0f, true);
    m_pCurPlayBGM->Set_Volume(_fVolume); // bgm 사운드는 고정 30으로
}

void CSound_Manager::Stop_BGM()
{
    if (nullptr != m_pCurPlayBGM)
        m_pCurPlayBGM->Stop_Sound();
}

void CSound_Manager::End_BGM()
{
    if (nullptr != m_pCurPlayBGM)
    {
        m_pCurPlayBGM->Stop_Sound(true);
        Safe_Release(m_pCurPlayBGM);
        m_pCurPlayBGM = nullptr;
    }
}

void CSound_Manager::Set_BGMVolume(_float _fVolume)
{
    if (nullptr != m_pCurPlayBGM)
        m_pCurPlayBGM->Set_Volume(_fVolume);
}

void CSound_Manager::Set_SFXTargetVolume(const _wstring& _strSFXTag, _float _fTargetVolume, _float _fFactor)
{
    vector<CSound*>* pSFXs = Find_SFX(_strSFXTag);
    if (nullptr == pSFXs)
        return;

    for (auto& pSFX : *pSFXs)
    {
        pSFX->Set_TargetVolume(_fTargetVolume, _fFactor);

        m_UpdateSounds.push_back(pSFX);
    }
}

void CSound_Manager::Set_BGMTargetVolume(_float _fTargetVolume, _float _fFactor)
{
    if (nullptr != m_pCurPlayBGM)
    {
        m_pCurPlayBGM->Set_TargetVolume(_fTargetVolume, _fFactor);
        m_UpdateSounds.push_back(m_pCurPlayBGM);
    }
}

_float CSound_Manager::Get_BGMVolume()
{
    if (nullptr == m_pCurPlayBGM)
        return -1.0f;

    return  m_pCurPlayBGM->Get_Volume();
}

void CSound_Manager::Set_SFXVolume(const wstring& strSFXTag, _float _fVolume)
{
    vector<CSound*>* pSFXs = Find_SFX(strSFXTag);
    if (nullptr == pSFXs)
        return;

    for (auto& pSFX : *pSFXs)
    {
        pSFX->Set_Volume(_fVolume);
    }
}

HRESULT CSound_Manager::Clear_Sound()
{
    Safe_Release(m_pCurPlayBGM);
    m_pCurPlayBGM = nullptr;
    for (auto& Pair : m_DelaySFXs)
    {
        Safe_Release(Pair.first);
    }
    m_DelaySFXs.clear();


    for (auto& iter : m_BGMs)
    {
        Safe_Release(iter.second);
    }
    m_BGMs.clear();

    for (auto& iter : m_SFXs)
    {
        for (CSound* pSound : iter.second)
        {
            Safe_Release(pSound);
        }
        iter.second.clear();
    }
    m_SFXs.clear();

    m_UpdateSounds.clear();
    return S_OK;
}

_float CSound_Manager::Get_SFXVolume(const wstring& strSFXTag)
{
    vector<CSound*>* pSFXs = Find_SFX(strSFXTag);
    if (nullptr == pSFXs)
        return -1.0f;

    return (*pSFXs)[0]->Get_Volume();
}

void CSound_Manager::Stop_SFX(const wstring& strSFXTag)
{
    vector<CSound*>* pSFXs = Find_SFX(strSFXTag);
    if (nullptr == pSFXs)
        return;

    for (auto& pSFX : *pSFXs)
    {
        pSFX->Stop_Sound();
    }
    
}

void CSound_Manager::End_SFX(const wstring& strSFXTag)
{
    vector<CSound*>* pSFXs = Find_SFX(strSFXTag);
    if (nullptr == pSFXs)
        return;

    for (auto& pSFX : *pSFXs)
    {
        pSFX->Stop_Sound(true);
    }
}

void CSound_Manager::Start_SFX_Distance(const _wstring& strSFXTag, _fvector _vPosition, _float _fMaxVolume, _float _fMinVolume, _float _fMaxVolumeDist, _float _fFactor, _bool _isLoop)
{
    vector<CSound*>* pSFXs = Find_SFX(strSFXTag);
    if (nullptr == pSFXs)
        return;

    _float fVolume = _fMinVolume;
    _vector vCamposition = m_pGameInstance->Get_vCamPosition();

    _float fDist = XMVectorGetX(XMVector3LengthEst(_vPosition - vCamposition));

    if (_fMaxVolumeDist > fDist)
        fVolume = _fMaxVolume;
    else
        fVolume = std::fmaxf(-_fFactor * powf((fDist - _fMaxVolumeDist), 2.f) + _fMaxVolume, _fMinVolume);

    // 그냥 실행 ㄴㄴ.
    if (0.1f >= fVolume)
        return;

    for (auto& pSFX : *pSFXs)
    {
        if (false == pSFX->Is_Playing())
        {
            pSFX->Stop_Sound(true);
            pSFX->Play_Sound(_isLoop);
            pSFX->Set_Volume(fVolume);

            return;
        }
    }

    if (FAILED(Pulling_SFX(5, strSFXTag, pSFXs)))
        assert(nullptr);

    Start_SFX(strSFXTag, fVolume, _isLoop);
}

void CSound_Manager::Start_SFX_Distance_Delay(const _wstring& strSFXTag, _fvector _vPosition, _float _fDelayTime, _float _fMaxVolume, _float _fMinVolume, _float _fMaxVolumeDist, _float _fFactor, _bool _isLoop)
{
    vector<CSound*>* pSFXs = Find_SFX(strSFXTag);
    if (nullptr == pSFXs)
        return;

    _float fVolume = _fMinVolume;
    _vector vCamposition = m_pGameInstance->Get_vCamPosition();

    _float fDist = XMVectorGetX(XMVector3LengthEst(_vPosition - vCamposition));

    if (_fMaxVolumeDist > fDist)
        fVolume = _fMaxVolume;
    else
        fVolume = std::fmaxf(-_fFactor * powf((fDist - _fMaxVolumeDist), 2.f) + _fMaxVolume, _fMinVolume);

    // 그냥 실행 ㄴㄴ.
    if (0.1f >= fVolume)
        return;

    for (auto& pSFX : *pSFXs)
    {
        if (false == pSFX->Is_Playing() && false == pSFX->Is_Delay())
        {
            pSFX->Stop_Sound(true);
            pSFX->Set_Delay(true);
            pSFX->Set_DelayTime(_fDelayTime);
            pSFX->Set_Volume(fVolume);

            m_DelaySFXs.push_back(make_pair(pSFX, _isLoop));
            Safe_AddRef(pSFX);
            return;
        }
    }

    if (FAILED(Pulling_SFX(5, strSFXTag, pSFXs)))
        assert(nullptr);

    Start_SFX_Delay(strSFXTag, _fDelayTime, fVolume, _isLoop);
}


HRESULT CSound_Manager::Pulling_SFX(_int _iNumPullings, const wstring& strSFXTag, vector<CSound*>* _pPullingSFXs)
{
    if (nullptr == _pPullingSFXs)
        return E_FAIL;


    _wstring strFilePath = (*_pPullingSFXs)[0]->Get_SoundFilePath();
    for (int i = 0; i < _iNumPullings; ++i)
    {
        CSound* pSFX = CSound::Create(strFilePath, m_pSoundDevice);
        if (nullptr == pSFX)
            return E_FAIL;

        _pPullingSFXs->push_back(pSFX);
    }


    return S_OK;
}

void CSound_Manager::Start_SFX(const wstring& strSFXTag, _float _fVolume, _bool _isLoop)
{
    vector<CSound*>* pSFXs = Find_SFX(strSFXTag);
    if (nullptr == pSFXs)
        return;

    for (auto& pSFX : *pSFXs)
    {
        if (false == pSFX->Is_Playing())
        {
            pSFX->Stop_Sound(true);
            pSFX->Play_Sound(_isLoop);
            pSFX->Set_Volume(_fVolume);
            return;
        }
    }

    if (FAILED(Pulling_SFX(5, strSFXTag, pSFXs)))
        assert(nullptr);

    Start_SFX(strSFXTag, _fVolume, _isLoop);
}

void CSound_Manager::Start_SFX_Delay(const wstring& strSFXTag, _float _fDelayTime, _float _fVolume, _bool _isLoop)
{
    vector<CSound*>* pSFXs = Find_SFX(strSFXTag);
    if (nullptr == pSFXs)
        return;

    for (auto& pSFX : *pSFXs)
    {
        if (false == pSFX->Is_Playing() && false == pSFX->Is_Delay())
        {
            pSFX->Stop_Sound(true);
            pSFX->Set_Delay(true);
            pSFX->Set_DelayTime(_fDelayTime);
            pSFX->Set_Volume(_fVolume);

            m_DelaySFXs.push_back(make_pair(pSFX, _isLoop));
            Safe_AddRef(pSFX);
            return;
        }
    }

    if (FAILED(Pulling_SFX(5, strSFXTag, pSFXs)))
        assert(nullptr);

    Start_SFX_Delay(strSFXTag, _fDelayTime, _fVolume, _isLoop);
}

CSound* CSound_Manager::Find_BGM(const wstring& strBGMTag)
{
    auto iter = m_BGMs.find(strBGMTag);

    if (iter == m_BGMs.end())
        return nullptr;

    return iter->second;
}

vector <CSound*>* CSound_Manager::Find_SFX(const wstring& strSFXTag)
{
    auto iter = m_SFXs.find(strSFXTag);

    if (iter == m_SFXs.end())
        return nullptr;

    return &(iter->second);
}

HRESULT CSound_Manager::Load_BGM(const wstring& strBGMTag, const wstring& strBGMFilePath)
{
    CSound* pBGM = CSound::Create(strBGMFilePath, m_pSoundDevice);
    if (nullptr == pBGM)
        return E_FAIL;

    if (nullptr != Find_BGM(strBGMTag))
    {
        Safe_Release(pBGM);
        return E_FAIL;
    }

    m_BGMs.emplace(strBGMTag, pBGM);
    return S_OK;
}

HRESULT CSound_Manager::Load_SFX(const wstring& strSFXTag, const wstring& strSFXFilePath)
{
    CSound* pSFX = CSound::Create(strSFXFilePath, m_pSoundDevice);
    if (nullptr == pSFX)
        return E_FAIL;

    if (nullptr != Find_SFX(strSFXTag))
    {
        Safe_Release(pSFX);
        return E_FAIL;
    }

    vector<CSound*> PullingSFXs;
    PullingSFXs.push_back(pSFX);

    m_SFXs.emplace(strSFXTag, PullingSFXs);
    return S_OK;
}

_float CSound_Manager::Get_SFXTrackPosition(const wstring& strSFXTag)
{
    vector<CSound*>* pSFXs = Find_SFX(strSFXTag);
    if (nullptr == pSFXs)
        return -1.0f;

    return (*pSFXs)[0]->Get_TrackPostion();
}

_bool CSound_Manager::Is_SFXPlaying(const wstring& strSFXTag)
{
    vector<CSound*>* pSFXs = Find_SFX(strSFXTag);
    if (nullptr == pSFXs)
        return false;

    for (auto& pSFX : *pSFXs)
    {
        if (true == pSFX->Is_Playing())
            return true;
    }

    return false;
}

_float CSound_Manager::Get_BGMTrackPosition(const wstring& strBGMTag)
{
    auto iter = m_BGMs.find(strBGMTag);
    if (iter == m_BGMs.end())
        return -1.0f;

    return iter->second->Get_TrackPostion();
}

_bool CSound_Manager::Is_BGMPlaying(const wstring& strBGMTag)
{
    auto iter = m_BGMs.find(strBGMTag);
    if (iter == m_BGMs.end())
        return false;

    return iter->second->Is_Playing();
}

CSound_Manager* CSound_Manager::Create(HWND _hWnd)
{
    CSound_Manager* pInstance = new CSound_Manager();

    if (FAILED(pInstance->Initialize(_hWnd)))
    {
        MSG_BOX("Failed to Created : CSound_Manager");
        Safe_Release(pInstance);
    }

    return pInstance;
}




void CSound_Manager::Free()
{
    Clear_Sound();
    Safe_Release(m_pSoundDevice);
    Safe_Release(m_pGameInstance);
}
