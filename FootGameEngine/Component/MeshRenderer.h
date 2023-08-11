#pragma once
#include "RendererBase.h"
#include "Define/FootEngineTypedef.h"

namespace GameEngineSpace
{
	// 이 역시 Mesh 렌더러로 나올 수 있다.
		// 현재 내 구조가 특정 쉐이더를 사용하는 특정 빌더를 가져와서 만드는 구조기 때문에 이렇게 하는 것..
	class MeshRenderer : public RendererBase
	{
	private:
		bool isTrail = false;

	public:
		MeshRenderer(std::weak_ptr<GameObject> object);

		virtual ~MeshRenderer() {}

	public:
		// 빌더에 필요한 Init 함수.. 가변인자 템플릿을 사용하자.
		void Init(std::string objName);

		void LoadPrefab(std::string objName);

		void Render() override;
		void Render(std::shared_ptr<GraphicsEngineSpace::IRenderer> renderer) override;
		void YieldRender(std::shared_ptr<GraphicsEngineSpace::IRenderer> renderer) override;
		void PrepareRender(float tick) override;
		void DebugIMGUIRender(float tick) override;

		virtual void SetRimLightColor(SimpleMath::Vector4 color) override;

		void DeleteMaterial(std::string name);

		void SetMaterialColor(const std::string& matName, const SimpleMath::Vector4& color);
		void SetMaterialMetallicRoughness(const std::string& matName, float metallic, float roughness);

		void LightOff();
		void SetLoad();

		void SetIsTrail(bool isTrail) { this->isTrail = isTrail; }
		bool GetIsTrail() { return isTrail; }

		void SetGoalHeight(float height);
	};
}