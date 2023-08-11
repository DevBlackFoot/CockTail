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

		// 내부에서 파티클 리소스를 만들어 줍니다.
			// 빌더에서 해주어야하는 것은. 파티클 이펙트 오브젝트 안에
			// 파티클 Mesh를 만들어서 넣어줍니다.
			// Mesh 안에 파티클 렌더링 데이터를 만들어 넣어줍니다.
		BuildGeometry(pDXObj, objectName);

		return pDXObj;
	}

	std::shared_ptr<MeshResources> ParticleEffectBuilder::BuildGeometry(std::shared_ptr<IDXObject> DXObject,
		std::string objectPath)
	{
		// 파티클 메시를 만들어서 넣어주는 식으로 합니다.

		// 파티클 메시 자체를 만들어봅니다.
			// 생성자에 대부분 정보가 들어있습니다.
		
		// 들어온 오브젝트 이름이 파티클의 이름이라 가정하고..
		std::vector<RenderingDataParticle> data;

		std::ifstream ifs("BinaryFile/Particle/" + objectPath + ".dfx");
		

		if (ifs.is_open())
		{
			boost::archive::text_iarchive inArchive(ifs);

			inArchive >> data;

			ifs.close();
			
			// 해당 메시를 IDXobj에 넣어줍니다.
			// friend라 여기서도 가능합니다.
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
