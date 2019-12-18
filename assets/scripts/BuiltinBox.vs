struct VertexIn
{
    float3 PosL;
    float4 Color;
};

struct VertexOut
{
    float4 PosH;
    float4 Color;
};

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

cbuffer type_cbRarely : register(b2)
{
    row_major float4x4 cbRarely_TINY3D_MATRIX_P : packoffset(c0);
    row_major float4x4 cbRarely_TINY3D_MATRIX_I_P : packoffset(c4);
    row_major float4x4 cbRarely_TINY3D_MATRIX_T_P : packoffset(c8);
    row_major float4x4 cbRarely_TINY3D_MATRIX_IT_P : packoffset(c12);
};


static float4 gl_Position;
static float3 in_var_POSITION0;
static float4 in_var_COLOR0;
static float4 out_var_COLOR0;

struct SPIRV_Cross_Input
{
    float3 in_var_POSITION0 : TEXCOORD0;
    float4 in_var_COLOR0 : TEXCOORD1;
};

struct SPIRV_Cross_Output
{
    float4 out_var_COLOR0 : TEXCOORD0;
    float4 gl_Position : SV_Position;
};

#line 61 "C:\Users\aaronwang\workspace\projects\private\Tiny3D\steps\source\step09\vs2015-x64\..\..\..\assets\scripts\\BuiltinBox.hlsl"
VertexOut src_VS(VertexIn vin)
{
#line 63 "C:\Users\aaronwang\workspace\projects\private\Tiny3D\steps\source\step09\vs2015-x64\..\..\..\assets\scripts\\BuiltinBox.hlsl"
#line 67 "C:\Users\aaronwang\workspace\projects\private\Tiny3D\steps\source\step09\vs2015-x64\..\..\..\assets\scripts\\BuiltinBox.hlsl"
#line 67 "C:\Users\aaronwang\workspace\projects\private\Tiny3D\steps\source\step09\vs2015-x64\..\..\..\assets\scripts\\BuiltinBox.hlsl"
#line 67 "C:\Users\aaronwang\workspace\projects\private\Tiny3D\steps\source\step09\vs2015-x64\..\..\..\assets\scripts\\BuiltinBox.hlsl"
#line 67 "C:\Users\aaronwang\workspace\projects\private\Tiny3D\steps\source\step09\vs2015-x64\..\..\..\assets\scripts\\BuiltinBox.hlsl"
#line 67 "C:\Users\aaronwang\workspace\projects\private\Tiny3D\steps\source\step09\vs2015-x64\..\..\..\assets\scripts\\BuiltinBox.hlsl"
    VertexOut vout;
    vout.PosH = mul(cbPerObject_TINY3D_MATRIX_MVP, float4(vin.PosL, 1.0f));
#line 70 "C:\Users\aaronwang\workspace\projects\private\Tiny3D\steps\source\step09\vs2015-x64\..\..\..\assets\scripts\\BuiltinBox.hlsl"
#line 70 "C:\Users\aaronwang\workspace\projects\private\Tiny3D\steps\source\step09\vs2015-x64\..\..\..\assets\scripts\\BuiltinBox.hlsl"
#line 70 "C:\Users\aaronwang\workspace\projects\private\Tiny3D\steps\source\step09\vs2015-x64\..\..\..\assets\scripts\\BuiltinBox.hlsl"
    vout.Color = vin.Color;
#line 72 "C:\Users\aaronwang\workspace\projects\private\Tiny3D\steps\source\step09\vs2015-x64\..\..\..\assets\scripts\\BuiltinBox.hlsl"
#line 72 "C:\Users\aaronwang\workspace\projects\private\Tiny3D\steps\source\step09\vs2015-x64\..\..\..\assets\scripts\\BuiltinBox.hlsl"
    return vout;
}

#line 61 "C:\Users\aaronwang\workspace\projects\private\Tiny3D\steps\source\step09\vs2015-x64\..\..\..\assets\scripts\\BuiltinBox.hlsl"
void vert_main()
{
#line 61 "C:\Users\aaronwang\workspace\projects\private\Tiny3D\steps\source\step09\vs2015-x64\..\..\..\assets\scripts\\BuiltinBox.hlsl"
#line 51 "C:\Users\aaronwang\workspace\projects\private\Tiny3D\steps\source\step09\vs2015-x64\..\..\..\assets\scripts\\BuiltinBox.hlsl"
#line 52 "C:\Users\aaronwang\workspace\projects\private\Tiny3D\steps\source\step09\vs2015-x64\..\..\..\assets\scripts\\BuiltinBox.hlsl"
#line 61 "C:\Users\aaronwang\workspace\projects\private\Tiny3D\steps\source\step09\vs2015-x64\..\..\..\assets\scripts\\BuiltinBox.hlsl"
    VertexIn _36 = { in_var_POSITION0, in_var_COLOR0 };
    VertexIn param_var_vin = _36;
#line 61 "C:\Users\aaronwang\workspace\projects\private\Tiny3D\steps\source\step09\vs2015-x64\..\..\..\assets\scripts\\BuiltinBox.hlsl"
    VertexOut _38 = src_VS(param_var_vin);
#line 61 "C:\Users\aaronwang\workspace\projects\private\Tiny3D\steps\source\step09\vs2015-x64\..\..\..\assets\scripts\\BuiltinBox.hlsl"
#line 57 "C:\Users\aaronwang\workspace\projects\private\Tiny3D\steps\source\step09\vs2015-x64\..\..\..\assets\scripts\\BuiltinBox.hlsl"
    gl_Position = _38.PosH;
#line 61 "C:\Users\aaronwang\workspace\projects\private\Tiny3D\steps\source\step09\vs2015-x64\..\..\..\assets\scripts\\BuiltinBox.hlsl"
#line 58 "C:\Users\aaronwang\workspace\projects\private\Tiny3D\steps\source\step09\vs2015-x64\..\..\..\assets\scripts\\BuiltinBox.hlsl"
    out_var_COLOR0 = _38.Color;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    in_var_POSITION0 = stage_input.in_var_POSITION0;
    in_var_COLOR0 = stage_input.in_var_COLOR0;
    vert_main();
    SPIRV_Cross_Output stage_output;
    stage_output.gl_Position = gl_Position;
    stage_output.out_var_COLOR0 = out_var_COLOR0;
    return stage_output;
}
