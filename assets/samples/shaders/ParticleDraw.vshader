#pragma vertex main
#pragma target 5.0

struct Particle
{
    float3 position;
    float  life;
    float3 velocity;
    float  size;
};

cbuffer DrawParams : register(b0)
{
    row_major float4x4 gView;
    row_major float4x4 gProj;
    row_major float4x4 gViewProj;
    float4 gMode;   // x>0.5 时 iid 经 gVisible 间接索引
};

StructuredBuffer<Particle> gParticles : register(t0);
StructuredBuffer<uint> gVisible : register(t1);

struct VertexOutput
{
    float4 position : SV_POSITION;
    float4 color : COLOR0;
};

VertexOutput main(uint vid : SV_VertexID, uint iid : SV_InstanceID)
{
    uint particleIndex = (gMode.x > 0.5f) ? gVisible[iid] : iid;
    Particle p = gParticles[particleIndex];

    static const float2 kCorners[6] =
    {
        float2(-1.0f, -1.0f),
        float2( 1.0f, -1.0f),
        float2( 1.0f,  1.0f),
        float2(-1.0f, -1.0f),
        float2( 1.0f,  1.0f),
        float2(-1.0f,  1.0f)
    };

    float2 corner = kCorners[vid % 6];
    float3 camRight = float3(gView._11, gView._12, gView._13);
    float3 camUp    = float3(gView._21, gView._22, gView._23);
    float3 world = p.position + (camRight * corner.x + camUp * corner.y) * p.size;

    VertexOutput o;
    o.position = mul(gViewProj, float4(world, 1.0f));
    float t = saturate(p.life * 0.25f);
    o.color = float4(t, 0.6f, 1.0f - t, 1.0f);
    return o;
}
