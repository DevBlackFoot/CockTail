#include "GraphicsPch.h"
#include "ParticleEffectBuilder.h"

#include "Resources/ParticleMesh.h"

#include "Object/ParticleEffect.h"

#include "Manager/ResourceManager.h"

namespace GraphicsEngineSpace
{
	ParticleEffectBuilder::~ParticleEffectBuilder()
	{
	}

	std::shared_ptr<IDXObject> ParticleEffectBuilder::BuildDXObject(std::shared_ptr<IDXObject> pDXObj,
		std::string objectName)
	{
		if (std::dynamic_pointer_cast<ParticleEffect>(pDXObj) == nullptr)
		{
			OutputDebugString(L"Not Particle");

			return nullptr;
		}

		// ���ο��� ��ƼŬ ���ҽ��� ����� �ݴϴ�.
			// �������� ���־���ϴ� ����. ��ƼŬ ����Ʈ ������Ʈ �ȿ�
			// ��ƼŬ Mesh�� ���� �־��ݴϴ�.
			// Mesh �ȿ� ��ƼŬ ������ �����͸� ����� �־��ݴϴ�.
		BuildGeometry(pDXObj, objectName);

		return pDXObj;
	}

	std::shared_ptr<MeshResources> ParticleEffectBuilder::BuildGeometry(std::shared_ptr<IDXObject> DXObject,
		std::string objectPath)
	{
		// ��ƼŬ �޽ø� ���� �־��ִ� ������ �մϴ�.

		// ��ƼŬ �޽� ��ü�� �����ϴ�.
			// �����ڿ� ��κ� ������ ����ֽ��ϴ�.
		
		// ���� ������Ʈ �̸��� ��ƼŬ�� �̸��̶� �����ϰ�..
		std::vector<RenderingDataParticle> data;

		std::ifstream ifs("BinaryFile/Particle/" + objectPath + ".dfx");
		

		if (ifs.is_open())
		{
			boost::archive::text_iarchive inArchive(ifs);

			inArchive >> data;

			ifs.close();
			
			// �ش� �޽ø� IDXobj�� �־��ݴϴ�.
			// friend�� ���⼭�� �����մϴ�.
			auto particleEffect = std::dynamic_pointer_cast<ParticleEffect>(DXObject);

			for(size_t i = 0; i < data.size(); i++)
			{
				std::shared_ptr<ParticleMesh> particleMesh = std::make_shared<ParticleMesh>();

				particleMesh->SetRenderingData(std::make_shared<RenderingDataParticle>(data[i]));
				particleEffect->SetParticleMesh(particleMesh);
			}
		}

		return nullptr;
	}

	void ParticleEffectBuilder::InitBuilder(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDC)
	{
		D3DDevice = pDevice;
		D3DDeviceContext = pDC;
	}
}
