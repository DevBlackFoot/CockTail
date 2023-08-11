#include "GraphicsPch.h"
#include "ShaderManager.h"
#include "Object/Shader/VertexShader.h"
#include "Object/Shader/PixelShader.h"
#include "Object/Shader/GeometryShader.h"
#include "Object/Shader/ComputeShader.h"
#include "Resources/InputLayout.h"

#include "Util/ReadData.h"

namespace GraphicsEngineSpace
{
	std::shared_ptr<ShaderManager> ShaderManager::instance = nullptr;

	ShaderManager::~ShaderManager()
	{
		if (shaderBlob != nullptr)
			shaderBlob->Release();
	}

	std::shared_ptr<ShaderManager> ShaderManager::GetInstance()
	{
		if (instance == nullptr)
			instance = std::make_shared<ShaderManager>();

		return instance;
	}

	void ShaderManager::Init(ComPtr<ID3D11Device> _device, ComPtr<ID3D11DeviceContext> _deviceContext)
	{
		device = _device;
		deviceContext = _deviceContext;

		// path ����
		shaderPath = "Shader/";
#if defined(DEBUG) || defined(_DEBUG)
		shaderPath += "CompiledShader_Debug/";
#else
		shaderPath += "CompiledShader_Release/";
#endif
	}

	bool ShaderManager::CompileAllShader()
	{
		// �ϴ��� �ϵ��ڵ�����
			// ���Ŀ� ������ ���� �� ������.. ���� �Լ��� �ϳ� �Ĵ� �͵� ������ ������.
		bool isAllShaderCompile = true;

		// shaderBlob�� ���̴� Ŭ������ ��ü�� ���� �� ���� �ٲ��־���Ѵ�.
			// �װ� ���⼭ ������ ������ִ� ������...
			// ���̴��� ���鶧 ���ø� �Լ��� �θ��� ������ �ϱ⵵ ������.. shaderManager�� �ʿ� �־��ֱ� ���ؼ� ���⼭ �غ��� ������
		// ���ؽ����̴� ����
		isAllShaderCompile &= CreateVertexShader("Shader/LineShader/LineVS.hlsl", "main", "vs_5_0", "LineVS", InputLayoutDesc::Pos, 1, nullptr);
		isAllShaderCompile &= CreateVertexShader("Shader/LineShader/BasicColorVS.hlsl", "main", "vs_5_0", "BasicColorVS", InputLayoutDesc::PosColor, 2, nullptr);
		isAllShaderCompile &= CreateVertexShader("Shader/SkyBoxShader/SkyBoxVS.hlsl", "main", "vs_5_0", "SkyBoxVS", InputLayoutDesc::Pos, 1, nullptr);
		isAllShaderCompile &= CreateVertexShader("Shader/SpriteEffectShader/SpriteEffectVS.hlsl", "main", "vs_5_0", "SpriteEffectVS", InputLayoutDesc::PosTex, 2, nullptr);

		// PBRShader
		D3D_SHADER_MACRO skinned_MacroVS[] = { {"Skinned"}, {NULL, NULL} };
		D3D_SHADER_MACRO skinned_transparent_MacroVS[] = { {"Skinned"}, {"TRANSPARENTOBJ"}, {NULL, NULL} };
		isAllShaderCompile &= CreateVertexShader("Shader/PBRShader/PBRModelVS.hlsl", "main", "vs_5_0", "PBRStaticVS", InputLayoutDesc::PBRStatic, 5, nullptr);
		isAllShaderCompile &= CreateVertexShader("Shader/PBRShader/PBRModelVS.hlsl", "main", "vs_5_0", "PBRSkinnedVS", InputLayoutDesc::PBRSkinned, 9, skinned_MacroVS);
		isAllShaderCompile &= CreateVertexShader("Shader/PBRShader/PBRModelVS.hlsl", "main", "vs_5_0", "PBRAlphaSkinnedVS", InputLayoutDesc::PBRSkinned, 9, skinned_transparent_MacroVS);
		isAllShaderCompile &= CreateVertexShader("Shader/Deferred/QuadVS.hlsl", "main", "vs_5_0", "QuadVS", InputLayoutDesc::PosTex, 2, nullptr);

		// Shadow
		D3D_SHADER_MACRO dir_MacroVS[] = { {"DirLightShadow"}, {NULL, NULL} };
		D3D_SHADER_MACRO skinned_Dir_MacroVS[] = { {"Skinned"}, {"DirLightShadow"}, {NULL, NULL} };
		isAllShaderCompile &= CreateVertexShader("Shader/Deferred/ShadowVS.hlsl", "main", "vs_5_0", "ShadowStaticVS", InputLayoutDesc::PBRStatic, 5, dir_MacroVS);
		isAllShaderCompile &= CreateVertexShader("Shader/Deferred/ShadowVS.hlsl", "main", "vs_5_0", "ShadowSkinnedVS", InputLayoutDesc::PBRSkinned, 9, skinned_Dir_MacroVS);
		// Geo�� ���� �͵��� PointLight shadow�� ���� ���ؽ� ���̴��̴�..
			// => Geometry ���̴����� �� �鿡 �ش��ϴ� LightViewProj�� �������ش�.
		isAllShaderCompile &= CreateVertexShader("Shader/Deferred/ShadowVS.hlsl", "main", "vs_5_0", "GeoShadowStaticVS", InputLayoutDesc::PBRStatic, 5, nullptr);
		isAllShaderCompile &= CreateVertexShader("Shader/Deferred/ShadowVS.hlsl", "main", "vs_5_0", "GeoShadowSkinnedVS", InputLayoutDesc::PBRSkinned, 9, skinned_MacroVS);

		// �׸��ڿ� ���ϼ��̴� ����
		isAllShaderCompile &= CreateGeometryShader("Shader/Deferred/ShadowGS.hlsl", "main", "gs_5_0", "ShadowGS", nullptr);

		// �ȼ����̴� ����
		isAllShaderCompile &= CreatePixelShader("Shader/LineShader/LinePS.hlsl", "main", "ps_5_0", "LinePS", nullptr);
		isAllShaderCompile &= CreatePixelShader("Shader/LineShader/BasicColorPS.hlsl", "main", "ps_5_0", "BasicColorPS", nullptr);
		isAllShaderCompile &= CreatePixelShader("Shader/SpriteEffectShader/SpriteEffectPS.hlsl", "main", "ps_5_0", "SpriteEffectPS", nullptr);

		// Macro
		D3D_SHADER_MACRO model_MacroPS_1[] = { {"USE_ALBEDO"}, {NULL, NULL} };
		D3D_SHADER_MACRO model_MacroPS_2[] = { {"USE_ALBEDO"}, {"USE_NORMAL"}, {NULL, NULL} };
		D3D_SHADER_MACRO model_MacroPS_3[] = { {"USE_ALBEDO"}, {"USE_METALLIC"}, {"USE_ROUGHNESS"}, {NULL, NULL} };
		D3D_SHADER_MACRO model_MacroPS_4[] = { {"USE_ALBEDO"}, {"USE_NORMAL"}, {"USE_METALLIC"}, {"USE_ROUGHNESS"}, {NULL, NULL} };
		D3D_SHADER_MACRO model_MacroPS_5[] = { {"USE_ALBEDO"}, {"USE_NORMAL"}, {"USE_AO"}, {NULL, NULL} };
		D3D_SHADER_MACRO model_MacroPS_6[] = { {"USE_ALBEDO"}, {"USE_NORMAL"}, {"USE_METALLIC"}, {"USE_ROUGHNESS"}, {"USE_AO"}, {NULL, NULL} };
		D3D_SHADER_MACRO model_MacroPS_7[] = { {"USE_ALBEDO"}, {"USE_NORMAL"}, {"USE_METALLIC"}, {"USE_ROUGHNESS"}, {"USE_AO"}, {"USE_EMISSIVE"}, {NULL, NULL} };
		D3D_SHADER_MACRO model_MacroPS_8[] = { {"TRANSPARENCYZTEST"}, {NULL, NULL} };
		D3D_SHADER_MACRO model_MacroPS_9[] = { {"SILHOUETTE"}, {NULL, NULL} };


		D3D_SHADER_MACRO modelSkin_MacroPS_0[] = { {"IsSkinned"}, {NULL, NULL} };
		D3D_SHADER_MACRO modelSkin_MacroPS_1[] = { {"USE_ALBEDO"}, {"IsSkinned"}, {NULL, NULL} };
		D3D_SHADER_MACRO modelSkin_MacroPS_2[] = { {"USE_ALBEDO"}, {"IsSkinned"}, {"USE_NORMAL"}, {NULL, NULL} };
		D3D_SHADER_MACRO modelSkin_MacroPS_3[] = { {"USE_ALBEDO"}, {"IsSkinned"}, {"USE_METALLIC"}, {"USE_ROUGHNESS"}, {NULL, NULL} };
		D3D_SHADER_MACRO modelSkin_MacroPS_4[] = { {"USE_ALBEDO"}, {"IsSkinned"}, {"USE_NORMAL"}, {"USE_METALLIC"}, {"USE_ROUGHNESS"}, {NULL, NULL} };
		D3D_SHADER_MACRO modelSkin_MacroPS_5[] = { {"USE_ALBEDO"}, {"IsSkinned"}, {"USE_NORMAL"}, {"USE_AO"}, {NULL, NULL} };
		D3D_SHADER_MACRO modelSkin_MacroPS_6[] = { {"USE_ALBEDO"}, {"IsSkinned"}, {"USE_NORMAL"}, {"USE_METALLIC"}, {"USE_ROUGHNESS"}, {"USE_AO"}, {NULL, NULL} };
		D3D_SHADER_MACRO modelSkin_MacroPS_7[] = { {"USE_ALBEDO"}, {"IsSkinned"}, {"USE_NORMAL"}, {"USE_METALLIC"}, {"USE_ROUGHNESS"}, {"USE_AO"}, {"USE_EMISSIVE"}, {NULL, NULL} };

		D3D_SHADER_MACRO model_Burn[] = { {"USE_PAPERBURN"}, {NULL, NULL} };
		D3D_SHADER_MACRO modelSkin_Burn[] = { {"USE_PAPERBURN"}, {"IsSkinned"}, {NULL, NULL} };
		D3D_SHADER_MACRO modelSkin_Burn_Albedo[] = { {"USE_ALBEDO"}, {"USE_PAPERBURN"}, {"IsSkinned"}, {NULL, NULL} };
		D3D_SHADER_MACRO model_Burn_Alpha[] = { {"TRANSPARENCYZTEST"}, {"USE_PAPERBURN"}, {NULL, NULL} };
		D3D_SHADER_MACRO model_Burn_Albedo[] = { {"USE_ALBEDO"}, {"USE_PAPERBURN"}, {NULL, NULL} };



		// PBR Shader ����
		// basic
		isAllShaderCompile &= CreatePixelShader("Shader/PBRShader/PBRModelPS.hlsl", "main", "ps_5_0", "PBRPS", nullptr);
		// albedo
		isAllShaderCompile &= CreatePixelShader("Shader/PBRShader/PBRModelPS.hlsl", "main", "ps_5_0", "PBRAlbedoPS", model_MacroPS_1);
		// albedo + normal
		isAllShaderCompile &= CreatePixelShader("Shader/PBRShader/PBRModelPS.hlsl", "main", "ps_5_0", "PBRNormalPS", model_MacroPS_2);
		// albedo + metallic + roughness
		isAllShaderCompile &= CreatePixelShader("Shader/PBRShader/PBRModelPS.hlsl", "main", "ps_5_0", "PBRMetallicRoughnessPS", model_MacroPS_3);
		// albedo + normal + metallic + roughness
		isAllShaderCompile &= CreatePixelShader("Shader/PBRShader/PBRModelPS.hlsl", "main", "ps_5_0", "PBRNormalMetallicRoughnessPS", model_MacroPS_4);
		// albedo + normal + ao
		isAllShaderCompile &= CreatePixelShader("Shader/PBRShader/PBRModelPS.hlsl", "main", "ps_5_0", "PBRNormalAOPS", model_MacroPS_5);
		// albedo + normal + metallic + roughness + ao
		isAllShaderCompile &= CreatePixelShader("Shader/PBRShader/PBRModelPS.hlsl", "main", "ps_5_0", "PBRNormalMRAOPS", model_MacroPS_6);
		// albedo + normal + metallic + roughness + ao + emissive
		isAllShaderCompile &= CreatePixelShader("Shader/PBRShader/PBRModelPS.hlsl", "main", "ps_5_0", "PBRFullPS", model_MacroPS_7);
		// no TexFile only clip
		isAllShaderCompile &= CreatePixelShader("Shader/PBRShader/PBRModelPS.hlsl", "main", "ps_5_0", "PBRAlphaPS", model_MacroPS_8);
		// silouette
		isAllShaderCompile &= CreatePixelShader("Shader/PBRShader/PBRModelPS.hlsl", "main", "ps_5_0", "PBRSilhouettePS", model_MacroPS_9);

		// model Skin
		isAllShaderCompile &= CreatePixelShader("Shader/PBRShader/PBRModelPS.hlsl", "main", "ps_5_0", "PBRSkinPS", modelSkin_MacroPS_0);
		// albedo
		isAllShaderCompile &= CreatePixelShader("Shader/PBRShader/PBRModelPS.hlsl", "main", "ps_5_0", "PBRSkinAlbedoPS", modelSkin_MacroPS_1);
		// albedo + normal
		isAllShaderCompile &= CreatePixelShader("Shader/PBRShader/PBRModelPS.hlsl", "main", "ps_5_0", "PBRSkinNormalPS", modelSkin_MacroPS_2);
		// albedo + metallic + roughness
		isAllShaderCompile &= CreatePixelShader("Shader/PBRShader/PBRModelPS.hlsl", "main", "ps_5_0", "PBRSkinMetallicRoughnessPS", modelSkin_MacroPS_3);
		// albedo + normal + metallic + roughness
		isAllShaderCompile &= CreatePixelShader("Shader/PBRShader/PBRModelPS.hlsl", "main", "ps_5_0", "PBRSkinNormalMetallicRoughnessPS", modelSkin_MacroPS_4);
		// albedo + normal + ao
		isAllShaderCompile &= CreatePixelShader("Shader/PBRShader/PBRModelPS.hlsl", "main", "ps_5_0", "PBRSkinNormalAOPS", modelSkin_MacroPS_5);
		// albedo + normal + metallic + roughness + ao
		isAllShaderCompile &= CreatePixelShader("Shader/PBRShader/PBRModelPS.hlsl", "main", "ps_5_0", "PBRSkinNormalMRAOPS", modelSkin_MacroPS_6);
		// albedo + normal + metallic + roughness + ao + emissive
		isAllShaderCompile &= CreatePixelShader("Shader/PBRShader/PBRModelPS.hlsl", "main", "ps_5_0", "PBRSkinFullPS", modelSkin_MacroPS_7);

		// PaperBurn
		isAllShaderCompile &= CreatePixelShader("Shader/PBRShader/PBRModelPS.hlsl", "main", "ps_5_0", "PaperBurnPS", model_Burn);
		isAllShaderCompile &= CreatePixelShader("Shader/PBRShader/PBRModelPS.hlsl", "main", "ps_5_0", "SkinPaperBurnPS", modelSkin_Burn);
		isAllShaderCompile &= CreatePixelShader("Shader/PBRShader/PBRModelPS.hlsl", "main", "ps_5_0", "PaperBurnAlbedoPS", model_Burn_Albedo);
		isAllShaderCompile &= CreatePixelShader("Shader/PBRShader/PBRModelPS.hlsl", "main", "ps_5_0", "SkinPaperBurnAlbedoPS", modelSkin_Burn_Albedo);
		isAllShaderCompile &= CreatePixelShader("Shader/PBRShader/PBRModelPS.hlsl", "main", "ps_5_0", "PaperBurnAlphaPS", model_Burn_Alpha);

		D3D_SHADER_MACRO light_MacroPS_1[] = { {"IsShadow"}, {NULL, NULL} };
		D3D_SHADER_MACRO light_MacroPS_2[] = { { "IsPointShadow" }, { NULL, NULL } };
		D3D_SHADER_MACRO light_MacroPS_3[] = { { "IsSpotShadow" }, { NULL, NULL } };
		D3D_SHADER_MACRO light_MacroPS_4[] = { {"IsPointShadow"}, {"IsSpotShadow"}, {NULL, NULL} };
		D3D_SHADER_MACRO light_MacroPS_5[] = { {"IsShadow"}, {"IsPointShadow"}, {"IsSpotShadow"}, {NULL, NULL} };
		D3D_SHADER_MACRO light_MacroPS_6[] = { {"IsShadow"}, {"IsPointShadow"}, {"IsSpotShadow"}, {"IsIndoor"}, {NULL, NULL} };

		// PBR Light
		isAllShaderCompile &= CreatePixelShader("Shader/PBRShader/PBRLightPS.hlsl", "main", "ps_5_0", "PBRLightPS", nullptr);
		// �׸��� ���̴��� ����� ���� �ʹ� ���� �ɷ��� �ϴ��� ����
		//CreatePixelShader("Shader/PBRShader/PBRLightPS.hlsl", "main", "ps_5_0", "PBRLightShadowPS", light_MacroPS_1);
		//CreatePixelShader("Shader/PBRShader/PBRLightPS.hlsl", "main", "ps_5_0", "PBRLightPointShadowPS", light_MacroPS_2);
		//CreatePixelShader("Shader/PBRShader/PBRLightPS.hlsl", "main", "ps_5_0", "PBRLightSpotShadowPS", light_MacroPS_3);
		//isAllShaderCompile &= CreatePixelShader("Shader/PBRShader/PBRLightPS.hlsl", "main", "ps_5_0", "PBRLightPointSpotShadowPS", light_MacroPS_4);
		isAllShaderCompile &= CreatePixelShader("Shader/PBRShader/PBRLightPS.hlsl", "main", "ps_5_0", "PBRLightFullShadowPS", light_MacroPS_5);
		isAllShaderCompile &= CreatePixelShader("Shader/PBRShader/PBRLightPS.hlsl", "main", "ps_5_0", "PBRLightFullShadowIndoorPS", light_MacroPS_6);

		// Transparency Obj
			// ���� ���̰� ��� �ϴ� ���..
		// �⺻������ ��� �׸��� ����
		// Texture���� ��� �׸��� ����
		/*
		D3D_SHADER_MACRO transparency_Macro_All[] = { {"IsPointShadow"}, {"IsSpotShadow"}, {NULL, NULL} };
		CreatePixelShader("Shader/PBRShader/PBRTransparencyPS.hlsl", "main", "ps_5_0", "PBRAlphaPS", transparency_Macro_All);
		// �ʹ� �������� �ϴ��� shdaow ����
		// ��� �ؽ��� ��
		D3D_SHADER_MACRO transparency_Macro_All_TextureFull[] = { {"USE_ALBEDO"}, {"USE_NORMAL"}, {"USE_METALLIC"}, {"USE_ROUGHNESS"}, {"USE_AO"}, {"USE_EMISSIVE"}, {NULL, NULL} };
		D3D_SHADER_MACRO transparency_Macro_All_TextureAlbedo[] = { {"USE_ALBEDO"}, {NULL, NULL} };
		// A - albedo / N - normal / M - Metallic / R - roughness / AO
		D3D_SHADER_MACRO transparency_Macro_All_TextureAN[] = { {"USE_ALBEDO"}, {"USE_NORMAL"}, {NULL, NULL} };
		D3D_SHADER_MACRO transparency_Macro_All_TextureAMR[] = { {"USE_ALBEDO"}, {"USE_METALLIC"}, {"USE_ROUGHNESS"}, {NULL, NULL} };
		D3D_SHADER_MACRO transparency_Macro_All_TextureANMR[] = { {"USE_ALBEDO"}, {"USE_NORMAL"}, {"USE_METALLIC"}, {"USE_ROUGHNESS"}, {NULL, NULL} };
		D3D_SHADER_MACRO transparency_Macro_All_TextureANAO[] = { {"USE_ALBEDO"}, {"USE_NORMAL"}, {"USE_AO"}, {NULL, NULL} };
		D3D_SHADER_MACRO transparency_Macro_All_TextureANMRAO[] = { {"USE_ALBEDO"}, {"USE_NORMAL"}, {"USE_METALLIC"}, {"USE_ROUGHNESS"}, {"USE_AO"}, {NULL, NULL} };

		CreatePixelShader("Shader/PBRShader/PBRTransparencyPS.hlsl", "main", "ps_5_0", "PBRAlphaAlbedoPS", transparency_Macro_All_TextureAlbedo);
		CreatePixelShader("Shader/PBRShader/PBRTransparencyPS.hlsl", "main", "ps_5_0", "PBRAlphaNormalPS", transparency_Macro_All_TextureAN);
		CreatePixelShader("Shader/PBRShader/PBRTransparencyPS.hlsl", "main", "ps_5_0", "PBRAlphaMetallicRoughnessPS", transparency_Macro_All_TextureAMR);
		CreatePixelShader("Shader/PBRShader/PBRTransparencyPS.hlsl", "main", "ps_5_0", "PBRAlphaNormalMetallicRoughnessPS", transparency_Macro_All_TextureANMR);
		CreatePixelShader("Shader/PBRShader/PBRTransparencyPS.hlsl", "main", "ps_5_0", "PBRAlphaNormalAOPS", transparency_Macro_All_TextureANAO);
		CreatePixelShader("Shader/PBRShader/PBRTransparencyPS.hlsl", "main", "ps_5_0", "PBRAlphaNormalMRAOPS", transparency_Macro_All_TextureANMRAO);
		CreatePixelShader("Shader/PBRShader/PBRTransparencyPS.hlsl", "main", "ps_5_0", "PBRAlphaFullPS", transparency_Macro_All_TextureFull);*/


		// PostPorcessing
		D3D_SHADER_MACRO post_Macro_ToneMap[] = { {"UseToneMapping"}, {NULL, NULL} };
		D3D_SHADER_MACRO post_Macro_All[] = { {"UseToneMapping"}, {"UseVignette"}, {NULL, NULL} };
		isAllShaderCompile &= CreatePixelShader("Shader/Deferred/PostProcessPS.hlsl", "main", "ps_5_0", "PostToneMappingPS", post_Macro_ToneMap);
		isAllShaderCompile &= CreatePixelShader("Shader/Deferred/PostProcessPS.hlsl", "main", "ps_5_0", "PostProcessAllPS", post_Macro_All);
		//CreatePixelShader("Shader/Deferred/PostProcessPS.hlsl", "blurMain", "ps_5_0", "BlurPS", nullptr);
		isAllShaderCompile &= CreatePixelShader("Shader/Deferred/PostProcessPS.hlsl", "bloomMain", "ps_5_0", "DownPS", nullptr);
		isAllShaderCompile &= CreatePixelShader("Shader/Deferred/PostProcessPS.hlsl", "UnityBlurH", "ps_5_0", "BlurHPS", nullptr);
		isAllShaderCompile &= CreatePixelShader("Shader/Deferred/PostProcessPS.hlsl", "UnityBlurV", "ps_5_0", "BlurVPS", nullptr);
		isAllShaderCompile &= CreatePixelShader("Shader/Deferred/PostProcessPS.hlsl", "UpSampleMain", "ps_5_0", "UpSamplePS", nullptr);

		// SkyBox
		isAllShaderCompile &= CreatePixelShader("Shader/SkyBoxShader/SkyBoxPS.hlsl", "main", "ps_5_0", "SkyBoxPS", nullptr);

		// Particle
			// ��ƼŬ�� ���õ� ��� ���̴�(VS, GS, CS, PS)�� �� �Ʒ����� �����ϴ�.
		// particle CS
		isAllShaderCompile &= CreateComputeShader("Shader/Particle/BasicParticleCS.hlsl", "CS_Main", "cs_5_0", "ParticleCS", nullptr);
		// �� �� �κ��� ���� ������� �� �𸣰ڽ��ϴ�..
		isAllShaderCompile &= CreateComputeShader("Shader/Particle/BasicParticleCS.hlsl", "CS_ResetParticleBuffer", "cs_5_0", "ResetParticleBufferCS", nullptr);
		isAllShaderCompile &= CreateComputeShader("Shader/Particle/BasicParticleCS.hlsl", "CS_ClearCounter", "cs_5_0", "ClearCounterBufferCS", nullptr);

		// particle VS
		// ��ǻ� ID�� �������ݴϴ� => ��� ����� ���� ComputeShader���� ������, ���� ������ ���� ���ۿ� ��� �ֽ��ϴ�.
			// �ش� ���ۿ� ������ �� �ִ� �ε��� ���۸��� �����ϴ�.
		isAllShaderCompile &= CreateVertexShader("Shader/Particle/BasicParticleVS.hlsl", "ComputeShaderDrawVS", "vs_5_0", "ParticleVS", InputLayoutDesc::ParticleID, 1, nullptr);

		// particle GS
		// ��ƼŬ�� Ʈ���Ͽ� �ش��ϴ� ������Ʈ�� ���̴��� ������ ���ݴϴ�.
		isAllShaderCompile &= CreateGeometryShader("Shader/Particle/BasicParticleGS.hlsl", "DrawGS", "gs_5_0", "ParticleGS", nullptr);
		isAllShaderCompile &= CreateGeometryShader("Shader/Particle/BasicParticleGS.hlsl", "DrawTrailGS", "gs_5_0", "TrailGS", nullptr);

		// particle PS
		// OIT�� ����ϴ� ��ƼŬ PS�� ������ ���ݴϴ�.
			// �Ʒ��� ���̴��� ���� �� �ȼ��� ������ ������ ���ۿ� �����صΰ�
		isAllShaderCompile &= CreatePixelShader("Shader/Particle/BasicParticlePS.hlsl", "OIT_Particle_PS", "ps_5_0", "ParticlePS", nullptr);

		// ���� OIT Deferred Shader�� ���ؼ� ȭ�鿡 �׷��� �����Դϴ�.
		isAllShaderCompile &= CreatePixelShader("Shader/Particle/OITBlendPS.hlsl", "OITMain", "ps_5_0", "OITBlendPS", nullptr);
		/*
		 */

		return isAllShaderCompile;
	}

	bool ShaderManager::ReadAllShader()
	{
		bool isAllShaderRead = true;

		/// VSRead
		// Simple VS
		isAllShaderRead &= ReadVertexShader("LineVS", InputLayoutDesc::Pos, 1);
		isAllShaderRead &= ReadVertexShader("BasicColorVS", InputLayoutDesc::PosColor, 2);
		isAllShaderRead &= ReadVertexShader("SkyBoxVS", InputLayoutDesc::Pos, 1);
		isAllShaderRead &= ReadVertexShader("SpriteEffectVS", InputLayoutDesc::PosTex, 2);
		isAllShaderRead &= ReadVertexShader("QuadVS", InputLayoutDesc::PosTex, 2);

		// PBR VertexShader
		isAllShaderRead &= ReadVertexShader("PBRStaticVS", InputLayoutDesc::PBRStatic, 5);
		isAllShaderRead &= ReadVertexShader("PBRSkinnedVS", InputLayoutDesc::PBRSkinned, 9);
		isAllShaderRead &= ReadVertexShader("PBRAlphaVS", InputLayoutDesc::PBRSkinned, 5);
		isAllShaderRead &= ReadVertexShader("PBRAlphaSkinnedVS", InputLayoutDesc::PBRSkinned, 9);
		isAllShaderRead &= ReadVertexShader("PBRMonsterVS", InputLayoutDesc::PBRSkinned, 9);

		// Shadow
		isAllShaderRead &= ReadVertexShader("ShadowStaticVS", InputLayoutDesc::PBRStatic, 5);
		isAllShaderRead &= ReadVertexShader("ShadowSkinnedVS", InputLayoutDesc::PBRSkinned, 9);
		isAllShaderRead &= ReadVertexShader("GeoShadowStaticVS", InputLayoutDesc::PBRStatic, 5);
		isAllShaderRead &= ReadVertexShader("GeoShadowSkinnedVS", InputLayoutDesc::PBRSkinned, 9);

		// Particle
		isAllShaderRead &= ReadVertexShader("ParticleVS", InputLayoutDesc::ParticleID, 1);

		/// GS Read
		// Shadow
		isAllShaderRead &= ReadShader("ShadowGS", ShaderType::GEOMETRY);
		// Particle
		isAllShaderRead &= ReadShader("ParticleGS", ShaderType::GEOMETRY);
		isAllShaderRead &= ReadShader("TrailGS", ShaderType::GEOMETRY);

		/// PS Read
		// Simple
		isAllShaderRead &= ReadShader("LinePS", ShaderType::PIXEL);
		isAllShaderRead &= ReadShader("BasicColorPS", ShaderType::PIXEL);
		isAllShaderRead &= ReadShader("SpriteEffectPS", ShaderType::PIXEL);
		isAllShaderRead &= ReadShader("SpriteEffectSRCPS", ShaderType::PIXEL);
		isAllShaderRead &= ReadShader("SkyBoxPS", ShaderType::PIXEL);

		// PBR Model
		// basic
		isAllShaderRead &= ReadShader("PBRPS", ShaderType::PIXEL);
		// albedo
		isAllShaderRead &= ReadShader("PBRAlbedoPS", ShaderType::PIXEL);
		// albedo + normal
		isAllShaderRead &= ReadShader("PBRNormalPS", ShaderType::PIXEL);
		// albedo + metallic + roughness
		isAllShaderRead &= ReadShader("PBRMetallicRoughnessPS", ShaderType::PIXEL);
		// albedo + normal + metallic + roughness
		isAllShaderRead &= ReadShader("PBRNormalMetallicRoughnessPS", ShaderType::PIXEL);
		// albedo + normal + ao
		isAllShaderRead &= ReadShader("PBRNormalAOPS", ShaderType::PIXEL);
		// albedo + normal + metallic + roughness + ao
		isAllShaderRead &= ReadShader("PBRNormalMRAOPS", ShaderType::PIXEL);
		// albedo + normal + metallic + roughness + ao + emissive
		isAllShaderRead &= ReadShader("PBRFullPS", ShaderType::PIXEL);
		// no TexFile only clip
		isAllShaderRead &= ReadShader("PBRAlphaPS", ShaderType::PIXEL);
		// silouette
		isAllShaderRead &= ReadShader("PBRSilhouettePS", ShaderType::PIXEL);
		isAllShaderRead &= ReadShader("PBRMonsterSilhouettePS", ShaderType::PIXEL);
		isAllShaderRead &= ReadShader("PBREdgePS", ShaderType::PIXEL);

		// Skin PBR Model
		isAllShaderRead &= ReadShader("PBRSkinPS", ShaderType::PIXEL);
		// albedo
		isAllShaderRead &= ReadShader("PBRSkinAlbedoPS", ShaderType::PIXEL);
		// albedo + normal
		isAllShaderRead &= ReadShader("PBRSkinNormalPS", ShaderType::PIXEL);
		// albedo + metallic + roughness
		isAllShaderRead &= ReadShader("PBRSkinMetallicRoughnessPS", ShaderType::PIXEL);
		// albedo + normal + metallic + roughness
		isAllShaderRead &= ReadShader("PBRSkinNormalMetallicRoughnessPS", ShaderType::PIXEL);
		// albedo + normal + ao
		isAllShaderRead &= ReadShader("PBRSkinNormalAOPS", ShaderType::PIXEL);
		// albedo + normal + metallic + roughness + ao
		isAllShaderRead &= ReadShader("PBRSkinNormalMRAOPS", ShaderType::PIXEL);
		// albedo + normal + metallic + roughness + ao + emissive
		isAllShaderRead &= ReadShader("PBRSkinFullPS", ShaderType::PIXEL);

		// PaperBurn
		isAllShaderRead &= ReadShader("PaperBurnPS", ShaderType::PIXEL);
		isAllShaderRead &= ReadShader("SkinPaperBurnPS", ShaderType::PIXEL);
		isAllShaderRead &= ReadShader("PaperBurnAlbedoPS", ShaderType::PIXEL);
		isAllShaderRead &= ReadShader("SkinPaperBurnAlbedoPS", ShaderType::PIXEL);
		isAllShaderRead &= ReadShader("PaperBurnAlphaPS", ShaderType::PIXEL);

		// PBR Light
		isAllShaderRead &= ReadShader("PBRLightPS", ShaderType::PIXEL);
		isAllShaderRead &= ReadShader("PBRLightFullShadowPS", ShaderType::PIXEL);
		isAllShaderRead &= ReadShader("PBRLightFullShadowIndoorPS", ShaderType::PIXEL);

		// PostProcessing
		isAllShaderRead &= ReadShader("PostProcessAllPS", ShaderType::PIXEL);
		isAllShaderRead &= ReadShader("DownPS", ShaderType::PIXEL);
		isAllShaderRead &= ReadShader("ExceptDownPS", ShaderType::PIXEL);
		isAllShaderRead &= ReadShader("BlurHPS", ShaderType::PIXEL);
		isAllShaderRead &= ReadShader("BlurVPS", ShaderType::PIXEL);
		isAllShaderRead &= ReadShader("UpSamplePS", ShaderType::PIXEL);
		isAllShaderRead &= ReadShader("PostProcessNotBloomPS", ShaderType::PIXEL);
		isAllShaderRead &= ReadShader("NotPostProcessPS", ShaderType::PIXEL);
		
		// Particle
		isAllShaderRead &= ReadShader("ParticlePS", ShaderType::PIXEL);
		isAllShaderRead &= ReadShader("OITBlendPS", ShaderType::PIXEL);

		// CS
		// Particle
		isAllShaderRead &= ReadShader("ParticleCS", ShaderType::COMPUTE);
		isAllShaderRead &= ReadShader("ResetParticleBufferCS", ShaderType::COMPUTE);
		isAllShaderRead &= ReadShader("ClearCounterBufferCS", ShaderType::COMPUTE);
		
		return isAllShaderRead;
	}

	std::shared_ptr<ShaderBase> ShaderManager::GetShader(const std::string& shaderName)
	{
		if (shaderMap.find(shaderName) == shaderMap.end())
			return nullptr;

		return shaderMap.at(shaderName);
	}

	void ShaderManager::Finalize()
	{
		// ���� ��ȸ �ϸ鼭 ��� �ڵ� ����
		for (auto shader : shaderMap)
		{
			shader.second.reset();
		}

		shaderMap.clear();

		SafeReset(instance)
	}

	/**
	 * \brief hlsl�ϳ��� �������ϴ� �Լ�
	 * \param path ���̴� ������ path
	 * \param entryName shader �ڵ� ������ entryPointer name
	 * \param shaderModel ����� ���̴� ���α׷��� ������ ��� ����.
	 * \param blobOut ���̴��� �������ؼ� ������ blob�� ������
	 * \return ���� ����
	 */
	bool ShaderManager::CompileShaderFromFile(const TCHAR* path, const char* entryName, const char* shaderModel,
		ID3DBlob** blobOut, const D3D_SHADER_MACRO* macro)
	{
		if (path == nullptr || entryName == nullptr || shaderModel == nullptr || blobOut == nullptr)
			return false;

		*blobOut = nullptr;

		UINT flags = D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;
#if defined(DEBUG) || defined(_DEBUG)
		flags |= D3DCOMPILE_DEBUG;
		flags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

		ID3DBlob* errorBlob = nullptr;

		HRESULT hr = D3DCompileFromFile(
			path,
			macro,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			entryName,
			shaderModel,
			flags,
			0,
			blobOut,
			&errorBlob);

		if (FAILED(hr))
		{
			if (errorBlob != nullptr)
			{
				OutputDebugStringA(static_cast<char*>(errorBlob->GetBufferPointer()));
				printf(static_cast<char*>(errorBlob->GetBufferPointer()));
				errorBlob->Release();
			}

			return false;
		}

		if (errorBlob != nullptr)
			errorBlob->Release();

		return true;
	}

	bool ShaderManager::CreateVertexShader(const std::string& path, const std::string& entryName,
		const std::string& shaderModel, const std::string& shaderName, D3D11_INPUT_ELEMENT_DESC* desc,
		UINT elementsSize, const D3D_SHADER_MACRO* macro)
	{
		// �ʿ� �ش� �̸��� ���̴��� ������ �������ݴϴ�
			// �ٽ� ���� �ʿ� ���� �����Դϴ�.
		if (shaderMap.find(shaderName) != shaderMap.end())
			return false;

		std::shared_ptr<ShaderBase> tmpShader;

		// Ȥ�� �� ��� ����
		if (shaderBlob != nullptr)
		{
			//shaderBlob->Release();
			shaderBlob = nullptr;
		}

		// ���� ���ڸ� ����մϴ�.
		std::wstring wpath;

		wpath.assign(path.begin(), path.end());

		if (CompileShaderFromFile(wpath.c_str(), entryName.c_str(), shaderModel.c_str(), &shaderBlob, macro) != true)
			return false;

		// �߰��� �ش� ���ؽ� ���̴��� ����ϴ� ��ǲ ���̾ƿ��� ������ش�.
		tmpShader.reset(new VertexShader(device, deviceContext, shaderBlob, desc, elementsSize));

		if (tmpShader == nullptr)
		{
			assert(0);		// ���� ó��
		}

		// ������� ���ؽ� ���̴��� �ʿ� �־��ش�.
		shaderMap.insert(std::pair{ shaderName, tmpShader });

		printf("%s Compile Complete! \n", shaderName.c_str());

		// ������� ���� ����
		return true;
	}

	bool ShaderManager::CreatePixelShader(const std::string& path, const std::string& entryName,
		const std::string& shaderModel, const std::string& shaderName, const D3D_SHADER_MACRO* macro)
	{
		// �ʿ� �ش� �̸��� ���̴��� ������ �������ݴϴ�
			// �ٽ� ���� �ʿ� ���� �����Դϴ�.
		if (shaderMap.find(shaderName) != shaderMap.end())
			return false;

		// �⺻������ ���� �Ͱ� ����ϳ� pixelshader�� �����մϴ�.
		std::shared_ptr<ShaderBase> tmpShader;

		// Ȥ�� �� ��� ����
		if (shaderBlob != nullptr)
		{
			//shaderBlob->Release();
			shaderBlob = nullptr;
		}

		// ���� ���ڸ� ����մϴ�.
		std::wstring wpath = std::wstring(path.begin(), path.end());

		if (CompileShaderFromFile(wpath.c_str(), entryName.c_str(), shaderModel.c_str(), &shaderBlob, macro) != true)
			return false;

		tmpShader.reset(new PixelShader(device, deviceContext, shaderBlob));

		if (tmpShader == nullptr)
		{
			assert(0);		// ���� ó��
		}

		// ������� �ȼ� ���̴��� �ʿ� �־��ش�.
		shaderMap.insert(std::pair{ shaderName, tmpShader });

		printf("%s Compile Complete! \n", shaderName.c_str());


		// ������� ���� ����
		return true;
	}

	bool ShaderManager::CreateGeometryShader(const std::string& path, const std::string& entryName,
		const std::string& shaderModel, const std::string& shaderName, const D3D_SHADER_MACRO* macro)
	{
		// �ʿ� �ش� �̸��� ���̴��� ������ �������ݴϴ�
			// �ٽ� ���� �ʿ� ���� �����Դϴ�.
		if (shaderMap.find(shaderName) != shaderMap.end())
			return false;

		std::shared_ptr<ShaderBase> tmpShader;

		// Ȥ�� �� ��� ����
		if (shaderBlob != nullptr)
		{
			shaderBlob = nullptr;
		}

		// ���� ���ڸ� ����մϴ�.
		std::wstring wpath = std::wstring(path.begin(), path.end());

		if (CompileShaderFromFile(wpath.c_str(), entryName.c_str(), shaderModel.c_str(), &shaderBlob, macro) != true)
			return false;

		tmpShader.reset(new GeometryShader(device, deviceContext, shaderBlob));

		if (tmpShader == nullptr)
		{
			assert(0);		// ���� ó��
		}

		// ������� ������Ʈ�� ���̴��� �ʿ� �־��ش�.
		shaderMap.insert(std::pair{ shaderName, tmpShader });

		printf("%s Compile Complete! \n", shaderName.c_str());

		// ������� ���� ����
		return true;
	}

	bool ShaderManager::CreateComputeShader(const std::string& path, const std::string& entryName,
		const std::string& shaderModel, const std::string& shaderName, const D3D_SHADER_MACRO* macro)
	{
		// �ʿ� �ش� �̸��� ���̴��� ������ �������ݴϴ�
				// �ٽ� ���� �ʿ� ���� �����Դϴ�.
		if (shaderMap.find(shaderName) != shaderMap.end())
			return false;

		std::shared_ptr<ShaderBase> tmpShader;

		// Ȥ�� �� ��� ����
		if (shaderBlob != nullptr)
		{
			shaderBlob = nullptr;
		}

		// ���� ���ڸ� ����մϴ�.
		std::wstring wpath = std::wstring(path.begin(), path.end());

		if (CompileShaderFromFile(wpath.c_str(), entryName.c_str(), shaderModel.c_str(), &shaderBlob, macro) != true)
			return false;

		// �� �κп��� ���̴��� ������ݴϴ�.
			// ��Ȯ���� ������ �������� ���̴��� Ŭ������ ����ݴϴ�.
		tmpShader.reset(new ComputeShader(device, deviceContext, shaderBlob));

		if (tmpShader == nullptr)
		{
			assert(0);		// ���� ó��
		}

		// ������� ��ǻƮ ���̴��� �ʿ� �־��ش�.
		shaderMap.insert(std::pair{ shaderName, tmpShader });

		printf("%s Compile Complete! \n", shaderName.c_str());

		// ������� ���� ����
		return true;
	}

	bool ShaderManager::ReadVertexShader(const std::string& shaderName, D3D11_INPUT_ELEMENT_DESC* desc,
		UINT elementSize)
	{
		// ���ؽ� ���̴��� �о�´�..
		// �ʿ� �ش� �̸��� ���̴��� ������ �������ݴϴ�
			// �ٽ� ���� �ʿ� ���� �����Դϴ�.
		if (shaderMap.find(shaderName) != shaderMap.end())
			return false;

		std::shared_ptr<ShaderBase> tmpShader;

		// ���� ���ڸ� ����մϴ�.
		std::string path = shaderPath;
		path += shaderName;
		path += ".cso";

		std::wstring wpath;

		wpath.assign(path.begin(), path.end());

		// �о�ɴϴ�.
		auto shaderByteCode = DX::ReadData(wpath.c_str());

		// �߰��� �ش� ���ؽ� ���̴��� ����ϴ� ��ǲ ���̾ƿ��� ������ش�.
		tmpShader.reset(new VertexShader(device, deviceContext, shaderByteCode, desc, elementSize));

		if (tmpShader == nullptr)
		{
			assert(0);		// ���� ó��
		}

		// ������� ���ؽ� ���̴��� �ʿ� �־��ش�.
		shaderMap.insert(std::pair{ shaderName, tmpShader });

		printf("%s Read Complete! \n", shaderName.c_str());

		// ������� ���� ����
		return true;
	}

	bool ShaderManager::ReadShader(const std::string& shaderName, ShaderType type)
	{
		// �ʿ� �ش� �̸��� ���̴��� ������ �������ݴϴ�
				// �ٽ� ���� �ʿ� ���� �����Դϴ�.
		if (shaderMap.find(shaderName) != shaderMap.end())
			return false;

		std::shared_ptr<ShaderBase> tmpShader;

		// ���� ���ڸ� ����մϴ�.
		std::string path = shaderPath;
		path += shaderName;
		path += ".cso";

		std::wstring wpath;

		wpath.assign(path.begin(), path.end());

		// �о�ɴϴ�.
		auto shaderByteCode = DX::ReadData(wpath.c_str());

		// ���������� �з��ؼ� ������ݴϴ�.
		switch (type)
		{
		case ShaderType::PIXEL:
			tmpShader.reset(new PixelShader(device, deviceContext, shaderByteCode));
			break;

		case ShaderType::COMPUTE:
			tmpShader.reset(new ComputeShader(device, deviceContext, shaderByteCode));
			break;

		case ShaderType::GEOMETRY:
			tmpShader.reset(new GeometryShader(device, deviceContext, shaderByteCode));
			break;

		case ShaderType::VERTEX:
		case ShaderType::NONE:
		default:
			break;
		}

		if (tmpShader == nullptr)
		{
			assert(0);		// ���� ó��
		}

		// ������� ���ؽ� ���̴��� �ʿ� �־��ش�.
		shaderMap.insert(std::pair{ shaderName, tmpShader });

		printf("%s Read Complete! \n", shaderName.c_str());

		// ������� ���� ����
		return true;
	}
}
