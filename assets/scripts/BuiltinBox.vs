cbuffer type_cbPerObject : register(b0)
{
    row_major float4x4 cbPerObject_TINY3D_MATRIX_M : packoffset(c0);
    row_major float4x4 cbPerObject_TINY3D_MATRIX_I_M : packoffset(c4);
    row_major float4x4 cbPerObject_TINY3D_MATRIX_T_M : packoffset(c8);
    row_major float4x4 cbPerObject_TINY3D_MATRIX_IT_M : packoffset(c12);
    row_major float4x4 cbPerObject_TINY3D_MATRIX_MV : packoffset(c16);
    row_major float4x4 cbPerObject_TINY3D_MATRIX_I_MV : packoffset(c20);
    row_major float4x4 cbPerObject_TINY3D_MATRIX_T_MV : packoffset(c24);
    row_major float4x4 cbPerObject_TINY3D_MATRIX_IT_MV : packoffset(c28);
    row_major float4x4 cbPerObject_TINY3D_MATRIX_MVP : packoffset(c32);
    row_major float4x4 cbPerObject_TINY3D_MATRIX_I_MVP : packoffset(c36);
    row_major float4x4 cbPerObject_TINY3D_MATRIX_T_MVP : packoffset(c40);
    row_major float4x4 cbPerObject_TINY3D_MATRIX_IT_MVP : packoffset(c44);
};

cbuffer type_cbPerFrame : register(b1)
{
    row_major float4x4 cbPerFrame_TINY3D_MATRIX_V : packoffset(c0);
    row_major float4x4 cbPerFrame_TINY3D_MATRIX_I_V : packoffset(c4);
    row_major float4x4 cbPerFrame_TINY3D_MATRIX_T_V : packoffset(c8);
    row_major float4x4 cbPerFrame_TINY3D_MATRIX_IT_V : packoffset(c12);
    row_major float4x4 cbPerFrame_TINY3D_MATRIX_VP : packoffset(c16);
    row_major float4x4 cbPerFrame_TINY3D_MATRIX_I_VP : packoffset(c20);
    row_major float4x4 cbPerFrame_TINY3D_MATRIX_T_VP : packoffset(c24);
    row_major float4x4 cbPerFrame_TINY3D_MATRIX_IT_VP : packoffset(c28);
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
    float4 _34 = float4(in_var_POSITION, 1.0f);
    gl_Position = mul(cbPerObject_TINY3D_MATRIX_MVP, _34) + mul(cbPerFrame_TINY3D_MATRIX_V, _34);
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
