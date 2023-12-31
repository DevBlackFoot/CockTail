#pragma once
#include "PassBase.h"
#include "Util/LightHelper.h"

namespace GraphicsEngineSpace
{
	// 쉐이더에 사용할 light buffer
	class VertexShader;
	class PixelShader;
	class RenderTargetTexture;
	class Texture;

	// Screen Render Target => Depth와 Viewport를 가지고 있다.
	class DepthStencilResource;

	class LightPass : public PassBase
	{
	private:
		std::shared_ptr<RenderTargetTexture> lightingRTV;
		std::shared_ptr<DepthStencilResource> lightingDSV;

		std::shared_ptr<VertexShader> quadVS;
		std::shared_ptr<PixelShader> PBRLightPS;
		std::shared_ptr<PixelShader> PBRLightShadowPS;
		std::shared_ptr<PixelShader> PBRLightIndoorPS;
		std::shared_ptr<DX11GraphicsCore> graphicsCore;

		bool isShadow = false;
		bool isIndoor = true;

		// 일단은 빛과 EyePos를 여기서 가지고 있어보자 -> 이후에 이전 가능
		SimpleMath::Vector3 eyePos;

		// 빛은 업데이트 되어야한다..? 한번 지나면 리셋
		PBR::DirectionalLight directionalLight;
		std::vector<PBR::PointLight> pointLights;
		std::vector<PBR::SpotLight> spotLights;

		// IBL을 하나밖에 사용하지 않기 때문에 LightPass에 넣어줍니다.
		std::shared_ptr<Texture> IBLDiffuse;
		std::shared_ptr<Texture> IBLSpecular;
		std::shared_ptr<Texture> IBLBRDFLookUp;

		SimpleMath::Vector4 ambientSkyColor = { 1.0f, 1.0f, 1.0f, 1.0f };
		SimpleMath::Vector4 IBLOffset = { 0.045f, 0.f, 0.f, 0.f };

		cbLight cbLightPass = cbLight();

	public:
		void Start(const std::shared_ptr<RenderTargetTexture>& mainRTV, const std::shared_ptr<DepthStencilResource>& mainDSV) override;
		void Release() override;

		void OnResize(int width, int height) override;

		void RenderStart();

		void Render(const std::vector<std::shared_ptr<RenderTargetTexture>>& renderTargets);
		void Render(const std::vector<std::shared_ptr<RenderTargetTexture>>& renderTargets, std::shared_ptr<DepthStencilResource> pointShadowResource, std::shared_ptr<DepthStencilResource> spotShadowResource);
		void Render(const std::vector<std::shared_ptr<RenderTargetTexture>>& renderTargets, std::shared_ptr<DepthStencilResource> pointShadowResource, std::shared_ptr<DepthStencilResource> spotShadowResource, std::shared_ptr<DepthStencilResource> bakedPointShadowResource, std::shared_ptr<DepthStencilResource> bakedSpotShadowResource);
		void Render(const std::vector<std::shared_ptr<RenderTargetTexture>>& renderTargets, const std::vector<std::shared_ptr<DepthStencilResource>>& shadowMaps);


		void EndRender();

		// Test용 IMGui 렌더
		void IMGuiRender();

		void UpdateEyePos(SimpleMath::Vector3 eyePos) { this->eyePos = eyePos; }

		void SetDirectionalLight(SimpleMath::Vector3 color, SimpleMath::Vector3 direction, float power, bool isShadow, SimpleMath::Matrix lightViewProj);
		const PBR::DirectionalLight& GetDirectionalLight() { return directionalLight; }

		void SetPointLight(SimpleMath::Vector3 color, SimpleMath::Vector3 position, float power, float range, bool isShadow, std::vector<SimpleMath::Matrix> lightViewProj);
		const std::vector<PBR::PointLight>& GetPointLights() { return pointLights; }

		void SetSpotLight(SimpleMath::Vector3 color, SimpleMath::Vector3 position, SimpleMath::Vector3 direction, float power, float innerSpotAngle, float outerSpotAngle, float range, bool
			isShadow, SimpleMath::Matrix lightViewProj);
		const std::vector<PBR::SpotLight>& GetSpotLights() { return spotLights; }

		const std::shared_ptr<RenderTargetTexture>& GetLightingRTV() { return lightingRTV; }
		void SetAmbientSkyColor(SimpleMath::Vector4 color);

		void SetEyePos(const SimpleMath::Vector3& eyePos);

		void SetIndoor(bool isIndoor) { this->isIndoor = isIndoor; }

		const cbLight& GetcbLight() { return cbLightPass; }
		void resetcbLight() { cbLightPass = cbLight(); }
		const SimpleMath::Vector4& GetAmbientSkyColor() { return ambientSkyColor; }
		const SimpleMath::Vector3& GetEyePos() { return eyePos; }
	};
}