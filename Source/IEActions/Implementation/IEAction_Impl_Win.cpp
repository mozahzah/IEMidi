// Copyright © 2024 mozahzah (Incus Entertainment). All rights reserved.

#include "IEActions/Implementation/IEAction_Impl_Win.h"

#if defined (_WIN32)
IEAction_Volume_Impl_Win::IEAction_Volume_Impl_Win()
{
    m_AudioEndpointVolume = GetMainAudioEndpointVolume();
}

float IEAction_Volume_Impl_Win::GetVolume() const
{
    float Volume;
    if (m_AudioEndpointVolume)
    {
        m_AudioEndpointVolume->GetMasterVolumeLevel(&Volume);
    }
    return Volume;
}

void IEAction_Volume_Impl_Win::SetVolume(float Volume)
{
    if (m_AudioEndpointVolume)
    {
        m_AudioEndpointVolume->SetMasterVolumeLevelScalar(Volume/127, nullptr);
    }
}

void IEAction_Volume_Impl_Win::RegisterVolumeChangeCallback(const std::function<void(float)>& Callback)
{
    m_OnVolumeChangeCallback = Callback;
}

IAudioEndpointVolume* IEAction_Volume_Impl_Win::GetMainAudioEndpointVolume()
{
    IAudioEndpointVolume* EndpointVolume = nullptr;
    HRESULT Result = CoInitialize(NULL);
    if (SUCCEEDED(Result))
    {
        IMMDeviceEnumerator* DeviceEnumerator = nullptr;
        Result = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&DeviceEnumerator));
        if (SUCCEEDED(Result) && DeviceEnumerator)
        {
            IMMDevice* Device = nullptr;
            Result = DeviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &Device);
            if (SUCCEEDED(Result) && Device)
            {
                Result = Device->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, NULL, (void**)&EndpointVolume);
                if (FAILED(Result))
                {
                    Device->Release();
                    DeviceEnumerator->Release();
                    CoUninitialize();
                }
            }
            else
            {
                DeviceEnumerator->Release();
                CoUninitialize();
            }
        }
        else
        {
            CoUninitialize();
        }
    }
    return EndpointVolume;
}

IEAction_Mute_Impl_Win::IEAction_Mute_Impl_Win()
{
    m_AudioEndpointVolume = GetMainAudioEndpointVolume();
}

bool IEAction_Mute_Impl_Win::GetMute() const
{
    BOOL bMute;
    if (m_AudioEndpointVolume)
    {
        m_AudioEndpointVolume->GetMute(&bMute);
    }
    return bMute;
}

void IEAction_Mute_Impl_Win::SetMute(bool bMute)
{
    if (m_AudioEndpointVolume)
    {
        m_AudioEndpointVolume->SetMute(bMute, nullptr);
    }
}

void IEAction_Mute_Impl_Win::RegisterMuteChangeCallback(const std::function<void(bool)>& Callback)
{
    m_OnMuteChangeCallback = Callback;
}

IAudioEndpointVolume* IEAction_Mute_Impl_Win::GetMainAudioEndpointVolume()
{
    IAudioEndpointVolume* EndpointVolume = nullptr;
    HRESULT Result = CoInitialize(NULL);
    if (SUCCEEDED(Result))
    {
        IMMDeviceEnumerator* DeviceEnumerator = nullptr;
        Result = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&DeviceEnumerator));
        if (SUCCEEDED(Result) && DeviceEnumerator)
        {
            IMMDevice* Device = nullptr;
            Result = DeviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &Device);
            if (SUCCEEDED(Result) && Device)
            {
                Result = Device->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, NULL, (void**)&EndpointVolume);
                if (FAILED(Result))
                {
                    Device->Release();
                    DeviceEnumerator->Release();
                    CoUninitialize();
                }
            }
            else
            {
                DeviceEnumerator->Release();
                CoUninitialize();
            }
        }
        else
        {
            CoUninitialize();
        }
    }
    return EndpointVolume;
}
#endif