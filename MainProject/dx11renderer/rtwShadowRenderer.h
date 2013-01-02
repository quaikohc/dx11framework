
/*

original paper by Paul Rosen - "Rectilinear Texture Warping for Fast Adaptive Shadow Mapping" 
http://broniac.blogspot.com/2012/06/rectilinear-texture-warping-for-fast.html

The 2D importance map is speciﬁed as a regular grid along the lights image plane
Each grid cell (pixel) receives a non-negative ﬂoating point importance Iuv ∈ [0, inf) 
where non-zero values imply a relative importance while zero implies unneeded data

Forward method analyze the scene from the light’s view
Backwards - desired view (camera)

Forward Analysis The forward analysis method begins by rendering the scene from the light’s perspective into a depth image.

Backward Analysis The backward analysis method begins by rendering the output image with all shading information (except shadows),
saving the color and depth. Importance is determined by projecting the output samples backward into the light’s image space and analyzing their importance.

Rendering the Shadow Map
Rendering proceeds by computing an output domain position for every vertex. First, the
vertex V is projected with the view and projection matrices (ML) used for the conventional shadow map to the location P using P =ML · V . 
The projected location P is used to locate the vertical and horizontal super-cells in the warp maps, and the vertex is relocated
to its output image plane location P by applying the warp

**** Shadow Edge Function ( only works using forward analysis)

This method only  and is done by performing a depth discontinuity test on each texel of the depth image with its eight neighboring texels. This
is done by taking the difference between 2 texels and comparing the value to the shadow bias. Texels with shadow edges have their
full importance included while other regions receive a minimal importance α (in our examples α = 0.001).

*/


#define SCREEN_SIZE_X           1600.0f
#define SCREEN_SIZE_Y           900.0f

#define INITIAL_SHADOWMAP_SIZE  2048.0f // size of the depth buffer and depth normal buffer
#define FINAL_SHADOWMAP_SIZE    2048.0f



struct SImportanceVSCBuffer
{
    XMMATRIX  mtxView;
    XMMATRIX  mtxProj;
    XMMATRIX  mtxWorld;
    XMMATRIX  mtxShadowVP;
    XMFLOAT4  screenParam;
};

struct SImportancePSCBuffer
{
    float   minEdgeValue;
    float   edgeThreshold;
    float   normalMultiValue;
    float   depthOffset;
};

struct SShadowMapRTWCBuffer
{
    XMMATRIX    mtxShadowVP;
    XMMATRIX    mtxWorld;
};

struct SRenderFinalVSCbuffer
{
    XMMATRIX    mtxWorld;
    XMMATRIX    mtxWorldIt;
    XMMATRIX    mtxShadowVP;
    XMMATRIX    mtxView;
    XMMATRIX    mtxProj;
    float       g_texScale;
};

struct SRenderFinalPSCBuffer
{
    XMFLOAT4  lightDir;
};

struct SDepthCBuffer2
{
    XMMATRIX gWorldLightViewProj;
};

class CRtwShadowRenderer
{
    ID3D11InputLayout*              m_inputLayout;

    D3D11_VIEWPORT                  m_importanceViewport;
    D3D11_VIEWPORT                  m_finalShadowmapViewport;
    D3D11_VIEWPORT                  m_finalScreenViewport;
    D3D11_VIEWPORT                  m_warpMapViewport;

    ID3D11Texture2D*                m_importanceTex;
    ID3D11ShaderResourceView*       m_importanceMap;
    ID3D11RenderTargetView*         m_importanceRTView;
    ID3D11PixelShader*              m_importancePS;
    ID3D11VertexShader*             m_importanceVS;


    ID3D11DomainShader*             m_finalDS;
    ID3D11HullShader*               m_finalHS;


    ID3D11PixelShader*              m_calculateWarpPS;

    ID3D11Texture2D*                m_warpedShadowTex;
    ID3D11ShaderResourceView*       m_warpedShadowMap;
    ID3D11RenderTargetView*         m_warpedShadowRTView;

    ID3D11Buffer*                   m_importanceVSCBuffer;
    ID3D11Buffer*                   m_importancePSCBuffer;

    ID3D11Buffer*                   m_rtwShadowMapCBuffer;
    ID3D11Buffer*                   m_finalPSCBuffer;
    ID3D11Buffer*                   m_finalVSCBuffer;

    ///////////////////////

    ID3D11Texture1D*                m_horizontalWarpingTex;
    ID3D11Texture1D*                m_verticalWarpingTex;

    ID3D11ComputeShader*            m_warpHorizontal;
    ID3D11ComputeShader*            m_warpVertical;

    ID3D11UnorderedAccessView*      m_horizUAV;
    ID3D11UnorderedAccessView*      m_vertUAV;

    ID3D11ShaderResourceView*       m_horizWarpSRV;
    ID3D11ShaderResourceView*       m_vertWarpSRV;

    ID3D11Buffer*                   m_depthConstBuffer;
    //////////////

    ID3D11PixelShader*              m_shadowMapRtwPS;
    ID3D11VertexShader*             m_shadowMapRtwVS;

    ////////////////
    ID3D11DepthStencilView*         m_depthStencilView;
   
    ID3D11ShaderResourceView*       m_depthMap;
    ID3D11VertexShader*             m_depthVS;
    //////////////

    ID3D11PixelShader*              m_finalPS;
    ID3D11VertexShader*             m_finalVS;

    //////////////

//    ID3D11DepthStencilView*     m_depthStencilView;


    void            InitImportanceTex();
    void            InitViewports();

    void            InitImportanceShaders();
    void            CreateCBuffers();
    void            InitComputeShader();


    void            DrawImportance();

    void            DrawFirstPassDepth();

    void            GenerateWarpMapPS();
    void            GenerateWarpMapCS();

    void            DrawFinal();
    void            DrawRtwShadows();

public:
    CRtwShadowRenderer();
    ~CRtwShadowRenderer();


    bool            Initialize();
    void            Draw();


    ID3D11ShaderResourceView*  GetDepthMap()        const { return m_depthMap;        }
    ID3D11ShaderResourceView*  GetImportanceMap()   const { return m_importanceMap;   }
    ID3D11ShaderResourceView*  GetWarpedShadowMap() const { return m_warpedShadowMap; }

    ID3D11ShaderResourceView*  GetHorizontalWarpTex() const { return m_horizWarpSRV; }
    ID3D11ShaderResourceView*  GetVerticalWarpTex()   const { return m_vertWarpSRV;  }


};