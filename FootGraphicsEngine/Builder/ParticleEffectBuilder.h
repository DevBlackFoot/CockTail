#pragma once

#include "Interface/IBuilder.h"

#include "Define/GraphicsRenderingData.h"

namespace GraphicsEngineSpace
{

	class ResourceManager;

	/**
	 * \brief Particle Effect Object�� �����ϴ� �����Դϴ�.
	 */
	class ParticleEffectBuilder : public IBuilder
	{
		// ���������� Device
		ComPtr<ID3D11Device> D3DDevice;
		ComPtr<ID3D11DeviceContext> D3DDeviceContext;

		// resource Manager ĳ�̿�
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
		// ��ƼŬ ����Ʈ�� �ش� �Լ��� ������� �ʽ��ϴ�..
		virtual std::shared_ptr<MeshResources> BuildGeometry(std::shared_ptr<IDXObject> DXObject, std::string objectPath) override;

		virtual void InitBuilder(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDC) override;
	};

}
