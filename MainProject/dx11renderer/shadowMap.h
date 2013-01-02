#pragma once



class ShadowMap
{
public:
    ShadowMap(uint shadowMapWidth, uint shadowMapHeight);
    ~ShadowMap();

    ID3D11ShaderResourceView* DepthMapSRV();

    void BindDsvAndSetNullRenderTarget();

private:
    ShadowMap(const ShadowMap& rhs);
    ShadowMap& operator=(const ShadowMap& rhs);

private:
    uint                        m_shadowWidth;
    uint                        m_shadowHeight;

    ID3D11DepthStencilView*     m_depthStencilView;
    D3D11_VIEWPORT              m_viewport;
    ID3D11ShaderResourceView*   m_depthMap;

};