#ifndef TINY3D_SHADER_VARIABLES_INCLUDED
#define TINY3D_SHADER_VARIABLES_INCLUDED

#define kMaxBoneMatrices 200
#define kMaxBlendBones 4

#define BLINN_PHONG
#define T3D_MAX_POINT_LIGHTS	4	// 点光源支持的数量
#define T3D_MAX_SPOT_LIGHTS		4	// 聚光灯支持的数量

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
   row_major float4x4 tiny3d_MatrixLightSpaceVP;
   row_major float4x4 tiny3d_BoneMatrices[kMaxBoneMatrices];
}

static float4x4 tiny3d_MatrixMVP = mul(tiny3d_MatrixVP, tiny3d_ObjectToWorld);
static float4x4 tiny3d_MatrixMV = mul(tiny3d_MatrixV, tiny3d_ObjectToWorld);
static float4x4 tiny3d_MatrixLightSpaceMVP = mul(tiny3d_MatrixLightSpaceVP, tiny3d_ObjectToWorld);

#define TINY3D_MATRIX_M          tiny3d_ObjectToWorld
#define TINY3D_MATRIX_V          tiny3d_MatrixV
#define TINY3D_MATRIX_P          tiny3d_MatrixP
#define TINY3D_MATRIX_VP         tiny3d_MatrixVP
#define TINY3D_MATRIX_MV         tiny3d_MatrixMV
#define TINY3D_MATRIX_MVP        tiny3d_MatrixMVP
#define TINY3D_MATRIX_LIGHTSPACE tiny3d_MatrixLightSpaceMVP
#define TINY3D_MATRIX_LIGHTSPACE_VP tiny3d_MatrixLightSpaceVP

cbuffer tiny3d_LightParams
{
    // 环境光
	float4 tiny3d_AmbientLight;	// rgb - 环境光颜色, a - 强度

	// 平行光，只允许一个
	float4 tiny3d_DirLightColor;	// rgb - 颜色, a - diffuse 强度
	float4 tiny3d_DirLightDir;		// xyz - 方向, w - specular 强度

	// 点光源
	float4 tiny3d_PointLightColor[T3D_MAX_POINT_LIGHTS];	// rgb - 颜色, a - diffuse 强度
	float4 tiny3d_PointLightPos[T3D_MAX_POINT_LIGHTS];		// xyz - 位置, w - specular 强度
	float4 tiny3d_PointLightAttenuation[T3D_MAX_POINT_LIGHTS];	// x - 衰减常量项, y - 衰减一次项, z - 衰减二次项, w - padding

	// 聚光灯
	float4 tiny3d_SpotLightColor[T3D_MAX_SPOT_LIGHTS];			// rgb - 颜色, a - diffuse 强度
	float4 tiny3d_SpotLightPos[T3D_MAX_SPOT_LIGHTS];			// xyz - 位置, w - specular 强度
	float4 tiny3d_SpotLightDir[T3D_MAX_SPOT_LIGHTS];			// xyz - 方向, w - cutoff
	float4 tiny3d_SpotLightAttenuation[T3D_MAX_SPOT_LIGHTS];	// x - 衰减常量项, y - 衰减一次项, z - 衰减二次项, w - outer cutoff
}

cbuffer tiny3d_CameraParams
{
    float4 tiny3d_CameraWorldPos;	// 相机世界位置
}

cbuffer tiny3d_ObjectParams
{
    float4 tiny3d_ObjectSmoothness;	// x - 物体光滑度, yzw - padding
}

cbuffer tiny3d_ProjectionParamsCB
{
	float4 tiny3d_ProjectionParams;	// x - +1.0 正常 / -1.0 投影Y翻转(OpenGL RTT), y - +1.0 shadow map未翻转 / -1.0 shadow map Y翻转, zw - 保留
}

#define T3D_SHADOWMAP_WIDTH		2048
#define T3D_SHADOWMAP_HEIGHT	2048

#endif  /*TINY3D_SHADER_VARIABLES_INCLUDED*/