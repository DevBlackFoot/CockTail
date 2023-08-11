#pragma once

namespace GameEngineSpace
{
	class SoundManager final
	{
		// �� �ϳ� �����ϴ� SceneManager ��ü
		static std::shared_ptr<SoundManager> instance;

	private:
		FMOD::System* system;
		std::unordered_map<std::string, FMOD::Sound*> backgroundSounds;		 // ����� ������ ���� ��
		std::unordered_map<std::string, FMOD::Sound*> sfxSounds;             // ȿ���� ������ ���� ��
		std::unordered_map<std::string, FMOD::Channel*> backgroundChannels;  // ����� ä�� ������ ���� ��
		std::unordered_map<std::string, FMOD::Channel*> sfxChannels;		 // ȿ���� ä�� ������ ���� ��

	private:
		float masterVolume = 1.f;
		float sfxVolume = 1.f;
		float backVolume = 1.f;

	public:
		SoundManager() = default;

		~SoundManager() = default;

		// �̵�, ���� ������ ����
		SoundManager(const SoundManager& _other) = delete;
		SoundManager& operator= (const SoundManager& _other) = delete;
		SoundManager(SoundManager&& _other) = delete;
		SoundManager& operator= (SoundManager&& _other) = delete;

		static std::shared_ptr<SoundManager> GetInstance();

	public:
		// FMOD �ý����� �����ϰ� �ʱ�ȭ �Ѵ�.
		void Initialize();

		// sounds�� channels �迭�� �Ҵ�� �޸𸮸� �����Ѵ�.
		void Release();

		void LoadBackgroundSound(const std::string& filePath, const std::string& name, bool loop);

		void LoadSFXSound(const std::string& filePath, const std::string& name, bool loop);

		void PlayBackgroundSound(const std::string& name, float volume = 1.f);

		void PlaySFXSound(const std::string& name, float volume = 1.f);

		void SetMasterVolume(float volume);

		void SetBackgroundVolume(float volume);

		void SetSFXVolume(float volume);

		// �ʿ��� ���� ��ü�� ã�� ����� �Ͻ� ���� �Ѵ�.
		void PauseSound(const std::string& name);

		// �ʿ��� ���� ��ü�� ã�� ����� �簳 �Ѵ�.
		void ResumeSound(const std::string& name);

		// ����ǰ� �ִ� ��� ���带 ����.
		void StopAllSound();
	};
}