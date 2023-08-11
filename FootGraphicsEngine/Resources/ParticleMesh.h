#pragma once
#include "GraphicsCore/DX11GraphicsCore.h"
#include "Resources/Resource.h"
#include "GraphicsCore/UnorderedAccessResource.h"

namespace GraphicsEngineSpace
{
	// UAV를 사용하기 위해 전방선언을 해줍니다.
	class Texture;

	/**
	 * \brief 파티클 메시의 각종 정보들을 저장해두는 Particle Mesh 클래스입니다.
	 *
	 * 처음에는 Object와 합쳐보려 하였으나, 동작 자체가 다른 클래스였기 때문에 Resource영역으로 분리하였습니다.
	 */
	class ParticleMesh : public Resource
	{
		ComPtr<ID3D11Device> D3DDevice;
		ComPtr<ID3D11DeviceContext> D3DDeviceContext;

		// MeshResources를 따로 가지지 않고 자체적인 구조를 가지도록 해줍니다.
		std::shared_ptr<UnorderedAccessResource> particleUAResource;
		std::shared_ptr<UnorderedAccessResource> counterUAResource;

		// 카운터를 복사해 값을 확인하기 위한 버퍼
		ComPtr<ID3D11Buffer> copyCounterBuffer;

		// 성현이의 구조는 Rendering Data Particle을 받아서 업데이트 및 드로우를 해주는 구조..
			// 우리는 메시가 얘를 가지게 하면 되지 않을까..?
			// 사실상 파티클의 Mesh Resource 역할을 하는 Rendering data
		// Mesh 하나는 데이터 하나를 가지고 있습니다.
		std::shared_ptr<RenderingDataParticle> renderingData;

		// 여기서 텍스쳐를 가지도록 해줍니다.
		std::shared_ptr<Texture> particleTexture;
		std::shared_ptr<Texture> trailTexture;
		std::shared_ptr<Texture> noiseTexture;
		std::shared_ptr<Texture> randomTexture;

		// 버텍스 버퍼관련 변수
		D3D11_BUFFER_DESC vbd;
		// 파티클의 인덱스를 저장해 두는 버텍스 버퍼
		ComPtr<ID3D11Buffer> indexVB;

		// 토폴로지와 스테이트
		ComPtr<ID3D11RasterizerState> rasterState;
		D3D11_PRIMITIVE_TOPOLOGY topology;

		// 파티클에 대한 각종 정보
		unsigned int maxParticles;

		unsigned int emitterCount;

		unsigned int particleCount;

		int dim;

		// 파티클의 재생에 대한 각종 정보들
		bool isFirstRun;
		bool isPlay;
		bool isDelayStart;

		float playTime;		// 파티클의 재생 시간
		float delayTime;	// 파티클을 몇 초 뒤에 재생 시킬지..

		SimpleMath::Matrix prevMatrix;		// 한 프레임 동안 생긴 변화량에 대한 TM

		//초기 Rot 및 Scale 값.
		SimpleMath::Vector3 basePosition;
		SimpleMath::Vector3 baseScale;
		SimpleMath::Quaternion baseRotation;

	public:
		ParticleMesh();
		~ParticleMesh();

		// 파티클 복사 생성자 및 대입 연산자
		ParticleMesh(const ParticleMesh& other)
			: Resource(other.resourceType)
		{
			D3DDevice = other.D3DDevice;
			D3DDeviceContext = other.D3DDeviceContext;
			
			renderingData = std::make_shared<RenderingDataParticle>(*other.renderingData);

			particleUAResource = std::make_shared<UnorderedAccessResource>(*other.particleUAResource);
			counterUAResource = std::make_shared<UnorderedAccessResource>(*other.counterUAResource);
			copyCounterBuffer = other.copyCounterBuffer;

			particleTexture = other.particleTexture ? std::make_shared<Texture>(*other.particleTexture) : nullptr;
			trailTexture = other.trailTexture ? std::make_shared<Texture>(*other.trailTexture) : nullptr;
			noiseTexture = other.noiseTexture ? std::make_shared<Texture>(*other.noiseTexture) : nullptr;
			randomTexture = other.randomTexture ? std::make_shared<Texture>(*other.randomTexture) : nullptr;

			vbd = other.vbd;
			indexVB = other.indexVB;

			rasterState = other.rasterState;
			topology = other.topology;

			maxParticles = other.maxParticles;
			emitterCount = other.emitterCount;
			particleCount = other.particleCount;

			dim = other.dim;

			isFirstRun = other.isFirstRun;
			isPlay = other.isPlay;
			isDelayStart = other.isDelayStart;

			playTime = other.playTime;	
			delayTime = other.delayTime;

			prevMatrix = other.prevMatrix;

			basePosition = other.basePosition;
			baseScale = other.baseScale;
			baseRotation = other.baseRotation;
		}

		ParticleMesh& operator=(const ParticleMesh& other)
		{
			if (this != &other)
			{
				D3DDevice = other.D3DDevice;
				D3DDeviceContext = other.D3DDeviceContext;

				renderingData = std::make_shared<RenderingDataParticle>(*other.renderingData);

				particleUAResource = std::make_shared<UnorderedAccessResource>(*other.particleUAResource);
				counterUAResource = std::make_shared<UnorderedAccessResource>(*other.counterUAResource);
				copyCounterBuffer = other.copyCounterBuffer;

				particleTexture = other.particleTexture ? std::make_shared<Texture>(*other.particleTexture) : nullptr;
				trailTexture = other.trailTexture ? std::make_shared<Texture>(*other.trailTexture) : nullptr;
				noiseTexture = other.noiseTexture ? std::make_shared<Texture>(*other.noiseTexture) : nullptr;
				randomTexture = other.randomTexture ? std::make_shared<Texture>(*other.randomTexture) : nullptr;

				vbd = other.vbd;
				indexVB = other.indexVB;

				rasterState = other.rasterState;
				topology = other.topology;

				maxParticles = other.maxParticles;
				emitterCount = other.emitterCount;
				particleCount = other.particleCount;

				dim = other.dim;

				isFirstRun = other.isFirstRun;
				isPlay = other.isPlay;
				isDelayStart = other.isDelayStart;

				playTime = other.playTime;
				delayTime = other.delayTime;

				prevMatrix = other.prevMatrix;

				basePosition = other.basePosition;
				baseScale = other.baseScale;
				baseRotation = other.baseRotation;
			}
			return *this;
		}

		// 세팅 관련 각종 함수
	public:
		void SetMaxParticleSize(unsigned int size);

		void ParticleUpdate();

		void VSSetResource();

		void ResetParticleBuffer();
		void ResetCounter();

		int GetDim();
		unsigned int GetParticleCount();

		const ComPtr<ID3D11RasterizerState>& GetRasterizer() { return rasterState; }
		D3D11_PRIMITIVE_TOPOLOGY GetTopology() { return topology; }
		const std::shared_ptr<RenderingDataParticle>& GetRenderingData() { return renderingData; }

		const std::shared_ptr<Texture>& GetParticleTexture() { return particleTexture; }
		const std::shared_ptr<Texture>& GetTrailTexture() { return trailTexture; }
		const std::shared_ptr<Texture>& GetNoiseTexture() { return noiseTexture; }
		const std::shared_ptr<Texture>& GetRandomTexture() { return randomTexture; }

		// 각종 게터 세터
		bool GetIsPlay() { return isPlay; }
		bool GetIsFirstRun() { return isFirstRun; }
		bool GetIsDelayStart() { return isDelayStart; }
		float GetPlayTime() { return playTime; }

		void SetIsFirstRun(bool firstRun) { this->isFirstRun = firstRun; }

		void Play();
		void Stop();

		void PlayTimeSetting(float tick, const SimpleMath::Matrix& world);

		void SetParticleTexture(std::wstring fileName);
		void SetParticleTexture(const std::shared_ptr<Texture>& texture);
		void SetTrailTexture(std::wstring fileName);
		void CreateNoiseTexture();

		// Rendering Data 세팅
			// 밖에서 만들어서 넣어주는 방식으로 진행합니다.
		void SetRenderingData(const std::shared_ptr<RenderingDataParticle>& data) { this->renderingData = data; }
		void SetBaseScaleRotation();

		void SetWorldTM(const SimpleMath::Matrix& translateMatrix);
		// 이미지를 돌리기 위해..!
		void SetStartRocation(float radAngle);
	};
}

