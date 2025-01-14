#include "Channel.h"
#include "Model.h"
#include "Bone.h"

CChannel::CChannel()
{
}

HRESULT CChannel::Initialize(const aiNodeAnim* _pAIChannel, const CModel* _pModel)
{
    /* 1. 현재 해당되는 뼈의 이름을 가져온다. >>> 이 뼈의 이름은 우리가 아는 CBone의 뼈 이름과 동기화 되어있다.*/
    strcpy_s(m_szName, _pAIChannel->mNodeName.data);

    /* 2. 현재 해당 되는 뼈(Channel)의 인덱스를 받아온다. */
    m_iBoneIndex = _pModel->Find_BoneIndex(m_szName);

    /* 3. 크기, 회전, 이동에 대한 KeyFrame들이 각각 있다. >>> 이들 중 가장 개수가 많은 애를 기준으로 m_iNumKeyFrame을 채운다. */
    m_iNumKeyFrames = max(_pAIChannel->mNumScalingKeys, _pAIChannel->mNumRotationKeys);

    m_iNumKeyFrames = max(m_iNumKeyFrames, _pAIChannel->mNumPositionKeys);
    /* 변수들을 루프 밖에 둔 이유 : 만약 크, 자, 이 중 특정 값이 없는경우, 이전에 저장되어있던 값을 그대로 쓰기위해. */
    _float3 vScale{}, vPosition{};
    _float4 vRotation{};

    /* 4. 루프를 돌며, 해당 뼈(Channel)에 저장된 해당 애니메이션 상에서의 모든 KeyFrame 정보를 Assimp를 통해 가져와서 저장한다. */
    for (_uint i = 0; i < m_iNumKeyFrames; ++i)
    {
        KEYFRAME KeyFrame = {};
        if (i < _pAIChannel->mNumScalingKeys)
        {
            memcpy(&vScale, &_pAIChannel->mScalingKeys[i].mValue, sizeof(_float3));
            /* 추가 : 현재 KeyFrame의 Track 상의 위치정보도 받아오자. 크, 자, 이 중 키프레임 데이터가 없는경우가 있으니까, 세곳에서 일단 다받는거임. 하나만 걸려라. */
            KeyFrame.fTrackPosition = (_float)_pAIChannel->mScalingKeys[i].mTime;
        }
        if (i < _pAIChannel->mNumRotationKeys)
        {
            /* 사원수를 이용하기 위해 float4타입의 데이터를 받는데, w,z,y,x 순으로 데이터가 저장되어있음. 그래서 멤카피 불가 */
            vRotation.x = (_float)_pAIChannel->mRotationKeys[i].mValue.x;
            vRotation.y = (_float)_pAIChannel->mRotationKeys[i].mValue.y;
            vRotation.z = (_float)_pAIChannel->mRotationKeys[i].mValue.z;
            vRotation.w = (_float)_pAIChannel->mRotationKeys[i].mValue.w;
            KeyFrame.fTrackPosition = (_float)_pAIChannel->mRotationKeys[i].mTime;
        }
        if (i < _pAIChannel->mNumPositionKeys)
        {
            memcpy(&vPosition, &_pAIChannel->mPositionKeys[i].mValue, sizeof(_float3));
            KeyFrame.fTrackPosition = (_float)_pAIChannel->mPositionKeys[i].mTime;
        }

        KeyFrame.vScale = vScale;
        KeyFrame.vRotation = vRotation;
        KeyFrame.vPosition = vPosition;

        m_KeyFrames.push_back(KeyFrame);
    }
    return S_OK;
}

HRESULT CChannel::Initialize(const FBX_CHANNEL& _ChannelDesc)
{
    strcpy_s(m_szName, _ChannelDesc.szName);

    m_iBoneIndex = _ChannelDesc.iBoneIndex;

    m_iNumKeyFrames = _ChannelDesc.iNumKeyFrames;

    m_KeyFrames.resize(m_iNumKeyFrames);
    for (_uint i = 0; i < m_iNumKeyFrames; ++i)
    {
        m_KeyFrames[i].vScale = _ChannelDesc.vecKeyFrames[i].vScale;
        m_KeyFrames[i].vRotation = _ChannelDesc.vecKeyFrames[i].vRotation;
        m_KeyFrames[i].vPosition = _ChannelDesc.vecKeyFrames[i].vPosition;
        m_KeyFrames[i].fTrackPosition = _ChannelDesc.vecKeyFrames[i].fTrackPosition;
    }
    
    return S_OK;
}
                                                                     
void CChannel::Update_TransformationMatrix(_float _fCurTrackPosition, _uint* _pKeyFrameIndex,  const vector<CBone*>& _vecBones)
{
    // fCurTrackPosition이 정말 정확하게 0.0f이 되는상황은 사실 상 이전 함수에 fCurTrackPosition을 0으로 초기화시킨 그 프레임뿐이다.
    if (0.0f == _fCurTrackPosition)
        *_pKeyFrameIndex = 0;

    /* KeyFrame의 값과, 매개변수로 들어오는 CurTrackPosition(TickPerSec * TimeDelta의 누적값) 
       을 기준으로 크, 자, 이 선형보간 하고 그 데이터로 TransformationMatrix 만들어서 CBone의 Transformation 행렬 바꿔주기. */

    /* 가장 마지막 키프레임의 데이터를 받아오기 위해.. */
    KEYFRAME LastKeyFrame = m_KeyFrames.back();

    _float3 vScale{}, vPosition{};
    _float4 vRotation{};

    if (_fCurTrackPosition >= LastKeyFrame.fTrackPosition)
    {
        /* 만약, 현재 애니메이션의 진행위치가 가장 마지막 키프레임을 넘어섰다면, (아까 말한 마지막 키프레임을 정의하지 않은 경우...) */
        vScale = LastKeyFrame.vScale;
        vRotation = LastKeyFrame.vRotation;
        vPosition = LastKeyFrame.vPosition;
    }
    else
    {
        /* Update CurKeyFrameIndex */
        while (_fCurTrackPosition >= m_KeyFrames[*_pKeyFrameIndex + 1].fTrackPosition)
        {
            // if문을 while로 바꾸었다 그 효과로 프레임드랍이 심한경우에도, fCurTrackPosition이 한번에 너무 많이 증가하더라도, KeyFrameIndex는 그에 맞춰 여러번 증가될 것이다.
            ++*_pKeyFrameIndex;
        }

        /* 나의 트랙포지션상의 위치를 기준으로 좌, 우 키프레임과의 비율을 구한다. */
        _float fRatio = (_fCurTrackPosition - m_KeyFrames[*_pKeyFrameIndex].fTrackPosition) /
                        (m_KeyFrames[*_pKeyFrameIndex + 1].fTrackPosition - m_KeyFrames[*_pKeyFrameIndex].fTrackPosition);

        _float3 vCurScale{},    vNextScale{};
        _float4 vCurRotation{}, vNextRotation{};
        _float3 vCurPosition{}, vNextPosition{};

        vCurScale = m_KeyFrames[*_pKeyFrameIndex].vScale;
        vCurRotation = m_KeyFrames[*_pKeyFrameIndex].vRotation;
        vCurPosition = m_KeyFrames[*_pKeyFrameIndex].vPosition;

        vNextScale = m_KeyFrames[*_pKeyFrameIndex + 1].vScale;
        vNextRotation = m_KeyFrames[*_pKeyFrameIndex + 1].vRotation;
        vNextPosition = m_KeyFrames[*_pKeyFrameIndex + 1].vPosition;

        /* 구한 Ratio 로 크, 자, 이 선형보간 값 계산. */
        XMStoreFloat3(&vScale, XMVectorLerp(XMLoadFloat3(&vCurScale), XMLoadFloat3(&vNextScale), fRatio));
        XMStoreFloat4(&vRotation, XMQuaternionSlerp(XMLoadFloat4(&vCurRotation), XMLoadFloat4(&vNextRotation), fRatio));
        XMStoreFloat3(&vPosition, XMVectorLerp(XMLoadFloat3(&vCurPosition), XMLoadFloat3(&vNextPosition), fRatio));

    }
    /* 보간된 값이 나왔다면 Affine Matrix 구성. >> 이게 CBone의 Transformation Matrix가 된다. */
    _vecBones[m_iBoneIndex]->Set_TransformationMatrix(XMMatrixAffineTransformation(XMLoadFloat3(&vScale), 
                                                                                   XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), 
                                                                                   XMLoadFloat4(&vRotation),
                                                                                   XMVectorSetW(XMLoadFloat3(&vPosition), 1.0f)));
}



CChannel* CChannel::Create(const aiNodeAnim* _pAIChannel, const CModel* _pModel)
{
    CChannel* pInstance = new CChannel();

    if (FAILED(pInstance->Initialize(_pAIChannel, _pModel)))
    {
        MSG_BOX("Failed to Created : CChannel");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CChannel* CChannel::Create(const FBX_CHANNEL& _ChannelDesc)
{
    CChannel* pInstance = new CChannel();

    if (FAILED(pInstance->Initialize(_ChannelDesc)))
    {
        MSG_BOX("Failed to Created : CChannel");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CChannel::Free()
{
    __super::Free();
}

HRESULT CChannel::ConvertToBinary(HANDLE _hFile, DWORD* _dwByte)
{
    if (!WriteFile(_hFile, m_szName, sizeof(m_szName), _dwByte, nullptr))
        return E_FAIL;

    if (!WriteFile(_hFile, &m_iNumKeyFrames, sizeof(_uint), _dwByte, nullptr))
        return E_FAIL;

    if (!WriteFile(_hFile, &m_iBoneIndex, sizeof(_uint), _dwByte, nullptr))
        return E_FAIL;


    for (KEYFRAME& KeyFrame : m_KeyFrames)
    {
        if (!WriteFile(_hFile, &KeyFrame.vScale, sizeof(_float3), _dwByte, nullptr))
            return E_FAIL;
        if (!WriteFile(_hFile, &KeyFrame.vRotation, sizeof(_float4), _dwByte, nullptr))
            return E_FAIL;
        if (!WriteFile(_hFile, &KeyFrame.vPosition, sizeof(_float3), _dwByte, nullptr))
            return E_FAIL;
        if (!WriteFile(_hFile, &KeyFrame.fTrackPosition, sizeof(_float), _dwByte, nullptr))
            return E_FAIL;
    }


    return S_OK;
}
