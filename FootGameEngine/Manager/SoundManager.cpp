#include "GamePch.h"
#include "SoundManager.h"

namespace GameEngineSpace
{
	std::shared_ptr<SoundManager> SoundManager::instance = nullptr;

	std::shared_ptr<SoundManager> SoundManager::GetInstance()
	{
		if (instance == nullptr)
			instance = std::make_shared<SoundManager>();

		return instance;
	}

	void SoundManager::Initialize()
	{
		// FMOD 시스템을 생성하고 포인터 system에 할당
		FMOD::System_Create(&system);

		// FMOD 시스템을 초기화합니다.
		// 512는 최대 동시 재생 가능한 오디오 채널 수입니다.
		system->init(512, FMOD_INIT_NORMAL, 0);

		backgroundSounds.clear();    // 배경음 맵 초기화
		sfxSounds.clear();           // 효과음 맵 초기화
		backgroundChannels.clear();  // 배경음 채널 맵 초기화
		sfxChannels.clear();         // 효과음 채널 맵 초기화
	}

	void SoundManager::Release()
	{
		for (auto& sound : backgroundSounds)
		{
			if (sound.second)
				sound.second->release();
		}

		for (auto& sound : sfxSounds)
		{
			if (sound.second)
				sound.second->release();
		}

		system->close();
		system->release();
	}

	void SoundManager::LoadBackgroundSound(const std::string& filePath, const std::string& name, bool loop)
	{
		FMOD::Sound* sound;
		system->createSound(filePath.c_str(), loop ? FMOD_LOOP_NORMAL : FMOD_DEFAULT, 0, &sound);

		backgroundSounds[name] = sound;
	}

	void SoundManager::LoadSFXSound(const std::string& filePath, const std::string& name, bool loop)
	{
		FMOD::Sound* sound;
		system->createSound(filePath.c_str(), loop ? FMOD_LOOP_NORMAL : FMOD_DEFAULT, 0, &sound);

		sfxSounds[name] = sound;
	}

	void SoundManager::PlayBackgroundSound(const std::string& name, float volume)
	{
		auto iter = backgroundChannels.find(name);

		// 이전에 재생된 채널이 있는 경우 중지
		if (iter != backgroundChannels.end())
		{
			iter->second->stop();
			backgroundChannels.erase(iter);
		}

		auto findIt = backgroundSounds.find(name);

		if (findIt == backgroundSounds.end())
		{
			// 로드되지 않은 사운드인 경우
			return;
		}

		FMOD::Channel* channel;
		system->playSound(findIt->second, 0, true, &channel);
		channel->setVolume(backVolume * masterVolume);  // 주어진 볼륨으로 사운드 크기 설정
		channel->setPaused(false);

		backgroundChannels[name] = channel;
	}

	void SoundManager::PlaySFXSound(const std::string& name, float volume)
	{
		auto iter = sfxChannels.find(name);

		// 이전에 재생된 채널이 있는 경우 중지
		if (iter != sfxChannels.end())
		{
			iter->second->stop();
			sfxChannels.erase(iter);
		}

		auto findIt = sfxSounds.find(name);

		if (findIt == sfxSounds.end())
		{
			// 로드되지 않은 사운드인 경우
			return;
		}

		FMOD::Channel* channel;
		system->playSound(findIt->second, 0, false, &channel);
		channel->setVolume(sfxVolume * masterVolume);  // 주어진 볼륨으로 사운드 크기 설정
		channel->setPaused(false);

		sfxChannels[name] = channel;
	}

	void SoundManager::SetMasterVolume(float volume)
	{
		masterVolume = volume;

		// 모든 배경음 채널의 볼륨을 조절합니다.
		for (auto& pair : backgroundChannels)
		{
			pair.second->setVolume(backVolume * masterVolume);
		}

		// 모든 효과음 채널의 볼륨을 조절합니다.
		for (auto& pair : sfxChannels)
		{
			pair.second->setVolume(sfxVolume * masterVolume);
		}
	}

	void SoundManager::SetBackgroundVolume(float volume)
	{
		backVolume = volume;

		for (auto& pair : backgroundChannels)
		{
			pair.second->setVolume(backVolume);
		}
	}

	void SoundManager::SetSFXVolume(float volume)
	{
		sfxVolume = volume;

		for (auto& pair : sfxChannels)
		{
			pair.second->setVolume(sfxVolume);
		}
	}

	void SoundManager::PauseSound(const std::string& name)
	{
		auto findBackIt = backgroundChannels.find(name);

		if (findBackIt != backgroundChannels.end())
		{
			findBackIt->second->setPaused(true);
		}

		auto findSFXIt = sfxChannels.find(name);

		if (findSFXIt != sfxChannels.end())
		{
			findSFXIt->second->setPaused(true);
		}
	}

	void SoundManager::ResumeSound(const std::string& name)
	{
		auto findBackIt = backgroundChannels.find(name);

		if (findBackIt != backgroundChannels.end())
		{
			findBackIt->second->setPaused(false);
		}

		auto findSFXIt = sfxChannels.find(name);

		if (findSFXIt != sfxChannels.end())
		{
			findSFXIt->second->setPaused(false);
		}
	}

	void SoundManager::StopAllSound()
	{
		for (auto iter = backgroundChannels.begin(); iter != backgroundChannels.end(); ++iter)
		{
			iter->second->stop();
		}

		for (auto iter = sfxChannels.begin(); iter != sfxChannels.end(); ++iter)
		{
			iter->second->stop();
		}

		backgroundChannels.clear();
		sfxChannels.clear();
	}
}