#include "AudioManager.h"
#include <iostream>
#include <fmod_errors.h>

AudioManager::AudioManager()
    : m_System(nullptr)
{
}

AudioManager::~AudioManager()
{
    Shutdown();
}

AudioManager& AudioManager::GetInstance()
{
    static AudioManager instance;
    return instance;
}

bool AudioManager::CheckError(FMOD_RESULT result, const std::string& context)
{
    if (result != FMOD_OK)
    {
        std::cerr << "FMOD error in " << context << ": " << FMOD_ErrorString(result) << std::endl;
        return false;
    }
    return true;
}

bool AudioManager::Initialise()
{
    FMOD_RESULT result = FMOD::System_Create(&m_System);
    if (!CheckError(result, "System_Create")) return false;

    result = m_System->init(512, FMOD_INIT_NORMAL, nullptr);
    return CheckError(result, "System init");
}

void AudioManager::Process()
{
    if (m_System)
    {
        m_System->update();
    }
}

void AudioManager::Shutdown()
{
    for (auto& pair : m_Sounds)
    {
        if (pair.second)
        {
            pair.second->release();
        }
    }
    m_Sounds.clear();

    if (m_System)
    {
        m_System->close();
        m_System->release();
        m_System = nullptr;
    }
}

bool AudioManager::LoadSound(const std::string& name, const std::string& path, bool loop)
{
    if (!m_System) return false;

    FMOD_MODE mode = FMOD_DEFAULT;
    if (loop)
    {
        mode |= FMOD_LOOP_NORMAL;
    }

    FMOD::Sound* sound = nullptr;
    FMOD_RESULT result = m_System->createSound(path.c_str(), mode, nullptr, &sound);
    if (!CheckError(result, "createSound: " + path)) return false;

    m_Sounds[name] = sound;
    return true;
}

void AudioManager::PlaySound(const std::string& name, float volume)
{
    if (!m_System) return;

    auto it = m_Sounds.find(name);
    if (it != m_Sounds.end())
    {
        FMOD::Channel* channel = nullptr;
        FMOD_RESULT result = m_System->playSound(it->second, nullptr, false, &channel);
        if (CheckError(result, "playSound: " + name) && channel)
        {
            channel->setVolume(volume);
            m_Channels[name] = channel;  // Store the channel
        }
    }
    else
    {
        std::cerr << "Sound not found: " << name << std::endl;
    }
}

void AudioManager::StopSound(const std::string& name)
{
    if (!m_System) return;

    auto it = m_Channels.find(name);
    if (it != m_Channels.end())
    {
        FMOD::Channel* channel = it->second;
        channel->stop();
        m_Channels.erase(it);
    }
}