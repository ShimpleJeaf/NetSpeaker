#ifndef AUDIOCAPTURED_H
#define AUDIOCAPTURED_H

#include <MMDeviceAPI.h>
#include <propvarutil.h>
#include <Audioclient.h>
#include <Audiopolicy.h>
#include <Functiondiscoverykeys_devpkey.h>

#include <cstring>
#include <QDebug>
#include <QAudioFormat>


//-----------------------------------------------------------
// Record an audio stream from the default audio capture
// device. The RecordAudioStream function allocates a shared
// buffer big enough to hold one second of PCM audio data.
// The function uses this buffer to stream data from the
// capture device. The main loop runs every 1/2 second.
//-----------------------------------------------------------

// REFERENCE_TIME time units per second and per millisecond
#define REFTIMES_PER_SEC  10000000
#define REFTIMES_PER_MILLISEC  10000

#define EXIT_ON_ERROR(hres)  \
if (FAILED(hres)) { goto Exit; }
#define SAFE_RELEASE(punk)  \
if ((punk) != NULL)  \
    { (punk)->Release(); (punk) = NULL; }

const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
const IID IID_IAudioClient = __uuidof(IAudioClient);
const IID IID_IAudioCaptureClient = __uuidof(IAudioCaptureClient);


class MyAudioSink
{
    unsigned char buffer[1024000];
    size_t buferLen;
    size_t frameSize;
    QAudioFormat m_format;
public:
    QAudioFormat format() const
    {
        return m_format;
    }

    HRESULT SetFormat(WAVEFORMATEX *pwfx)
    {
        frameSize = (pwfx->wBitsPerSample / 8) * pwfx->nChannels;

        // m_format.setSampleFormat(SampleFormat)
        // m_format.setByteOrder(QAudioFormat::LittleEndian);
        // m_format.setChannelCount(pwfx->nChannels);
        // m_format.setCodec("audio/pcm");
        // m_format.setSampleRate(pwfx->nSamplesPerSec);
        // m_format.setSampleSize(pwfx->wBitsPerSample);
        // m_format.setSampleType(QAudioFormat::Float);
        return 0;
    }

    HRESULT CopyData(BYTE* pData, UINT32 numFramesAvailable, BOOL* bDone)
    {
        if (!pData || numFramesAvailable <= 0)
        {
            *bDone = FALSE;
            return 0;
        }
        std::memcpy(buffer, pData, numFramesAvailable * frameSize);
        buferLen = numFramesAvailable;
        *bDone = FALSE;
        return 0;
    }
};

// HRESULT __RecordAudioStream(MyAudioSink *pMySink)
// {
//     ComPtr<IMMDeviceEnumerator> enumerator;
//     HRESULT res;
//     res = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)enumerator.GetAddressOf());
//     if (FAILED(res))
//         qDebug() << "Failed to create enumerator";
// }

HRESULT RecordAudioStream(MyAudioSink *pMySink)
{
    HRESULT hr;
    REFERENCE_TIME hnsRequestedDuration = REFTIMES_PER_SEC;
    REFERENCE_TIME hnsActualDuration;
    UINT32 bufferFrameCount;
    UINT32 numFramesAvailable;
    IMMDeviceEnumerator *pEnumerator = NULL;
    IMMDevice *pDevice = NULL;
    IAudioClient *pAudioClient = NULL;
    IAudioCaptureClient *pCaptureClient = NULL;
    WAVEFORMATEX *pwfx = NULL;
    UINT32 packetLength = 0;
    BOOL bDone = FALSE;
    BYTE *pData;
    DWORD flags;

    hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED | COINIT_SPEED_OVER_MEMORY);
    EXIT_ON_ERROR(hr);

    hr = CoCreateInstance(CLSID_MMDeviceEnumerator, NULL, CLSCTX_INPROC_SERVER, IID_IMMDeviceEnumerator, (void**)&pEnumerator);
    EXIT_ON_ERROR(hr);

    hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
    EXIT_ON_ERROR(hr)

    LPWSTR tempStr;
    hr = pDevice->GetId(&tempStr);
    qDebug() << QString::fromWCharArray(tempStr);

    hr = pDevice->Activate(IID_IAudioClient, CLSCTX_ALL, NULL, (void**)&pAudioClient);
    EXIT_ON_ERROR(hr)

    hr = pAudioClient->GetMixFormat(&pwfx);
    EXIT_ON_ERROR(hr)

    // 输出设备第二个参数必须是AUDCLNT_STREAMFLAGS_LOOPBACK，输入设备是0
    hr = pAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_LOOPBACK, hnsRequestedDuration, 0, pwfx, NULL);
    EXIT_ON_ERROR(hr)

    // Get the size of the allocated buffer.
    hr = pAudioClient->GetBufferSize(&bufferFrameCount);
    EXIT_ON_ERROR(hr)

    hr = pAudioClient->GetService(IID_IAudioCaptureClient, (void**)&pCaptureClient);
    EXIT_ON_ERROR(hr)

    // Notify the audio sink which format to use.
    hr = pMySink->SetFormat(pwfx);
    EXIT_ON_ERROR(hr)

    // Calculate the actual duration of the allocated buffer.
    hnsActualDuration = (double)REFTIMES_PER_SEC * bufferFrameCount / pwfx->nSamplesPerSec;

    hr = pAudioClient->Start();  // Start recording.
    EXIT_ON_ERROR(hr)

    // Each loop fills about half of the shared buffer.
    while (bDone == FALSE)
    {
        // Sleep for half the buffer duration.
        Sleep(hnsActualDuration/REFTIMES_PER_MILLISEC/2);

        hr = pCaptureClient->GetNextPacketSize(&packetLength);
        EXIT_ON_ERROR(hr)

        while (packetLength != 0)
        {
            // Get the available data in the shared buffer.
            hr = pCaptureClient->GetBuffer( &pData, &numFramesAvailable, &flags, NULL, NULL);
            EXIT_ON_ERROR(hr)

            if (flags & AUDCLNT_BUFFERFLAGS_SILENT)
            {
                pData = NULL;  // Tell CopyData to write silence.
            }

            // Copy the available capture data to the audio sink.
            hr = pMySink->CopyData(pData, numFramesAvailable, &bDone);
            EXIT_ON_ERROR(hr)

            hr = pCaptureClient->ReleaseBuffer(numFramesAvailable);
            EXIT_ON_ERROR(hr)

            hr = pCaptureClient->GetNextPacketSize(&packetLength);
            EXIT_ON_ERROR(hr)
        }
    }

    hr = pAudioClient->Stop();  // Stop recording.
    EXIT_ON_ERROR(hr)

Exit:
    DWORD errorCode = hr;//WSAGetLastError();
    LPSTR errorMessage = nullptr;

    DWORD result = FormatMessageA( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL,
                                  errorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&errorMessage, 0, NULL);

    if (result == 0) {
        qDebug() << "Failed to get error message";
    }
    else
        qDebug() << QString::fromLocal8Bit(errorMessage);
    CoTaskMemFree(pwfx);
    SAFE_RELEASE(pEnumerator)
    SAFE_RELEASE(pDevice)
    SAFE_RELEASE(pAudioClient)
    SAFE_RELEASE(pCaptureClient)

    return hr;
}

#endif // AUDIOCAPTURED_H
