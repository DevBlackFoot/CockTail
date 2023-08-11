#pragma once

namespace ClientSpace
{
	class PlayerController;

	// Room4���� ������ ���̺�� ��ȯ�ϴ� ������Ʈ�Դϴ�.
	class EnemyWaveSpawn : public ComponentBase
	{
	private:
		// ������ ���� ���� �Ŵ���
		std::shared_ptr<GameManager> gameManager;

		std::shared_ptr<PlayerController> playerController;

		// ���� ���̺� �ε���
		int waveIdx;
		// ������ ų ī��Ʈ => ���� �Ŵ������� ���ɴϴ�.
		// ���̺��� ��ǥ ų ī��Ʈ => ���� ����
		int waveKillGoal;
		// ���̺갡 �� �� ���� ų ī��Ʈ�� �ʱ�ȭ �����ݴϴ�.

		// Spawn Event�� ���� ���� bool ����
			// �� ���� ����Ǿ���Ѵ�. (false �� ��, ���͸� ��ȯ���ִ� ��������)
		bool wave1Play;
		bool wave2Play;
		bool wave3Play;

		// ���������� �з��� �صӽô�.
			// ID, TYPE(1 - Melee, 2 - Range, 3 - Toad), PosX, PosY, PosZ, RotY
		std::vector<std::vector<int>> wave1EnemyData;
		std::vector<std::vector<int>> wave2EnemyData;
		std::vector<std::vector<int>> wave3EnemyData;

		std::shared_ptr<SceneManager> sceneManager;

		// ��� - waveStart 
		bool isWaveStart;

	public:
		EnemyWaveSpawn(std::weak_ptr<GameObject> _object);
		virtual ~EnemyWaveSpawn();

		// ��â�� ����..
			// ���� ���� �� ���� ��������
		void Init();

		// ����� �κп��� ���͵� �� ��ȯ�صα�
			// ù ��° ���̺긦 ������ ���͵� �� SetEnable False�� �صӴϴ�.
		void SetAllEnemy();

		// ������ ���̺� �ε����� üũ�ϸ鼭 �ε��� �ȵǾ����� �ε� �����ִ� ��.
		void Update(float tick) override;

		// Wave���� ���¸� �ٲٴ� �Լ�
		void SetIsWaveStart(bool val) { isWaveStart = val; }

		void SetPlayerController(std::shared_ptr<PlayerController> val) { playerController = val; }
	};

}
