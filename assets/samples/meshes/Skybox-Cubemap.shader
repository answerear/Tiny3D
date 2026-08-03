Shader "Tiny3DBuiltin/Skybox-Cubemap"
{
	// 常量名不能以下划线开头：HLSL → SPIR-V → 各目标语言的往返里，SPIRV-Cross 会
	// 把 cbuffer 成员的前导下划线当保留标识符去掉，导致 D3D11 反射出来的名字和
	// Vulkan 反射出来的不一致，材质就只能在其中一个后端上绑对。纹理不受影响。
	Properties
	{
		Tint("Tint Color", Color) = (0.5,0.5,0.5,1)
		Exposure("Exposure", Range(0,8)) = 1.0
		Rotation("Rotation", Range(0,360)) = 0
		_Tex("Cubemap", Cube) = "skybox_default.ttex" {}
	}

	SubShader
	{
		Tags { "Queue"="Background" "RenderType"="Background" "PreviewType"="Skybox" }

		Pass
		{
			Name "SKYBOX"
			Tags { "LightMode" = "ForwardBase" }

			Cull Off
			ZWrite Off
			ZTest LEqual

			CGPROGRAM

			#pragma vertex vert
			#pragma fragment frag
			#pragma target 4.0

			// 天空盒不走 per-draw 的世界变换，只需要 VP 的逆矩阵把裁剪空间角点
			// 反投影回世界空间。单独一个 cbuffer，避免动到 Tiny3DPerFrame 的布局
			cbuffer Tiny3DSkybox
			{
				row_major float4x4 tiny3d_MatrixInvVP;
			}

			cbuffer tiny3d_CameraParams
			{
				float4 tiny3d_CameraWorldPos;
			}

			cbuffer tiny3d_ProjectionParamsCB
			{
				float4 tiny3d_ProjectionParams;
			}

			cbuffer Tiny3DSkyboxParams
			{
				float4 Tint;
				float Exposure;
				float Rotation;
			}

			TextureCube _Tex;
			SamplerState sampler_Tex;

			struct VertexInput
			{
				float3 position : POSITION;
			};

			struct VertexOutput
			{
				float4 position : SV_POSITION;
				float3 direction : TEXCOORD0;
			};

			VertexOutput vert(VertexInput input)
			{
				VertexOutput output;

				// 输入是覆盖整个 NDC 的大三角形，z = w 让它正好落在远平面上，
				// 配合 ZWrite Off / ZTest LEqual 被前面的不透明物体挡住
				float4 clip = float4(input.position.xy, 1.0f, 1.0f);

				float4 world = mul(tiny3d_MatrixInvVP, clip);
				output.direction = world.xyz / world.w - tiny3d_CameraWorldPos.xyz;

				output.position = clip;
				output.position.y *= tiny3d_ProjectionParams.x;

				return output;
			}

			struct PS_INPUT
			{
				float4 position : SV_POSITION;
				float3 direction : TEXCOORD0;
			};

			float4 frag(PS_INPUT input) : SV_Target
			{
				float3 dir = normalize(input.direction);

				// 绕世界 Y 轴旋转天空盒
				float rad = Rotation * 0.01745329252f;
				float sinR = sin(rad);
				float cosR = cos(rad);
				float3 rotated = float3(
					cosR * dir.x - sinR * dir.z,
					dir.y,
					sinR * dir.x + cosR * dir.z);

				float4 texColor = _Tex.Sample(sampler_Tex, rotated);

				// 与 Unity 内置天空盒一致，Tint 以 0.5 为中性灰，所以要乘 2
				float3 color = texColor.rgb * Tint.rgb * 2.0f * Exposure;

				return float4(color, 1.0f);
			}

			ENDCG
		}
	}
}
