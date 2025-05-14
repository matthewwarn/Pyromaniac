#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include <string>
#include <map>
#include <fmod.hpp>

class AudioManager
{
public:
    static AudioManager& GetInstance();

    bool Initialise();
    void Process();
    void Shutdown();

    bool LoadSound(const std::string& name, const std::string& path, bool loop);
    void PlaySound(const std::string& name, float volume);
    void StopSound(const std::string& name);

    bool CheckError(FMOD_RESULT result, const std::string& context);


private:
    AudioManager();
    ~AudioManager();

    std::map<std::string, FMOD::Channel*> m_Channels;
    std::map<std::string, FMOD::Sound*> m_Sounds;
    FMOD::System* m_System;
};

#endif // AUDIOMANAGER_H