Shader "Tiny3DBuiltin/Default-Material"
{
	Properties
	{
		_Color("Color", Color) = (1,1,1,1)
		_MainTex("Albedo", 2D) = "white" {}
	}
	
	Category 
	{

		Tags { "Queue"="Transparent" "IgnoreProjector"="True" "RenderType"="Transparent" }

		SubShader
		{
			Tags { "Queue"="Opaque" "PerformanceChecks"="False" }
			
			Pass
			{
				Name "FORWARD"
				Tags { "LightMode" = "ForwardBase" }
				
				CGPROGRAM
				
				#pragma vertex vert
				#pragma fragment frag
				#pragma target 4.0

				cbuffer Tiny3DPerDraw
				{
				   row_major float4x4 tiny3d_ObjectToWorld;
				   row_major float4x4 tiny3d_WorldToObject;
				}

				cbuffer Tiny3DPerFrame
				{
				   row_major float4x4 tiny3d_MatrixV;
				   row_major float4x4 tiny3d_MatrixP;
				   row_major float4x4 tiny3d_MatrixVP;
				}

				static float4x4 tiny3d_MatrixMVP = mul(tiny3d_MatrixVP, tiny3d_ObjectToWorld);
				static float4x4 tiny3d_MatrixMV = mul(tiny3d_MatrixV, tiny3d_ObjectToWorld);

				#define TINY3D_MATRIX_M    tiny3d_ObjectToWorld
				#define TINY3D_MATRIX_V    tiny3d_MatrixV
				#define TINY3D_MATRIX_P    tiny3d_MatrixP
				#define TINY3D_MATRIX_VP   tiny3d_MatrixVP
				#define TINY3D_MATRIX_MV   tiny3d_MatrixMV
				#define TINY3D_MATRIX_MVP  tiny3d_MatrixMVP

				struct VertexInput
				{
				   float3 position : POSITION;
				   float2 uv : TEXCOORD0;
				};
				struct VertexOutput
				{
				   float4 position : SV_POSITION;
				   float2 uv : TEXCOORD0;
				};
				
				VertexOutput vert(VertexInput input)
				{
				   VertexOutput output;
				   output.position = mul(TINY3D_MATRIX_MVP, float4(input.position, 1.0f));
				   output.uv = input.uv;
				   return output;
				}

				#define CONCATENATE(a, b) a##b
				#define TEX2D(name) Texture2D name; SamplerState CONCATENATE(sampler, name);
				#define TEX2D_R(name, r) Texture2D name : register(CONCATENATE(t, r)); SamplerState CONCATENATE(sampler, name) : register(CONCATENATE(s, r));
				#define SAMPLE(tex, uv) tex.Sample(CONCATENATE(sampler, tex), uv);
				
				Texture2D _MainTex : register(t0);
				SamplerState sampler__MainTex : register(s0);
				struct PS_INPUT
				{
					float4 position : SV_POSITION;
					float2 uv : TEXCOORD0;
				};
				float4 frag(PS_INPUT input) : SV_Target
				{
					float4 color = _MainTex.Sample(sampler__MainTex, input.uv);
					return color;
				}
				
				ENDCG
			}
		}
	}
}



			