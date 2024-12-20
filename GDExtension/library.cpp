#include "library.h"

#include <vector>

#ifndef GDEXAMPLE_H
#define GDEXAMPLE_H

#include <Godot.hpp>
#include <Node.hpp>
#include <AudioStreamGeneratorPlayback.hpp>
#include <AudioStreamGenerator.hpp>

#include <fstream>
#include <iostream>

#include <windows.h>
#include <ttsapi.h>

#define  NSAMPLES         8192
#define  MAX_PHONEMES      128
#define  MAX_INDEX_MARKS   128
#define  MAX_STRING_LEN  32768
#define  NAUDIO_BUFFERS      1

class DecTalk;
using namespace godot;

static DecTalk *instance;

HINSTANCE hinstDLL;

class DecTalk : public Node {
    GODOT_CLASS(DecTalk, Node)
    MMRESULT status = 0;

    LPTTS_HANDLE_T ttsHandle = nullptr;

    LPTTS_BUFFER_T OnelpSpeechBuffer = nullptr;
    LPTTS_BUFFER_T lpSpeechBuffers[NAUDIO_BUFFERS] = {};

    std::vector<char> speechBuffers = {};

public:
    static void _register_methods() {
        register_method("Speak", &DecTalk::Speak);
    }

    static void callback(LONG lParam1, LONG lParam2, DWORD dwCallbackParameter, UINT uiMsg) {
        instance->OnelpSpeechBuffer = instance->lpSpeechBuffers[0];

        Godot::print(String(": OnelpSpeechBuffer {status}").format(
            Dictionary::make("status", (int) instance->OnelpSpeechBuffer->dwBufferLength)));

        for (int i = 0; i < instance->OnelpSpeechBuffer->dwBufferLength; i++) {
            instance->speechBuffers.push_back(instance->OnelpSpeechBuffer->lpData[i]);
        }

        instance->status = TextToSpeechAddBuffer(instance->ttsHandle, instance->OnelpSpeechBuffer);

        if (instance->status) {
            Godot::print(
                String(": TextToSpeechAddBuffer failed with code {status}, exiting.\n").format(
                    Dictionary::make("status", instance->status)));
            instance->queue_free();
            return;
        }
    }

    std::wstring GetExecutableDirectory() {
        wchar_t path[512];
        if (GetModuleFileNameW(hinstDLL, path, MAX_PATH) == 0) {
            Godot::print("Failed to get executable path!");
            return L"";
        }

        Godot::print(path);

        std::wstring fullPath(path);
        size_t pos = fullPath.find_last_of(L"\\/");

        if (pos != std::wstring::npos) {
            return fullPath.substr(0, pos);
        }

        return L"";
    }

    std::wstring GetCurrentWorkingDirectory() {
        wchar_t currentDir[512];

        DWORD length = GetCurrentDirectoryW(512, currentDir);
        if (length == 0) {
            Godot::print("Failed to get current directory!");
            return L"";
        }
        return std::wstring(currentDir);
    }

    void _init() {
        instance = this;

        auto versionInfo = static_cast<LPSTR>(malloc(64));
        TextToSpeechVersion(&versionInfo);
        Godot::print(versionInfo);

        const std::wstring currentDir = GetCurrentWorkingDirectory();
        const std::wstring fullPath = GetExecutableDirectory() + L"\\dic";

        SetCurrentDirectoryW(fullPath.c_str());
        // TODO: some how make callback non-static?
        status = TextToSpeechStartupEx(&ttsHandle, WAVE_MAPPER, DO_NOT_USE_AUDIO_DEVICE, callback, 0);
        SetCurrentDirectoryW(currentDir.c_str());

        switch (status) {
            case MMSYSERR_NODRIVER:
                Godot::print(": Could not find any wave devices\n");
                Godot::print(": Is the MM server ready? \n");
                Godot::print(": Exiting\n");
                queue_free();
                return;

            case MMSYSERR_NOTENABLED:
                Godot::print(": DECtalk licence not found.\n");
                Godot::print(": Exiting\n");
                queue_free();
                return;

            case MMSYSERR_ALLOCATED:
                Godot::print(": DECtalk has exceeded licence quota.\n");
                Godot::print(": Exiting\n");
                queue_free();
                return;

            case MMSYSERR_NOMEM:
                Godot::print(": DECtalk has ran out of memory.\n");
                queue_free();
                return;

            case MMSYSERR_NOERROR:
                Godot::print(": DECTalk started.\n");
                break;

            default:
                Godot::print(
                    String(": TextToSpeechStartup failed with code {status}, exiting.\n").format(
                        Dictionary::make("status", status)));
                queue_free();
                return;
        }

        for (int i = 0; i < NAUDIO_BUFFERS; i++) {
            lpSpeechBuffers[i] = (LPTTS_BUFFER_T) malloc(sizeof(TTS_BUFFER_T));

            if (lpSpeechBuffers[i] == (LPTTS_BUFFER_T) NULL) {
                Godot::print(": Could not allocate space for the speech buffer.\n");
                queue_free();
                return;
            }
            lpSpeechBuffers[i]->lpData = (LPSTR) malloc(NSAMPLES * sizeof(char));
            lpSpeechBuffers[i]->dwMaximumBufferLength = NSAMPLES;
            lpSpeechBuffers[i]->lpPhonemeArray =
                    (LPTTS_PHONEME_T) malloc(MAX_PHONEMES * sizeof(TTS_PHONEME_T));
            lpSpeechBuffers[i]->dwMaximumNumberOfPhonemeChanges = MAX_PHONEMES;
            lpSpeechBuffers[i]->lpIndexArray =
                    (LPTTS_INDEX_T) malloc(MAX_INDEX_MARKS * sizeof(TTS_INDEX_T));
            lpSpeechBuffers[i]->dwMaximumNumberOfIndexMarks = MAX_INDEX_MARKS;
        }

        status = TextToSpeechOpenInMemory(ttsHandle, WAVE_FORMAT_1M16);
        if (status != MMSYSERR_NOERROR) {
            Godot::print(
                String(": TextToSpeechOpenInMemory failed with code {status}, exiting.\n").format(
                    Dictionary::make("status", status)));
            queue_free();
            return;
        }

        for (int i = 0; i < NAUDIO_BUFFERS; i++) {
            status = TextToSpeechAddBuffer(ttsHandle, lpSpeechBuffers[i]);

            if (status) {
                Godot::print(
                    String(": TextToSpeechAddBuffer failed with code {status}, exiting.\n").format(
                        Dictionary::make("status", status)));
                queue_free();
                return;
            }
        }
    }

    void Speak(String text, Variant playback_variant) {
        Object *obj = playback_variant.operator godot::Object *();
        if (obj == nullptr) {
            Godot::print("Invalid AudioStreamGeneratorPlayback provided.");
            return;
        }

        // Cast Object to AudioStreamGeneratorPlayback
        auto *playback = cast_to<AudioStreamGeneratorPlayback>(obj);
        if (playback == nullptr) {
            Godot::print("Failed to cast to AudioStreamGeneratorPlayback.");
            return;
        }

        // WEBFISHING Filters out all [ and ] characters, so we need to replace them with { and } then replace them back
        // I'm assuming they're doing this to prevent users from putting BBCode in their chat messages
        text = String("[:name p][:phoneme on]") + text;
        text = text.replace('{', '[');
        text = text.replace('}', ']');

        Godot::print(
            String(": Speaking: {text}\n").format(
                Dictionary::make("text", text)));

        status = TextToSpeechSpeak(ttsHandle, text.alloc_c_string(), TTS_FORCE);
        if (status != MMSYSERR_NOERROR) {
            Godot::print(
                String(": TextToSpeechSpeak failed with code {status}, exiting.\n").format(
                    Dictionary::make("status", status)));
            queue_free();
            return;
        }

        Godot::print(String(": Syncing\n"));
        TextToSpeechSync(ttsHandle);
        Godot::print(String(": Synced\n"));

        for (int i = 0; i < speechBuffers.size(); i += 2) {
            float sample = speechBuffers[i]; // Stinkyy
            sample += (float)speechBuffers[i + 1] * 255;

            const float s = static_cast<float>(sample) / 32768.0f;

            // Constant up-sampling thing? https://github.com/DeltaNeverUsed/WebfishingDECTalk/issues/1#issue-2676964517
            playback->push_frame(Vector2(s, s));
            playback->push_frame(Vector2(s, s));
            playback->push_frame(Vector2(s, s));
            playback->push_frame(Vector2(s, s));
        }

        speechBuffers.clear();
    }

    ~DecTalk() {
        Godot::print(": DECTalk shutting down.\n");
        TextToSpeechCloseInMemory(ttsHandle);

        status = TextToSpeechShutdown(ttsHandle);
        if (status != MMSYSERR_NOERROR) {
            Godot::print(
                String(": TextToSpeechShutdown failed with code {status}.\n").format(
                    Dictionary::make("status", status)));
        }

        for (auto &lpSpeechBuffer: lpSpeechBuffers) {
            if (lpSpeechBuffer != nullptr) {
                std::free(lpSpeechBuffer->lpData);
                std::free(lpSpeechBuffer->lpPhonemeArray);
                std::free(lpSpeechBuffer->lpIndexArray);
                std::free(lpSpeechBuffer);
                lpSpeechBuffer = nullptr;
            }
        }
    }
};


extern "C" void GDN_EXPORT godot_gdnative_init(godot_gdnative_init_options *o) {
    Godot::gdnative_init(o);
}

extern "C" void GDN_EXPORT godot_gdnative_terminate(godot_gdnative_terminate_options *o) {
    Godot::gdnative_terminate(o);
}

extern "C" void GDN_EXPORT godot_nativescript_init(void *handle) {
    Godot::nativescript_init(handle);
    register_class<DecTalk>();
}

BOOL WINAPI DllMain(
    HINSTANCE hinstDLL_org,  // handle to DLL module
    DWORD fdwReason,     // reason for calling function
    LPVOID lpvReserved
    ) {
    hinstDLL = hinstDLL_org;

    return TRUE;
}
#endif
