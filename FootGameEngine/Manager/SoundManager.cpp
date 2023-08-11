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
		// FMOD �ý����� �����ϰ� ������ system�� �Ҵ�
		FMOD::System_Create(&system);

		// FMOD �ý����� �ʱ�ȭ�մϴ�.
		// 512�� �ִ� ���� ��� ������ ����� ä�� ���Դϴ�.
		system->init(512, FMOD_INIT_NORMAL, 0);

		backgroundSounds.clear();    // ����� �� �ʱ�ȭ
		sfxSounds.clear();           // ȿ���� �� �ʱ�ȭ
		backgroundChannels.clear();  // ����� ä�� �� �ʱ�ȭ
		sfxChannels.clear();         // ȿ���� ä�� �� �ʱ�ȭ
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

		// ������ ����� ä���� �ִ� ��� ����
		if (iter != backgroundChannels.end())
		{
			iter->second->stop();
			backgroundChannels.erase(iter);
		}

		auto findIt = backgroundSounds.find(name);

		if (findIt == backgroundSounds.end())
		{
			// �ε���� ���� ������ ���
			return;
		}

		FMOD::Channel* channel;
		system->playSound(findIt->second, 0, true, &channel);
		channel->setVolume(backVolume * masterVolume);  // �־��� �������� ���� ũ�� ����
		channel->setPaused(false);

		backgroundChannels[name] = channel;
	}

	void SoundManager::PlaySFXSound(const std::string& name, float volume)
	{
		auto iter = sfxChannels.find(name);

		// ������ ����� ä���� �ִ� ��� ����
		if (iter != sfxChannels.end())
		{
			iter->second->stop();
			sfxChannels.erase(iter);
		}

		auto findIt = sfxSounds.find(name);

		if (findIt == sfxSounds.end())
		{
			// �ε���� ���� ������ ���
			return;
		}

		FMOD::Channel* channel;
		system->playSound(findIt->second, 0, false, &channel);
		channel->setVolume(sfxVolume * masterVolume);  // �־��� �������� ���� ũ�� ����
		channel->setPaused(false);

		sfxChannels[name] = channel;
	}

	void SoundManager::SetMasterVolume(float volume)
	{
		masterVolume = volume;

		// ��� ����� ä���� ������ �����մϴ�.
		for (auto& pair : backgroundChannels)
		{
			pair.second->setVolume(backVolume * masterVolume);
		}

		// ��� ȿ���� ä���� ������ �����մϴ�.
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