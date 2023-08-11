#pragma once

#include "Interface/IBuilder.h"

#include "Define/GraphicsRenderingData.h"

namespace GraphicsEngineSpace
{

	class ResourceManager;

	/**
	 * \brief Particle Effect Object를 생성하는 빌더입니다.
	 */
	class ParticleEffectBuilder : public IBuilder
	{
		// 내부적으로 Device
		ComPtr<ID3D11Device> D3DDevice;
		ComPtr<ID3D11DeviceContext> D3DDeviceContext;

		// resource Manager 캐싱용
		std::shared_ptr<ResourceManager> resourceManager;

	public:
		ParticleEffectBuilder()
			: D3DDevice(nullptr)
			, D3DDeviceContext(nullptr)
			, resourceManager(nullptr)
		{
			
		}
		~ParticleEffectBuilder() override;

		virtual std::shared_ptr<IDXObject> BuildDXObject(std::shared_ptr<IDXObject> pDXObj, std::string objectName) override;
		// 파티클 이펙트는 해당 함수를 사용하지 않습니다..
		virtual std::shared_ptr<MeshResources> BuildGeometry(std::shared_ptr<IDXObject> DXObject, std::string objectPath) override;

		virtual void InitBuilder(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDC) override;
	};

}
