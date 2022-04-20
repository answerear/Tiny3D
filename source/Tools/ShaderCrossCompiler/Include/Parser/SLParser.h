#pragma once

#include <stdio.h>
#include <stack>

#include "T3DSCCPrerequisites.h"
#include "SLParserData.h"

using namespace shaderlab;

extern SLShader*					g_CurrentShader;
extern std::stack<SLShaderState*>	g_ShaderStateStack;
extern const char*                  g_CurrentProgramCode;
extern int32_t                        g_CurrentProgramLine;
extern ProgramType					g_CurrentProgramType;

void InitLex(const char *c, int32_t length);

void FreeLex();

int32_t shaderlabparse();

SLShader* ParseShaderLab(const char* data, int32_t length);