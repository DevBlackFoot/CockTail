#pragma once
#include "Define/FootEngineTypedef.h"
#include "Util/ObjectPool.h"
//#include "../GraphicsEngine/GraphicsEngineFramework.h"

namespace GameEngineSpace
{
	class GameObject;

	class Camera;

	class RendererBase;

	class Collider;

	class Animator;

	class Light;

	/**
	 * \brief ���������� ���� ������Ʈ ���� ������ �ִ� ScenceBase
	 *
	 * �⺻������ ���� ��ġ�� ���� ������Ʈ�鸸�� update, Render �� �� �ִ�.
	 *
	 * Ŭ���̾�Ʈ �������� �ش� ���̽��� ��ӹ��� ������ ���� ���� ������ �ۼ��� �� �ִ�.
	 *
	 * 2022.07.25(��) Dev.BlackFoot
	 */
	class SceneBase : public std::enable_shared_from_this<SceneBase>
	{
		tstring sceneName;

		std::shared_ptr<Camera> mainCamera;

		std::vector<std::shared_ptr<GameObject>> gameObjectInScene;

		// ������ ���� ������Ʈ ��Ƶ� ����
		std::vector<std::shared_ptr<RendererBase>> renderObjInScene;

		// ����Ʈ ������Ʈ�� ��Ƶ� ����.

		// ��ƼŬ ������Ʈ���� ��Ƶ� ����
		std::vector<std::shared_ptr<RendererBase>> particleInScene;

		// �浹 ���� ������Ʈ ��Ƶ� ����
		std::vector<std::shared_ptr<Collider>> colliderObjInScene;

		// Scene�� ���ϳ� ����(�Ѵٰ� ����)�ϴ� mainLight
		std::shared_ptr<Light> mainLight;
		// ����Ʈ ����Ʈ + ���� ����Ʈ
		std::vector<std::shared_ptr<Light>> pointLightInScene;
		std::vector<std::shared_ptr<Light>> spotLightInScene;

		// ��ī�� �ڽ��� �޾��ݽô�.
			// �׷��Ƚ� ������ ���ҽ� �Ŵ������� �������ֱ� ������ �����͸� ������ش�.
		//GraphicsEngineSpace::SkyBoxBase* skyBox;
		// IBL�� ���� ����
		//GraphicsEngineSpace::IBLTexture* ibl;

		// ����� ������ �� �Լ� => ����ƽ���� ���� ��� ������ �����ϰ�.
		static bool debugRender;

		std::queue<std::shared_ptr<GameObject>> instantiateGameObjectList;

		std::queue<std::shared_ptr<GameObject>> destroyGameObjectList;

	protected:
		// ���� ���� �ǳ� ���� �ƴ���
		bool isInside;
		SimpleMath::Vector4 insideSceneMaskColor;
		SimpleMath::Vector4 rimLightColor;

	public:
		SceneBase(tstring sceneName = TEXT("SampleScene"));

		virtual ~SceneBase();
		// ���� �̺�Ʈ �Լ�..
			// ��κ��� ��� ������ ������Ʈ�� �Լ����� �ҷ��ش�.
		void Awake();
		void Start();
		void FixedUpdate(float tick);
		void Update(float tick = 0.0f);
		void LateUpdate(float tick = 0.0f);

		// �׸��� ���� Bake �غ��ϴ�.
		void BakeShadow(float tick);

		// ��� ������ ������Ʈ�� ������(�׷��� ����)�� �Ѱ��ش�.
		void Render(float tick);

		void OnEnable();

		// ī�޶� ����
		void SetMainCamera(std::shared_ptr<Camera> _camera) { mainCamera = _camera; }
		std::shared_ptr<Camera> GetMainCamera() { return mainCamera; }

		std::vector<std::shared_ptr<GameObject>> GetGameObjectInScene() { return gameObjectInScene; }
		std::shared_ptr<GameObject> FindObject(std::string uid);

		//
		//GraphicsEngineSpace::PointLight* CreatePointLight();

		virtual void BuildScene() abstract;

		void ClearScene();

	private:
		void AddGameObject(std::shared_ptr<GameObject> _newGameObj);


		// Debug Render �κ��� ���� ���ô�..
		void DebugRender(float tick);

	public:
		// �� ���� ������Ʈ�� �����Ѵ�.
		std::shared_ptr<GameObject> CreateEmpty();

		// ������ ���� �ְ� ����
		void InstantiateGameObject(std::shared_ptr<GameObject> _newGameObj);

		// ���� ����� ������Ʈ�� �ִٸ� �����Ѵ�.
		void ReservedInstantiateGameObject();
		
		// �ı� ����� ������Ʈ�� �ִٸ� �����Ѵ�.
		void ReservedDestroyGameObject();

		// ���ӿ�����Ʈ�� ������ �����մϴ�.
		void DestroyGameObject(std::shared_ptr<GameObject> _gameObj);

		// ����� TimeScale�� �ҷ��´�.
		void ReservedTimeScale();

		// ���� Ư�� ���� ������Ʈ�� �������� �ݶ��̴��� �߰��ϴ� �Լ��� �־��ش�
		// �ش� ������Ʈ�� ������ �ִ� ���ӿ�����Ʈ�� Awake�� �ҷ�����.
		void AddRenderer(std::shared_ptr<RendererBase> renderObj);
		void AddParticle(std::shared_ptr<RendererBase> particleObj);
		void AddCollider(std::shared_ptr<Collider> colliderObj);
		void AddPointLight(std::shared_ptr<Light> pLight);
		void AddSpotLight(std::shared_ptr<Light> sLight);
		void SetMainLight(std::shared_ptr<Light> dirLight) { mainLight = dirLight; }

		// debug ���� ����
		void SetDebugRender(bool debugRender) { this->debugRender = debugRender; }
		bool GetDebugRender() { return debugRender; }

		// �� �̸� ȹ��
		tstring GetSceneName() { return sceneName; }

		// SceneManager�� ���� �����ϱ� ���� friend Ŭ���� ����
		friend class SceneManager;
	};
}

