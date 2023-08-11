#pragma once
#include "GraphicsCore/DX11GraphicsCore.h"
#include "Resources/Resource.h"
#include "GraphicsCore/UnorderedAccessResource.h"

namespace GraphicsEngineSpace
{
	// UAV�� ����ϱ� ���� ���漱���� ���ݴϴ�.
	class Texture;

	/**
	 * \brief ��ƼŬ �޽��� ���� �������� �����صδ� Particle Mesh Ŭ�����Դϴ�.
	 *
	 * ó������ Object�� ���ĺ��� �Ͽ�����, ���� ��ü�� �ٸ� Ŭ�������� ������ Resource�������� �и��Ͽ����ϴ�.
	 */
	class ParticleMesh : public Resource
	{
		ComPtr<ID3D11Device> D3DDevice;
		ComPtr<ID3D11DeviceContext> D3DDeviceContext;

		// MeshResources�� ���� ������ �ʰ� ��ü���� ������ �������� ���ݴϴ�.
		std::shared_ptr<UnorderedAccessResource> particleUAResource;
		std::shared_ptr<UnorderedAccessResource> counterUAResource;

		// ī���͸� ������ ���� Ȯ���ϱ� ���� ����
		ComPtr<ID3D11Buffer> copyCounterBuffer;

		// �������� ������ Rendering Data Particle�� �޾Ƽ� ������Ʈ �� ��ο츦 ���ִ� ����..
			// �츮�� �޽ð� �긦 ������ �ϸ� ���� ������..?
			// ��ǻ� ��ƼŬ�� Mesh Resource ������ �ϴ� Rendering data
		// Mesh �ϳ��� ������ �ϳ��� ������ �ֽ��ϴ�.
		std::shared_ptr<RenderingDataParticle> renderingData;

		// ���⼭ �ؽ��ĸ� �������� ���ݴϴ�.
		std::shared_ptr<Texture> particleTexture;
		std::shared_ptr<Texture> trailTexture;
		std::shared_ptr<Texture> noiseTexture;
		std::shared_ptr<Texture> randomTexture;

		// ���ؽ� ���۰��� ����
		D3D11_BUFFER_DESC vbd;
		// ��ƼŬ�� �ε����� ������ �δ� ���ؽ� ����
		ComPtr<ID3D11Buffer> indexVB;

		// ���������� ������Ʈ
		ComPtr<ID3D11RasterizerState> rasterState;
		D3D11_PRIMITIVE_TOPOLOGY topology;

		// ��ƼŬ�� ���� ���� ����
		unsigned int maxParticles;

		unsigned int emitterCount;

		unsigned int particleCount;

		int dim;

		// ��ƼŬ�� ����� ���� ���� ������
		bool isFirstRun;
		bool isPlay;
		bool isDelayStart;

		float playTime;		// ��ƼŬ�� ��� �ð�
		float delayTime;	// ��ƼŬ�� �� �� �ڿ� ��� ��ų��..

		SimpleMath::Matrix prevMatrix;		// �� ������ ���� ���� ��ȭ���� ���� TM

		//�ʱ� Rot �� Scale ��.
		SimpleMath::Vector3 basePosition;
		SimpleMath::Vector3 baseScale;
		SimpleMath::Quaternion baseRotation;

	public:
		ParticleMesh();
		~ParticleMesh();

		// ��ƼŬ ���� ������ �� ���� ������
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

		// ���� ���� ���� �Լ�
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

		// ���� ���� ����
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

		// Rendering Data ����
			// �ۿ��� ���� �־��ִ� ������� �����մϴ�.
		void SetRenderingData(const std::shared_ptr<RenderingDataParticle>& data) { this->renderingData = data; }
		void SetBaseScaleRotation();

		void SetWorldTM(const SimpleMath::Matrix& translateMatrix);
		// �̹����� ������ ����..!
		void SetStartRocation(float radAngle);
	};
}

