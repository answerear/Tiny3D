#ifndef TINY3D_INCLUDED
#define TINY3D_INCLUDED

#define CONCATENATE(a, b) a##b
#define TEXTURE2D(name) Texture2D name; SamplerState CONCATENATE(sampler, name)
#define TEXTURE2D_R(name, r) Texture2D name : register(CONCATENATE(t, r)); SamplerState CONCATENATE(sampler, name) : register(CONCATENATE(s, r));
#define SAMPLE(tex, uv) tex.Sample(CONCATENATE(sampler, tex), uv)
#define SAMPLER(tex) CONCATENATE(sampler, tex)
#define TEXTURE2D_COMP_STATE(name) Texture2D name; SamplerComparisonState CONCATENATE(sampler, name)


#endif  /*TINY3D_INCLUDED*/