#pragma once



class CRenderToTexture
{
public:
    CRenderToTexture();
    ~CRenderToTexture();


private:
    uint                        m_width;
    uint                        m_height;

    ID3D11ShaderResourceView*   m_texture;
};