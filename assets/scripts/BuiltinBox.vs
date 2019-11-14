cbuffer type_cbPerObject : register(b0)
{
    column_major float4x4 cbPerObject_gWorldViewProj : packoffset(c0);
};


static float4 gl_Position;
static float3 in_var_POSITION;
static float4 in_var_COLOR;
static float4 out_var_COLOR;

struct SPIRV_Cross_Input
{
    float3 in_var_POSITION : TEXCOORD0;
    float4 in_var_COLOR : TEXCOORD1;
};

struct SPIRV_Cross_Output
{
    float4 out_var_COLOR : TEXCOORD0;
    float4 gl_Position : SV_Position;
};

void vert_main()
{
    gl_Position = mul(float4(in_var_POSITION, 1.0f), cbPerObject_gWorldViewProj);
    out_var_COLOR = in_var_COLOR;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    in_var_POSITION = stage_input.in_var_POSITION;
    in_var_COLOR = stage_input.in_var_COLOR;
    vert_main();
    SPIRV_Cross_Output stage_output;
    stage_output.gl_Position = gl_Position;
    stage_output.out_var_COLOR = out_var_COLOR;
    return stage_output;
}
