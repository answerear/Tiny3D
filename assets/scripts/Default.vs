cbuffer type_type_Globals_VP : register(b0)
{
    column_major float4x4 type_Globals_VP_Globals_ViewProj : packoffset(c0);
};

cbuffer type_type_Globals_M : register(b1)
{
    column_major float4x4 type_Globals_M_Globals_Model : packoffset(c0);
};


static float4 gl_Position;
static float4 in_var_POSITION;
static float4 in_var_COLOR;
static float4 out_var_COLOR;

struct SPIRV_Cross_Input
{
    float4 in_var_POSITION : TEXCOORD0;
    float4 in_var_COLOR : TEXCOORD1;
};

struct SPIRV_Cross_Output
{
    float4 out_var_COLOR : TEXCOORD0;
    float4 gl_Position : SV_Position;
};

void vert_main()
{
    gl_Position = mul(in_var_POSITION, mul(type_Globals_VP_Globals_ViewProj, type_Globals_M_Globals_Model));
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
