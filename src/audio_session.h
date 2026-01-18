#pragma once

#include "audio_manager.h"

/**
 * @file audio_session.h
 * @brief RAII helper for initializing and shutting down the audio system
 */

namespace audio {

/**
 * @class AudioSession
 * @brief RAII wrapper that initializes audio on construction and shuts down on destruction
 *
 * This provides an explicit but convenient lifecycle helper that aligns with
 * constructor/destructor patterns in game engines while keeping AudioManager
 * as the single point of control.
 */
class AudioSession {
public:
    AudioSession();
    ~AudioSession();

    AudioSession(const AudioSession&) = delete;
    AudioSession& operator=(const AudioSession&) = delete;
    AudioSession(AudioSession&&) = delete;
    AudioSession& operator=(AudioSession&&) = delete;

    /**
     * @brief Manually shut down if this session owns initialization
     */
    void Close();

private:
    bool owns_init_;
};

} // namespace audio
