#pragma once



class CDx11Device
{

public:
    HRESULT                     CompileShaderFromFile(const WCHAR* fileName, LPCSTR entryPoint, LPCSTR shaderModel, ID3DInclude* include, ID3DBlob** outBlob);
    
    D3D_FEATURE_LEVEL           GetAvailableFeatureLevel(D3D_DRIVER_TYPE DriverType);

public:
    CDx11Device();
    ~CDx11Device();
    
    bool                        Initialize();
    
    IDXGISwapChain*             m_swapChain;

    ID3D11Device*               m_device;
    ID3D11DeviceContext*        m_context;

    ID3D11RenderTargetView*     m_renderTargetView;
    ID3D11DepthStencilView*     m_depthStencilView;
    ID3D11Texture2D*            m_depthStencilBuffer;
    ID3D11Texture2D*            m_backBuffer;

};