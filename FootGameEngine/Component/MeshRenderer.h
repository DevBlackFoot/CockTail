#pragma once
#include "RendererBase.h"
#include "Define/FootEngineTypedef.h"

namespace GameEngineSpace
{
	// �� ���� Mesh �������� ���� �� �ִ�.
		// ���� �� ������ Ư�� ���̴��� ����ϴ� Ư�� ������ �����ͼ� ����� ������ ������ �̷��� �ϴ� ��..
	class MeshRenderer : public RendererBase
	{
	private:
		bool isTrail = false;

	public:
		MeshRenderer(std::weak_ptr<GameObject> object);

		virtual ~MeshRenderer() {}

	public:
		// ������ �ʿ��� Init �Լ�.. �������� ���ø��� �������.
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