#include "GamePch.h"
#include "MeshRenderer.h"

#include "../../FootGraphicsEngine/Interface/IDXObject.h"
#include "../../FootGraphicsEngine/GraphicsEngineFramework.h"

#include "Component/Camera.h"
#include "Object/GameObject.h"
#include "SceneBase.h"
#include "SceneManager.h"
#include "GraphicsManager.h"
#include "InputManager.h"

using namespace GraphicsEngineSpace;

namespace GameEngineSpace
{
	MeshRenderer::MeshRenderer(std::weak_ptr<GameObject> object)
		: RendererBase(object)
	{
	}

	void MeshRenderer::Init(std::string objName)
	{
		renderObj = Factory::GetInstance()->CreateDXObject<PBRObj>(
		BuilderManager::GetInstance()->GetBuilder("PBRBuilder"), objName);

		GraphicsManager::GetInstance()->GetRenderer()->InitObject(renderObj);
	}

	void MeshRenderer::LoadPrefab(std::string objName)
	{
		// �������� ��������.. ��
		renderObj = ResourceManager::GetInstance()->GetPrefab(objName);	
	}

	void MeshRenderer::Render()
	{
		renderObj->Render();
	}

	void MeshRenderer::Render(std::shared_ptr<GraphicsEngineSpace::IRenderer> renderer)
	{
		if(isTrail == true)
			return;

		renderObj->Render(renderer);
	}

	void MeshRenderer::YieldRender(std::shared_ptr<GraphicsEngineSpace::IRenderer> renderer)
	{
		if(isTrail == true)
			renderObj->ObjectTrailRender(renderer);
		else
			renderObj->YieldRender(renderer);
	}

	void MeshRenderer::PrepareRender(float tick)
	{
		std::shared_ptr<Camera> mainCam = SceneManager::GetInstance()->GetCurrentScene()->GetMainCamera();

		//if (transform->isInterpolate)
		//{
		//	auto& interpolateQueue = transform->interpolateQueue;

		//	if (!interpolateQueue.empty())
		//	{
		//		// ��¥ Ʈ�������� �ٲ��ָ� �ȵȴ�.. ��..
		//		// �������Ҷ� �� ������ worldTm���� ������༭ render->Update�� �Ѱ�����Ѵ�.
		//		// ��, ������ �Ҷ��� interpolate ������ �ִ°�..?
		//		// ���� ��ȯ(transform)�� ����(interpolate)���� �ʰ� ������(rendering) �߿��� ����
		//		// �̴� ���� ������Ʈ�� ������ ������Ʈ ������ �ð� ���̸� �����ϱ� ���� ���Ǵ� ��..
		//		// ���� �ùķ��̼��� ������ ���� ����Ͽ� ���ǰ�, �� ����� �������� ���� ���� ��ȯ ��
		//		//transform->SetPosition(interpolateQueue.front().position);

		//		//transform->SetRotation(interpolateQueue.front().rotation);

		//		auto worldTM = Matrix::CreateFromQuaternion(interpolateQueue.front().rotation) *
		//			Matrix::CreateTranslation(interpolateQueue.front().position);

		//		transform->interpolateQueue.pop();

		//		renderObj->Update(worldTM, mainCam->GetViewMatrix(), mainCam->GetProjMatrix());
		//	}
		//}
		//else
		//{
		//}

		renderObj->Update(transform->GetWorldTM(), mainCam->GetViewMatrix(), mainCam->GetProjMatrix());

		renderObj->PreRender(tick);
	}

	void MeshRenderer::DebugIMGUIRender(float tick)
	{
		/*auto imgui = ImGUIManager::GetInstance();

		auto gameObjName = GetGameObject()->GetName();
		std::string name = "";
		name.assign(gameObjName.begin(), gameObjName.end());

		if (imgui->TreeNode("Sprite Effect Info", "Mesh" + name))
		{
			auto pos = transform->GetWorldPosition();
			float posf[3] = { pos.x, pos.y, pos.z };

			auto scale = transform->GetWorldScale();
			float scalef[3] = { scale.x, scale.y, scale.z };

			auto rot = transform->GetLocalEulerRotation();
			float rotf[3] = { rot.x, rot.y, rot.z };

			imgui->DragFloat3("Position", posf, 0.5f);
			imgui->DragFloat3("Scale", scalef, 0.1f, 1.f, 20.f);
			imgui->DragFloat3("Rotation", rotf, 5.0f, -360.f, 360.f);

			transform->SetPosition(Vector3{ posf[0], posf[1], posf[2] });
			transform->SetScale(Vector3{ scalef[0], scalef[1], scalef[2] });
			transform->SetLocalEulerRotation(Vector3{ rotf[0], rotf[1], rotf[2] });

			imgui->TreePop();
			imgui->Separator();
		}*/
	}

	void MeshRenderer::SetRimLightColor(SimpleMath::Vector4 color)
	{
		renderObj->SetRimColor(color);
	}

	void MeshRenderer::DeleteMaterial(std::string name)
	{
		renderObj->DeleteMaterial(name);
	}

	void MeshRenderer::SetMaterialColor(const std::string& matName, const SimpleMath::Vector4& color)
	{
		renderObj->SetMaterialColor(matName, color);
	}

	void MeshRenderer::SetMaterialMetallicRoughness(const std::string& matName, float metallic, float roughness)
	{
		renderObj->SetMaterialMetallicRoughness(matName, metallic, roughness);
	}

	void MeshRenderer::LightOff()
	{
		renderObj->LightOff();
	}

	void MeshRenderer::SetLoad()
	{
		// �ε� �������� ������ݴϴ�.
		renderObj->SetLoad(true);
	}

	void MeshRenderer::SetGoalHeight(float height)
	{
		renderObj->SetGoalHeight(height);
	}
}
