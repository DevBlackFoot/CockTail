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
	 * \brief �ش� Ŭ������ 3D ������ ��ƼŬ�� �����ϱ� ���� ������� Ŭ�����Դϴ�.
	 *
	 * ���������� particle ������ �ʿ��� ���� ������ ������ ������Ʈ �� ������ ���ݴϴ�.
	 * ������ �������ִ� Object.. emitter�� ��ġ, ������ Draw..
	 * Particle ����ü�� �����ϱ� ���� Particle Effect��� �̸��� �ٿ����ϴ�.
	 *
	 */
	class ParticleEffect : public IDXObject
	{
		// ��� ���� ���
		ComPtr<ID3D11Device> D3DDevice;
		ComPtr<ID3D11DeviceContext> D3DDeviceContext;

		// ��ƼŬ �޽� �ϳ��� �������ϴ�..
		std::vector<std::shared_ptr<ParticleMesh>> particleMeshes;

		// Resource Manager ĳ��
		std::shared_ptr<ResourceManager> resourceManager;

		// noise�� Random Texture�� ������Ʈ �ϳ��� ������ �ְ� �մϴ�.


		// CB�� �� ������ �����ؾ��մϴ�.
			// PerObject�� ��� => �����ڿ��� rendering data�� �־��ָ� Particle ���� CB�� ������ �Ǵ� ����Դϴ�.
		// ������ ���۴� ũ�� �ΰ��� �Դϴ�.
		ConstantBufferDesc::cbDynamicBuffer dynamicBuffer;

		// �ð� ���� ��� ����
		__declspec(align(16)) struct cbTime
		{
			float timeStep;
			float gamePlayTime;
			SimpleMath::Vector2 pad;
		};

		cbTime timeBuffer;

		// ����� ���̴����� ��� ������ �־�ϴ�..
		std::shared_ptr<VertexShader> particleVS;
		std::shared_ptr<ComputeShader> particleCS;
		std::shared_ptr<GeometryShader> particleGS;
		std::shared_ptr<PixelShader> particlePS;

		// ���̴��� ������ ��Ʈ���� => ��ƼŬ ���̴������� �ٸ� �κп��� ������ �˴ϴ�..
		/*__declspec(align(16)) struct cbMatrix
		{
			SimpleMath::Matrix worldViewProj;
			SimpleMath::Matrix worldTransform;
			SimpleMath::Matrix worldInvTranspose;
		};
		*/
		
		// ������ World View Matrix��.. �������� ��ġ => ���� ������Ʈ�� ��ġ�� �����մϴ�.
			// view proj�� ���� ������� �ϱ� ������ ��Ƚ��ϴ�..
		// TM ���
		SimpleMath::Matrix world;
		SimpleMath::Matrix view;
		SimpleMath::Matrix proj;
	public:
		// ���� ������
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
		// ��� �Լ� ���
		GRAPHICSENGINE_DECLSPEC ParticleEffect();
		~ParticleEffect();

		// �⺻������ abstract�� �Ǿ��ִ� �Լ����� �������ݽô�.
		virtual bool Init(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext) override;
		virtual void Update(const XMMATRIX& _world, const XMMATRIX& _view, const XMMATRIX& _proj) override;

		// ���� ���� �Լ���
		virtual void PreRender(float tick) override;
		virtual void Render() override;
		virtual void Render(std::shared_ptr<IRenderer> renderer) override;
		virtual void YieldRender(std::shared_ptr<IRenderer> renderer) override;

		// mesh Resource ���� �Լ�����.. ��ƼŬ�� �ش� ����ü�� ������� �ʽ��ϴ�..
		virtual const std::string& GetObjName() override;
		virtual std::vector<std::shared_ptr<MeshResources>> GetMeshResources() override;

		// ���� ��Ʈ���� Getter
		virtual const SimpleMath::Matrix& GetWorld() override { return SimpleMath::Matrix::Identity; }
		virtual const SimpleMath::Matrix& GetView() override { return SimpleMath::Matrix::Identity; }
		virtual const SimpleMath::Matrix& GetProj() override { return SimpleMath::Matrix::Identity; }

		// total Time ����
		virtual void SetTotalTime(float gameTotalTime) override { timeBuffer.gamePlayTime = gameTotalTime; }

		// ���۰� ���� => �ϴ� ������ ���� �������� �۵��ϴ��� �����մϴ�.
		virtual void Play();
		virtual void Stop();
		virtual float GetPlayTime(int meshIdx);

		// �����̼� ����
		virtual void SetTexRotation(float radian);
		// Ư�� �޽��� �ؽ��ĸ� �ٲ��ݴϴ�.
		virtual void SetParticleTexture(int meshIdx, const std::shared_ptr<Texture>& texture);

		void SetParticleMesh(const std::shared_ptr<ParticleMesh>& particleMesh);

	private:
		// ���� ������ �Լ��� �з��Ͽ� ���⿡ ���� �־�ӽô�.
		void DispatchParticleCS(const std::shared_ptr<ParticleMesh>& mesh);

		void DrawParticle(std::shared_ptr<IRenderer> renderer, const std::shared_ptr<ParticleMesh>& mesh);
		void DrawTrail(std::shared_ptr<IRenderer> renderer, const std::shared_ptr<ParticleMesh>& mesh);

		friend ParticleEffectBuilder;
	};

}
