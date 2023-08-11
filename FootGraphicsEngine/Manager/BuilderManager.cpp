#include "GraphicsPch.h"
#include "BuilderManager.h"
#include "Interface/IBuilder.h"

// �߰��� �������� include ���ش�.
	// TODO : �ð� �� �� ���� �� �ִ� �͵��� �����
	// �ϴ� ����� �ڵ带 �Űܺ���.
#include "Builder/LineBuilder.h"
#include "Builder/SkyBoxBuilder.h"
#include "Builder/PBRBuilder.h"
#include "Builder/SpriteEffectBuilder.h"
#include "Builder/ParticleEffectBuilder.h"

namespace GraphicsEngineSpace
{
	std::shared_ptr<BuilderManager> BuilderManager::instance = nullptr;

	void BuilderManager::InitBuilderAll(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext)
	{
		// ����ó��
		assert(pDevice);
		assert(pDeviceContext);
		assert(RasterizerState::GetSolidRS());
		assert(RasterizerState::GetWireFrameRS());

		// ��� ���� �ʱ�ȭ.
		D3DDevice = pDevice;
		D3DDeviceContext = pDeviceContext;

		// ������ ���ø� �Լ��� �־��
		AddBuilder<LineBuilder>("LineBuilder");
		AddBuilder<SkyBoxBuilder>("SkyBoxBuilder");
		AddBuilder<PBRBuilder>("PBRBuilder");
		AddBuilder<SpriteEffectBuilder>("SpriteEffectBuilder");
		AddBuilder<ParticleEffectBuilder>("ParticleEffectBuilder");
	}


	void BuilderManager::DeleteBuildManager()
	{
		// ���� ��ȸ�ϸ鼭 ��� ������ �����ش�.
		for (auto _builder : builderMap)
		{
			SafeReset(_builder.second)
		}

		builderMap.clear();

		if (instance != nullptr)
		{
			SafeReset(instance);
		}
	}

	std::shared_ptr<IBuilder> BuilderManager::GetBuilder(std::string _builderName)
	{
		// Ű�� �´� ������ ã�Ƽ� ��������
		return builderMap.at(_builderName);
	}

}
