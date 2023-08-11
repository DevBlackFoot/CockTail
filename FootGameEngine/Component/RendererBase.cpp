#include "GamePch.h"
#include "RendererBase.h"

#include "Object/GameObject.h"
#include "Transform.h"

#include "SceneManager.h"
#include "SceneBase.h"

namespace GameEngineSpace
{
	RendererBase::RendererBase(std::weak_ptr<GameObject> _object)
		: ComponentBase(_object, ComponentType::RENDERER)
		, isRender(true)
	{
		// Ʈ������ ����
		transform = GetGameObject()->GetComponent<Transform>();

		// �������� ��ӹ��� �ڽĵ鿡�� Ư��ȭ�� IDXObject�� ����. => ���丮�� ���� ���.
			// �����͸� ����� �ڽĿ��� ���丮�� ���� ���� �� ���⿡ �־��ش�..!
			// reset����ϸ� �ɵ�..!
		//m_RenderObj = std::make_shared<IDXObject>();

		// �⺻������ ��ƼŬ�� false�� �����صӴϴ�.
		isParticle = false;
	}

	RendererBase::~RendererBase()
	{

	}

	void RendererBase::Release()
	{
		transform.reset();
		__super::Release();
	}

	void RendererBase::Awake()
	{
		// �ش� �������� ���� ����Ѵ�.(��ƼŬ�� �ƴ� ���)
		if(isParticle != true)
			SceneManager::GetInstance()->GetCurrentScene()->AddRenderer(this->shared_from_this());
		// ��ƼŬ�� ���.. �ٸ��� ���ݴϴ�.
		else
			SceneManager::GetInstance()->GetCurrentScene()->AddParticle(this->shared_from_this());
	}

}
