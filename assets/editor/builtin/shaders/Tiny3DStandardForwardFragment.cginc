#ifndef TINY3D_STANDARD_FORWARD_FRAGMENT_INCLUDED
#define TINY3D_STANDARD_FORWARD_FRAGMENT_INCLUDED

TEXTURE2D_COMP_STATE(shadowMap);

// 将裁剪空间投影坐标转换为阴影贴图采样用的 UV 和深度值
// 输入: 透视除法后的 projCoords (xyz)
// 输出: float3(u, v, depth)，均在 [0,1] 范围
// 注：通过 tiny3d_ProjectionParams.y 运行时判断 shadow map 是否被 Y 翻转，
//     无需编译时 TINY3D_DIRECTX / TINY3D_OPENGL 条件编译
float3 clipToShadowUV(float3 projCoords)
{
	projCoords.xy = projCoords.xy * 0.5 + 0.5;
	// shadow map 未被 Y 翻转时（D3D11 路径），需要 1-y 匹配纹理原点方向
	// shadow map 已被 Y 翻转时（OpenGL RTT 路径），lightSpaceMatrix 已包含翻转，跳过 1-y
	if (tiny3d_ProjectionParams.y > 0.0f)
		projCoords.y = 1.0f - projCoords.y;
	return projCoords;
}

// 计算阴影
float calcShadow(float4 lightSpacePos)
{
	// 执行透视除法
	float3 projCoords = lightSpacePos.xyz / lightSpacePos.w;
	// 变换到[0,1]的范围
	projCoords = clipToShadowUV(projCoords);

	float shadow = 0.0f;
	float2 texelSize = 1.0f / float2(T3D_SHADOWMAP_WIDTH, T3D_SHADOWMAP_HEIGHT);
	const float2 offsets[9] = {
		float2(-1, -1), float2(0, -1), float2(1, -1),
		float2(-1, 0), float2(0, 0), float2(1, 0),
		float2(-1, 1), float2(0, 1), float2(1, 1)
	};
	float depth = projCoords.z;

	[unroll]
	for (int i = 0; i < 9; i++)
	{
		float2 uv = projCoords.xy + offsets[i] * texelSize;
		shadow += shadowMap.SampleCmpLevelZero(SAMPLER(shadowMap), uv, depth);
	}
	shadow /= 9.0f;
	return shadow;
}

// 计算衰减值
float calcLightAttenuation(float3 lightPos, float3 pos, float attenuationConstant, float attenuationLinear, float attenuationQuadratic)
{
	float distance = length(lightPos - pos);
	float denom = attenuationConstant + attenuationLinear * distance + attenuationQuadratic * distance * distance;
	float attenuation = (denom > 0.0) ? (1.0 / denom) : 0.0;
	return attenuation;
}

// 计算漫反射
float calculateDiffuse(float3 normal, float3 lightDir)
{
	return max(0, dot(normal, lightDir));
}

// 计算镜面反射
float calculateSpecular(float3 viewDir, float3 normal, float3 lightDir, float smoothness)
{
	float shininess = smoothness * 128.0;
#if defined (BLINN_PHONG)
	float3 halfDir = normalize(lightDir + viewDir);
	return pow(max(dot(viewDir, halfDir), 0.0), shininess);
#else
	float3 reflectDir = reflect(-lightDir, normal);
	return pow(max(dot(viewDir, reflectDir), 0.0), shininess);
#endif
}

// 计算方向光
float3 calcDirectionalLight(float3 normal, float3 viewDir, float3 albedo, float3 lightDir, float3 lightColor, float diffuseIntensity, float specularIntensity, float smoothness)
{
	// 漫反射系数
	float diffuse = calculateDiffuse(normal, -lightDir);

	// 镜面反射系数
	float specular = calculateSpecular(viewDir, normal, -lightDir, smoothness);

	// 最终颜色
	return (diffuse * diffuseIntensity + specular * specularIntensity) * lightColor * albedo;
}

// 计算点光源
float3 calcPointLight(float3 pos, float3 normal, float3 viewDir, float3 albedo, float3 lightPos, float3 lightColor, float diffuseIntensity, float specularIntensity, float3 attenuation, float smoothness)
{
	// 计算衰减值
	float att = calcLightAttenuation(lightPos, pos, attenuation.x, attenuation.y, attenuation.z);

	// 计算光源方向
	float3 lightDir = normalize(lightPos - pos);

	// 漫反射系数
	float diffuse = calculateDiffuse(normal, lightDir);

	// 镜面反射系数
	float specular = calculateSpecular(viewDir, normal, lightDir, smoothness);

	// 最终颜色
	return (diffuse * diffuseIntensity + specular * specularIntensity) * lightColor * albedo * att;
}

// 计算聚光灯
float3 calcSpotLight(float3 pos, float3 normal, float3 viewDir, float3 albedo, float3 lightPos, float3 lightDir, float3 lightColor, float diffuseIntensity, float specularIntensity, float3 attenuation, float smoothness, float cutoff, float outerCutoff)
{
	// 计算衰减值
	float att = calcLightAttenuation(lightPos, pos, attenuation.x, attenuation.y, attenuation.z);

	// 计算光源和片段的方向
	float3 dir = normalize(lightPos - pos);

	float theta = dot(dir, -lightDir);	// 片段指向光源方向和光方向的夹角
	float epsilon = cutoff - outerCutoff;	// 切光角和外切光角的差
	float intensity = clamp((theta - outerCutoff) / epsilon, 0.0f, 1.0f);	// 根据切光角的差值，获取光照强度

	// 漫反射系数
	float diffuse = calculateDiffuse(normal, dir) * intensity;

	// 镜面反射系数
	float specular = calculateSpecular(viewDir, normal, dir, smoothness) * intensity;

	// 最终颜色
	return (diffuse * diffuseIntensity + specular * specularIntensity) * lightColor * albedo * att;
}

float4 fragForwardBase(VertexOutputForwardBase input) : SV_Target
{
	// 物体的光滑度
	float smoothness = tiny3d_ObjectSmoothness.x;

	// 计算环境光
	float3 ambient = tiny3d_AmbientLight.rgb * tiny3d_AmbientLight.a;

	// 计算观察方向的反方向，从表面指向相机
	float3 viewDir = normalize(tiny3d_CameraWorldPos.xyz - input.worldPos);

	// 物体表面纹理
	float4 albedo = SAMPLE(_MainTex, input.uv);

	// 计算方向光
	float3 color = float3(0, 0, 0);
	float diffuseIntensity = tiny3d_DirLightColor.a;
	float specularIntensity = tiny3d_DirLightDir.w;
	float3 lightDir = tiny3d_DirLightDir.xyz;
	float3 lightColor = tiny3d_DirLightColor.rgb;
	float3 dirColor = calcDirectionalLight(input.worldNormal, viewDir, albedo.rgb, lightDir, lightColor, diffuseIntensity, specularIntensity, smoothness);
	// 计算阴影
	float shadow = calcShadow(input.lightSpacePos);
	dirColor *= shadow;
	color += dirColor;

	int i = 0;
	// 计算点光源
	[unroll]
	for (i = 0; i < T3D_MAX_POINT_LIGHTS; i++)
	{
		diffuseIntensity = tiny3d_PointLightColor[i].a;
		specularIntensity = tiny3d_PointLightPos[i].w;
		float3 lightPos = tiny3d_PointLightPos[i].xyz;
		lightColor = tiny3d_PointLightColor[i].rgb;
		float3 pointColor = calcPointLight(input.worldPos, input.worldNormal, viewDir, albedo.rgb, lightPos, lightColor, diffuseIntensity, specularIntensity, tiny3d_PointLightAttenuation[i].xyz, smoothness);
		color += pointColor;
	}

	// 计算聚光灯
	[unroll]
	for (i = 0; i < T3D_MAX_SPOT_LIGHTS; i++)
	{
		diffuseIntensity = tiny3d_SpotLightColor[i].a;
		specularIntensity = tiny3d_SpotLightPos[i].w;
		float3 lightPos = tiny3d_SpotLightPos[i].xyz;
		float3 lightDir = tiny3d_SpotLightDir[i].xyz;
		lightColor = tiny3d_SpotLightColor[i].rgb;
		float cutoff = tiny3d_SpotLightDir[i].w;
		float outerCutoff = tiny3d_SpotLightAttenuation[i].w;
		float3 spotColor = calcSpotLight(input.worldPos, input.worldNormal, viewDir, albedo.rgb, lightPos, lightDir, lightColor, diffuseIntensity, specularIntensity, tiny3d_SpotLightAttenuation[i].xyz, smoothness, cutoff, outerCutoff);
		color += spotColor;
	}

	// 最后把环境光算上
	float4 finalColor = float4(ambient * albedo.rgb + color, 1.0f);
    
    return finalColor;
}

#endif  /*TINY3D_STANDARD_FORWARD_FRAGMENT_INCLUDED*/