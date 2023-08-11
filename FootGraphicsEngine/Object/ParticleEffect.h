#pragma once
#include "Interface/IDXObject.h"
#include "Util/ParticleConstantBufferDesc.h"
#include "Resources/ParticleMesh.h"
namespace GraphicsEngineSpace
{
	class ParticleEffectBuilder;
	class ParticleMesh;
	class ResourceManager;
	class Texture;
	class VertexShader;
	class ComputeShader;
	class GeometryShader;
	class PixelShader;

	/**
	 * \brief 해당 클래스는 3D 공간에 파티클을 렌더하기 위해 만들어진 클래스입니다.
	 *
	 * 실질적으로 particle 렌더에 필요한 각종 내용을 가지고 업데이트 및 렌더를 해줍니다.
	 * 렌더를 전담해주는 Object.. emitter의 위치, 실질적 Draw..
	 * Particle 구조체와 구별하기 위해 Particle Effect라는 이름을 붙였습니다.
	 *
	 */
	class ParticleEffect : public IDXObject
	{
		// 멤버 변수 목록
		ComPtr<ID3D11Device> D3DDevice;
		ComPtr<ID3D11DeviceContext> D3DDeviceContext;

		// 파티클 메시 하나만 가져봅니다..
		std::vector<std::shared_ptr<ParticleMesh>> particleMeshes;

		// Resource Manager 캐싱
		std::shared_ptr<ResourceManager> resourceManager;

		// noise와 Random Texture를 오브젝트 하나를 가지고 있게 합니다.


		// CB로 들어갈 내용을 정리해야합니다.
			// PerObject의 경우 => 생성자에서 rendering data를 넣어주면 Particle 관련 CB가 정리가 되는 방식입니다.
		// 만들어둔 버퍼는 크게 두가지 입니다.
		ConstantBufferDesc::cbDynamicBuffer dynamicBuffer;

		// 시간 관련 상수 버퍼
		__declspec(align(16)) struct cbTime
		{
			float timeStep;
			float gamePlayTime;
			SimpleMath::Vector2 pad;
		};

		cbTime timeBuffer;

		// 사용할 셰이더들을 모두 가지고 있어봅니다..
		std::shared_ptr<VertexShader> particleVS;
		std::shared_ptr<ComputeShader> particleCS;
		std::shared_ptr<GeometryShader> particleGS;
		std::shared_ptr<PixelShader> particlePS;

		// 쉐이더에 전달할 매트릭스 => 파티클 셰이더에서는 다른 부분에서 전달이 됩니다..
		/*__declspec(align(16)) struct cbMatrix
		{
			SimpleMath::Matrix worldViewProj;
			SimpleMath::Matrix worldTransform;
			SimpleMath::Matrix worldInvTranspose;
		};
		*/
		
		// 여기의 World View Matrix는.. 에미터의 위치 => 게임 오브젝트의 위치라 가정합니다.
			// view proj을 만들어서 보내줘야 하기 때문에 살렸습니다..
		// TM 행렬
		SimpleMath::Matrix world;
		SimpleMath::Matrix view;
		SimpleMath::Matrix proj;
	public:
		// 복사 생성자
		ParticleEffect(const ParticleEffect& other)
		{
			for (const auto& particleMesh : other.particleMeshes)
			{
				particleMeshes.push_back(std::make_shared<ParticleMesh>(*particleMesh));
			}

			dynamicBuffer = other.dynamicBuffer;
			timeBuffer = other.timeBuffer;

			resourceManager = other.resourceManager;

			particleVS = other.particleVS;
			particleCS = other.particleCS;
			particleGS = other.particleGS;
			particlePS = other.particlePS;

			world = other.world;
			view = other.view;
			proj = other.proj;
			D3DDevice = other.D3DDevice;
			D3DDeviceContext = other.D3DDeviceContext;
		}

	public:
		// 멤버 함수 목록
		GRAPHICSENGINE_DECLSPEC ParticleEffect();
		~ParticleEffect();

		// 기본적으로 abstract로 되어있는 함수들을 정의해줍시다.
		virtual bool Init(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext) override;
		virtual void Update(const XMMATRIX& _world, const XMMATRIX& _view, const XMMATRIX& _proj) override;

		// 렌더 관련 함수들
		virtual void PreRender(float tick) override;
		virtual void Render() override;
		virtual void Render(std::shared_ptr<IRenderer> renderer) override;
		virtual void YieldRender(std::shared_ptr<IRenderer> renderer) override;

		// mesh Resource 관련 함수지만.. 파티클은 해당 구조체를 사용하지 않습니다..
		virtual const std::string& GetObjName() override;
		virtual std::vector<std::shared_ptr<MeshResources>> GetMeshResources() override;

		// 각종 매트릭스 Getter
		virtual const SimpleMath::Matrix& GetWorld() override { return SimpleMath::Matrix::Identity; }
		virtual const SimpleMath::Matrix& GetView() override { return SimpleMath::Matrix::Identity; }
		virtual const SimpleMath::Matrix& GetProj() override { return SimpleMath::Matrix::Identity; }

		// total Time 세팅
		virtual void SetTotalTime(float gameTotalTime) override { timeBuffer.gamePlayTime = gameTotalTime; }

		// 시작과 정지 => 일단 만들어보고 게임 엔진에서 작동하는지 봐야합니다.
		virtual void Play();
		virtual void Stop();
		virtual float GetPlayTime(int meshIdx);

		// 로테이션 세팅
		virtual void SetTexRotation(float radian);
		// 특정 메시의 텍스쳐를 바꿔줍니다.
		virtual void SetParticleTexture(int meshIdx, const std::shared_ptr<Texture>& texture);

		void SetParticleMesh(const std::shared_ptr<ParticleMesh>& particleMesh);

	private:
		// 각종 렌더를 함수로 분류하여 여기에 따로 넣어둡시다.
		void DispatchParticleCS(const std::shared_ptr<ParticleMesh>& mesh);

		void DrawParticle(std::shared_ptr<IRenderer> renderer, const std::shared_ptr<ParticleMesh>& mesh);
		void DrawTrail(std::shared_ptr<IRenderer> renderer, const std::shared_ptr<ParticleMesh>& mesh);

		friend ParticleEffectBuilder;
	};

}
