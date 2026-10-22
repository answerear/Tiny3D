Shader "Tiny3DBuiltin/RmlUI-Texture"
{
	Properties
	{
		_MainTex("Texture", 2D) = "white" {}
	}

	SubShader
	{
		Pass
		{
			Name "RmlUI"
			Tags { "LightMode" = "ForwardBase" }

			Cull Off
			ZWrite Off
			ZTest Always

			CGPROGRAM

			#pragma vertex vert
			#pragma fragment frag
			#pragma target 4.0

			cbuffer Tiny3DPerFrame
			{
				row_major float4x4 tiny3d_MatrixVP;
			}

			cbuffer RmlUIPerDraw
			{
				row_major float4x4 Transform;
				float4 Translation;
			}

			struct VertexInput
			{
				float2 position : POSITION;
				float4 color : COLOR0;
				float2 uv : TEXCOORD0;
			};

			struct VertexOutput
			{
				float4 position : SV_POSITION;
				float4 color : COLOR0;
				float2 uv : TEXCOORD0;
			};

			VertexOutput vert(VertexInput input)
			{
				VertexOutput output;
				float4 pos = float4(input.position + Translation.xy, 0.0, 1.0);
				output.position = mul(tiny3d_MatrixVP, mul(Transform, pos));
				output.color = input.color;
				output.uv = input.uv;
				return output;
			}

			Texture2D _MainTex;
			SamplerState sampler_MainTex;

			float4 frag(VertexOutput input) : SV_TARGET
			{
				float4 texColor = _MainTex.Sample(sampler_MainTex, input.uv);
				if (Translation.w < 0.5)
				{
					texColor.rgb *= texColor.a;
				}
				return texColor * input.color;
			}

			ENDCG
		}
	}
}
