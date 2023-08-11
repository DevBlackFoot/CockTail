#include "GamePch.h"
#include "PosColorRenderer.h"

#include "../../FootGraphicsEngine/GraphicsEngineFramework.h"

#include "Camera.h"
#include "Object/GameObject.h"
#include "SceneBase.h"
#include "SceneManager.h"
#include "GraphicsManager.h"
#include "InputManager.h"

using namespace GraphicsEngineSpace;

namespace GameEngineSpace
{
	PosColorRenderer::PosColorRenderer(std::weak_ptr<GameObject> object)
		: RendererBase(object)
	{
	}

	void PosColorRenderer::Init(std::string _objName)
	{
		// 이 지점에서 새로 리셋
		renderObj = Factory::GetInstance()->CreateDXObject<LineObj>(
		BuilderManager::GetInstance()->GetBuilder("LineBuilder"), _objName);

		// 그려지는 오브젝트로 넣어줍니다.
		GraphicsManager::GetInstance()->GetRenderer()->InitObject(renderObj);
	}

	void PosColorRenderer::Render()
	{
		if(isRender == true)
			renderObj->Render();
	}

	void PosColorRenderer::Render(std::shared_ptr<GraphicsEngineSpace::IRenderer> renderer)
	{
		if(isRender == true)
			renderObj->Render(renderer);
	}

	void PosColorRenderer::YieldRender(std::shared_ptr<GraphicsEngineSpace::IRenderer> renderer)
	{
		if(isRender == true)
			renderObj->YieldRender(renderer);
	}

	void PosColorRenderer::PrepareRender(float tick)
	{
		if (isRender != true)
			return;

		// 씬 매니저에서 얻어와서 오브젝트 업데이트
		std::shared_ptr<Camera> mainCam = SceneManager::GetInstance()->GetCurrentScene()->GetMainCamera();
	
		renderObj->Update(transform->GetWorldTM(), mainCam->GetViewMatrix(), mainCam->GetProjMatrix());
	}
}
