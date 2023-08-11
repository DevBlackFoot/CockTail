#pragma once

namespace GameEngineSpace
{
	class SoundManager final
	{
		// 단 하나 존재하는 SceneManager 객체
		static std::shared_ptr<SoundManager> instance;

	private:
		FMOD::System* system;
		std::unordered_map<std::string, FMOD::Sound*> backgroundSounds;		 // 배경음 저장을 위한 맵
		std::unordered_map<std::string, FMOD::Sound*> sfxSounds;             // 효과음 저장을 위한 맵
		std::unordered_map<std::string, FMOD::Channel*> backgroundChannels;  // 배경음 채널 저장을 위한 맵
		std::unordered_map<std::string, FMOD::Channel*> sfxChannels;		 // 효과음 채널 저장을 위한 맵

	private:
		float masterVolume = 1.f;
		float sfxVolume = 1.f;
		float backVolume = 1.f;

	public:
		SoundManager() = default;

		~SoundManager() = default;

		// 이동, 복사 생성자 제거
		SoundManager(const SoundManager& _other) = delete;
		SoundManager& operator= (const SoundManager& _other) = delete;
		SoundManager(SoundManager&& _other) = delete;
		SoundManager& operator= (SoundManager&& _other) = delete;

		static std::shared_ptr<SoundManager> GetInstance();

	public:
		// FMOD 시스템을 생성하고 초기화 한다.
		void Initialize();

		// sounds와 channels 배열에 할당된 메모리를 해제한다.
		void Release();

		void LoadBackgroundSound(const std::string& filePath, const std::string& name, bool loop);

		void LoadSFXSound(const std::string& filePath, const std::string& name, bool loop);

		void PlayBackgroundSound(const std::string& name, float volume = 1.f);

		void PlaySFXSound(const std::string& name, float volume = 1.f);

		void SetMasterVolume(float volume);

		void SetBackgroundVolume(float volume);

		void SetSFXVolume(float volume);

		// 맵에서 사운드 객체를 찾아 재생을 일시 중지 한다.
		void PauseSound(const std::string& name);

		// 맵에서 사운드 객체를 찾아 재생을 재개 한다.
		void ResumeSound(const std::string& name);

		// 재생되고 있는 모든 사운드를 끈다.
		void StopAllSound();
	};
}