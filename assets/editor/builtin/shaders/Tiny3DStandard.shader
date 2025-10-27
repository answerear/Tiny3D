Shader "Tiny3DBuiltin/Standard"
{
	Properties
	{
		_Color("Color", Color) = (1,1,1,1)
		_MainTex("Albedo", 2D) = "white" {}
	}
	
	Category 
	{

		Tags { "Queue"="Transparent" "IgnoreProjector"="True" "RenderType"="Opaque" }

		SubShader
		{
			Tags { "Queue"="Opaque" "PerformanceChecks"="False" }
			
			Pass
			{
				Name "FORWARD"
				Tags { "LightMode" = "ForwardBase" }
				
				CGPROGRAM
				
				#pragma vertex vertForward
				#pragma fragment fragForward
				#pragma target 4.0

				#pragma multi_compile _ T3D_SKINNED_MESH 
				#pragma multi_compile _ T3D_GPU_SKINNING

				#include "Tiny3DStandardForward.cginc"
				
				ENDCG
			}

			Pass
			{
				Name "ShadowCaster"
				Tags { "LightMode" = "ShadowCaster" }

				CGPROGRAM
				#pragma vertex vertShadowCaster
				#pragma target 4.0

				#pragma multi_compile _ T3D_SKINNED_MESH 
				#pragma multi_compile _ T3D_GPU_SKINNING

				#include "Tiny3DStandardShadow.cginc"

				ENDCG
			}
		}
	}
}



			