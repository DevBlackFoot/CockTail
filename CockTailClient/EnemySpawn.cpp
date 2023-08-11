#include "pch.h"
#include "EnemySpawn.h"

#include "../../FootGraphicsEngine/GraphicsEngineFramework.h"

#include "Component/SpriteEffect.h"
#include "GameManager.h"
#include "EnemyBehavior.h"

namespace ClientSpace
{
	EnemySpawn::EnemySpawn(std::weak_ptr<GameObject> _object)
		: ComponentBase(_object)
		, alpha(0.5f)
		, maskColor(0.8f, 0.4f, 0.9f, 1.0f)
		, goalScale(1.0f)
		, nowHeight(0.f)
		, goalHeight(2.9f)
		, eulerZ(0.f)
		, isScaleSet(false)
		, isEnd(false)
		, isPlay(false)
		, isLoading(false)
		, tickCount(0.f)
		, loadSpeed(2.5f)
		, isLoadCompleted(false)
		, hasScale(true)
	{
	}

	EnemySpawn::~EnemySpawn()
	{
	}

	void EnemySpawn::Init(SimpleMath::Vector3 pos, float goalScale)
	{
		// ���� ���� ĳ�� �� transform�� ����.
		auto gameObj = GetGameObject();

		transform = gameObj->GetComponent<Transform>();
		gameManager = GameManager::GetInstance();

		// �߰�
		spriteEffect = gameObj->AddComponent<GameEngineSpace::SpriteEffect>();
		spriteEffect->Init("SpawnEffect");
		spriteEffect->SetIsRender(false);

		maskColor.w = alpha;
		spriteEffect->SetMaskColor(maskColor);
		// Emissive ����
		spriteEffect->SetEmissiveColor({ 1.5f, 0.5f, 2.5f, 1.0f });

		// �ڽ��� �����ϰ� ���÷� ��´�.
		transform->SetPosition(pos, Space::LOCAL);
		transform->SetScale(Vector3{ 0.f, 0.f, 0.f });
		//transform->SetLocalEulerRotation(Vector3{ 90.f, 45.f, 0.f });

		this->goalScale = goalScale;

	}

	void EnemySpawn::Update(float tick)
	{
		// �׽�Ʈ��.
		if (tick <= 0.f || isPlay == false)
			return;

		// �� �������� �ɶ� ���� ȸ���ϸ鼭 �������� �÷��ݴϴ�.
		if (isScaleSet != true && hasScale == true)
		{
			float nowScale = transform->GetLocalScale().x;
			//auto nowRot = transform->GetLocalEulerRotation();

			alpha += 2.0f * tick;

			if (alpha > 1.0f)
				alpha = 1.f;

			nowScale += 0.5f * tick;
			if (nowScale >= goalScale)
			{
				nowScale = goalScale;
				alpha = 1.f;
				isScaleSet = true;
			}
			transform->SetScale(Vector3{ nowScale, nowScale, nowScale });

			eulerZ += 200.f * tick;

			maskColor.w = alpha;
			spriteEffect->SetMaskColor(maskColor);
			transform->SetLocalEulerRotation(Vector3{ 90.f, 45.f, eulerZ });
		}
		else if (isScaleSet != true && hasScale != true)
		{
			// �ٷ� �Ѱ��ݴϴ�.
			isScaleSet = true;
		}
		// ������ �Ǿ�����.. �÷��ݴϴ�.
		if (isEnd != true && isScaleSet == true)
		{
			tickCount += tick;
			if (tickCount < 0.7f)
				return;
			else
			{
				if (isLoading != true)
					isLoading = true;

				auto nowPos = transform->GetLocalPosition();

				// PBRObj�� Offset�� �����ϰ� ���ݴϴ�.
				nowHeight += loadSpeed * tick;
				if (nowHeight > goalHeight)
				{
					nowHeight = goalHeight;
					isEnd = true;
					tickCount = 0.f;
				}
				transform->SetPosition(Vector3{ nowPos.x, nowPos.y + loadSpeed * tick, nowPos.z }, Space::LOCAL);
			}
		}

		if (isEnd == true && isScaleSet == true)
		{
			tickCount += tick;
			if (tickCount > 1.0f)
			{
				spriteEffect->SetIsRender(false);
				isPlay = false;
				isLoadCompleted = true;
				isLoaded = true;
			}
		}
	}

	void EnemySpawn::SetPlay(bool isPlay)
	{
		// play�� ����Ǹ� ���� �����ϵ��� ���ݴϴ�.
		this->isPlay = isPlay;
		spriteEffect->SetIsRender(isPlay);
	}

	void EnemySpawn::SetEmissiveColor(const SimpleMath::Vector4& color)
	{
		spriteEffect->SetEmissiveColor(color);
	}
}
