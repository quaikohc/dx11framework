#pragma once



class RasterizerStates
{
public:
    static void InitAll();
    static void DestroyAll();

    static ID3D11RasterizerState*    WireframeRS;
    static ID3D11RasterizerState*    NoCullRS;
    static ID3D11RasterizerState*    DepthRS;
    static ID3D11RasterizerState*    CullFrontRS;
    static ID3D11RasterizerState*    CullBackRS;
};

class BlendStates
{
public:
    static void InitAll();
    static void DestroyAll();

    static ID3D11BlendState*         AlphaToCoverageBS;
    static ID3D11BlendState*         TransparentBS;
    static ID3D11BlendState*         MaxBS;
    static ID3D11BlendState*         DisabledBS;
    static ID3D11BlendState*         AlphaBlendBS;
};

class DepthStencilStates
{
public:
    static void InitAll();
    static void DestroyAll();

    static ID3D11DepthStencilState*  DepthNoWriteDSS;
    static ID3D11DepthStencilState*  DepthDisabledDSS;
    static ID3D11DepthStencilState*  DepthTestDSS;
};

class SamplerStates
{
public:
    static void InitAll();
    static void DestroyAll();

    static ID3D11SamplerState*       TerrainHeightSampler;
    static ID3D11SamplerState*       PointWrapSampler;
    static ID3D11SamplerState*       DefaultSamplerWrap;
    static ID3D11SamplerState*       DefaultSamplerClamp;
    static ID3D11SamplerState*       ShadowSampler;
};