#pragma once

namespace GameEngineSpace
{
	class SpriteEffect;
}

namespace ClientSpace
{
	//class EnemyBehavior;

	/**
	 * \brief ���� ��Ÿ�� �� �����Ǵ� Spawn Effect�� ����ϴ� Component�Դϴ�.
	 */
	class EnemySpawn : public ComponentBase
	{
	private:
		std::shared_ptr<GameEngineSpace::SpriteEffect> spriteEffect;
		std::shared_ptr<GameManager> gameManager;
		std::shared_ptr<Transform> transform;

		//std::shared_ptr<EnemyBehavior> enemyBehavior;

		float alpha;
		// ����ũ �÷�.
		SimpleMath::Vector4 maskColor;

		// ���� ũ�⿡ ���� ��ǥ ������ �� ����.
			// ������ �����ϰ� �ø��ٰ� �����մϴ�.
		float goalScale;
		float nowHeight;
		float eulerZ;
		// ��ǥ height
		float goalHeight;

		// �������� �����ϰ� �Ǿ��°�
		bool isScaleSet;
		// ���� ���� �Ͽ��°�.
		bool isEnd;

		// ����Ʈ�� �����Ͽ��°�.
		bool isPlay;
		// �ε��� �����ϴ� ��.
		bool isLoading;
		// �ε� �Ϸ� ���θ� �˷��ִ� bool ����
		bool isLoadCompleted;

		// �������� �ִ°�
		bool hasScale;

		float tickCount;
		float loadSpeed;

		bool isLoaded = false;

	public:
		EnemySpawn(std::weak_ptr<GameObject> _object);
		virtual ~EnemySpawn();

		// ������ ��, ��ǥ �������� �����ϵ��� �սô�.
		void Init(SimpleMath::Vector3 pos, float goalScale);

		void Update(float tick) override;
		void SetPlay(bool isPlay);

		// �ٸ� ������Ʈ���� ���� �ε� ������ �˾ƺ� �� �ֵ��� �ϴ� isLoading ������ ���� �� �ֵ��� �Ѵ�.
		bool GetIsLoading() { return isLoading; }
		void SetGoalScale(float goalScale) { this->goalScale = goalScale; }
		void SetGoalHeight(float goalHeight) { this->goalHeight = goalHeight; }
		float GetGoalHeight() { return goalHeight; }
		void SetHasScale(bool hasScale) { this->hasScale = hasScale; }
		bool GetIsLoadCompleted() { return isLoadCompleted; }
		void SetLoadCompleted() { isLoadCompleted = true; }
		bool GetIsLoaded() { return isLoaded; }

		void SetEmissiveColor(const SimpleMath::Vector4& color);
		void SetLoadSpeed(float speed) { this->loadSpeed = speed; }
		float GetLoadSpeed() { return this->loadSpeed; }
	};

}
