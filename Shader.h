#ifndef SHADER_H
#define SHADER_H

#include "math/Vector.h"
#include "math/Matrix.h"
#include "PipelineData.h"
#include "FrameBufferAdapter.h"
#include "Texture.h"

struct SimpleShader
{
	// VS到FS之间传递的数据类型，目前只能做到手动给每一个数据插值
	struct VSToFS
	{
		VSToFS() {}
		VSToFS(VSToFS& from, VSToFS& to, float weight)
		{
			//裁剪阶段插值
			//通过构造函数生成插值后的实例
			pos = lerp(from.pos, to.pos, weight);
			texCoord = lerp(from.texCoord, to.texCoord, weight);
			norm = lerp(from.norm, to.norm, weight);
		}

		VSToFS(VSToFS& va, VSToFS& vb, VSToFS& vc, Vec3 weight)
		{
			//光栅化阶段的三角形重心插值
			pos = triLerp(va.pos, vb.pos, vc.pos, weight);
			texCoord = triLerp(va.texCoord, vb.texCoord, vc.texCoord, weight);
			norm = triLerp(va.norm, vb.norm, vc.norm, weight);
		}

		Vec3 pos;
		Vec2 texCoord;
		Vec3 norm;
	};

	// 输入VS的数据类型
	struct VSIn
	{
		Vec3 pos;
		Vec2 texCoord;
		Vec3 norm;
	};

	// Vertex Shader
	Pipeline::VSOut<VSToFS> processVertex(VSIn in)
	{
		Pipeline::VSOut<VSToFS> out;

		Vec4 inPos = { in.pos[0], in.pos[1], in.pos[2], 1.0f };
		out.sr_Position = proj * view * model * inPos;

		Vec4 outPos = model * inPos;

		Mat3 m(model);
		Mat3 modelInv = inverse(m).transpose();

		out.data.pos = { outPos[0], outPos[1], outPos[2] };
		out.data.texCoord = in.texCoord;
		out.data.norm = (modelInv * in.norm).normalized();
		return out;
	}

	// Fragment Shader
	void processFragment(FrameBufferAdapter& adapter, Pipeline::FSIn<VSToFS> in)
	{
		Vec3 result(0.0f);
		Vec3 lightPos = { 1.0f, -2.0f, 3.0f };
		Vec3 lightColor = { 1.0f, 1.0f, 1.0f };

		Vec3 L = (lightPos - in.data.pos).normalized();
		Vec3 N = in.data.norm;
		Vec3 V = (viewPos - in.data.pos).normalized();
		Vec3 H = (V + L).normalized();

		Vec3 F0 = lerp(Vec3(0.04f), albedo, metallic);

		Vec3 F = fresnelSchlick(std::max(dot(V, H), 0.0f), F0);
		float NDF = DistributionGGX(N, H, roughness);
		float G = GeometrySmith(N, V, L, roughness);

		Vec3 kS = F;
		Vec3 kD = Vec3(1.0f) - kS;
		kD *= 1.0f - metallic;

		Vec3 nominator = F * NDF * G;
		float denominator = 4.0f * std::max(dot(N, V), 0.0f) * std::max(dot(N, L), 0.0f) + 0.001f;
		Vec3 specular = nominator / denominator;

		float dist = (lightPos - in.data.pos).length();
		float attenuation = 1.0f / (dist * dist);
		Vec3 radiance = lightColor * lightStrength * attenuation;

		float NdotL = std::max(dot(N, L), 0.0f);
		Vec3 Lo = ((kD * albedo) / Pi + specular) * radiance * NdotL;
		Lo += Vec3(0.03f) * albedo * ao;

		Vec4 texColor = texture(tex, in.data.texCoord, NEAREST);
		Vec3 addition = { texColor[0], texColor[1], texColor[2] };

		float GAMMA = 2.2f;
		result = pow(Lo, 1.0f / GAMMA);

		result *= addition;

		adapter.writeColor(0, result);
		adapter.writeDepth(in.z);
	}

	Vec3 fresnelSchlick(float cosTheta, Vec3& F0)
	{
		return F0 + (Vec3(1.0f) - F0) * pow(1.0f - cosTheta, 5.0f);
	}

	float DistributionGGX(Vec3& N, Vec3& H, float roughness)
	{
		float a = roughness * roughness;
		float a2 = a * a;
		float NdotH = std::max(dot(N, H), 0.0f);
		float NdotH2 = NdotH * NdotH;

		float nom = a2;
		float denom = (NdotH2 * (a2 - 1.0f) + 1.0f);
		denom = Pi * denom * denom;

		return nom / denom;
	}

	float GeometrySchlickGGX(float NdotV, float roughness)
	{
		float r = (roughness + 1.0f);
		float k = (r * r) / 8.0f;

		float nom = NdotV;
		float denom = NdotV * (1.0f - k) + k;

		return nom / denom;
	}

	float GeometrySmith(Vec3& N, Vec3& V, Vec3& L, float roughness)
	{
		float NdotV = std::max(dot(N, V), 0.0f);
		float NdotL = std::max(dot(N, L), 0.0f);
		float ggx2 = GeometrySchlickGGX(NdotV, roughness);
		float ggx1 = GeometrySchlickGGX(NdotL, roughness);

		return ggx1 * ggx2;
	}

	//uniforms
	Mat4 model;
	Mat4 view;
	Mat4 proj;
	Vec3 viewPos;

	Vec3 albedo = Vec3(1.0f);
	float metallic = 1.0f;
	float roughness = 0.3f;
	float ao = 0.2f;

	float lightStrength;

	TextureRGB24 *tex = nullptr;
};

#endif
