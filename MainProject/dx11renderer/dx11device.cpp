#include "stdafx.h"
#include "dx11device.h"

#include <string>
#include <fstream>
#include <streambuf>


CDx11Device::CDx11Device()
{

}

CDx11Device::~CDx11Device()
{
    m_swapChain->Release();
    m_device->Release();
    m_context->Release();
    m_renderTargetView->Release();

    m_depthStencilView->Release();
    m_depthStencilBuffer->Release();

    m_backBuffer->Release();
}


bool CDx11Device::Initialize()
{
    HRESULT hr;
    DXGI_MODE_DESC bufferDesc;

    ZeroMemory(&bufferDesc, sizeof(DXGI_MODE_DESC));

    bufferDesc.Width                   = Globals::Get().app.GetSize().x;
    bufferDesc.Height                  = Globals::Get().app.GetSize().y;
    bufferDesc.RefreshRate.Numerator   = 60;
    bufferDesc.RefreshRate.Denominator = 1;
    bufferDesc.Format                  = DXGI_FORMAT_B8G8R8A8_UNORM;
    bufferDesc.ScanlineOrdering        = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    bufferDesc.Scaling                 = DXGI_MODE_SCALING_UNSPECIFIED;

    DXGI_SWAP_CHAIN_DESC swapChainDesc; 

    ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

    swapChainDesc.BufferDesc           = bufferDesc;
    swapChainDesc.SampleDesc.Count     = 1;
    swapChainDesc.SampleDesc.Quality   = 0;
    swapChainDesc.BufferUsage          = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount          = 1;
    swapChainDesc.OutputWindow         = Globals::Get().app.m_hWnd; 
    swapChainDesc.Windowed             = TRUE; 
    swapChainDesc.SwapEffect           = DXGI_SWAP_EFFECT_DISCARD;
                                       
    D3D_DRIVER_TYPE  driverType        = D3D_DRIVER_TYPE_HARDWARE;  //D3D_DRIVER_TYPE_NULL
    D3D_FEATURE_LEVEL featureLevel     = D3D_FEATURE_LEVEL_11_0;
                                       
    UINT createDeviceFlags             = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

//    #ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
   // #endif

    hr = D3D11CreateDeviceAndSwapChain(NULL, driverType, NULL, createDeviceFlags, NULL, NULL, D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_device, &featureLevel, &m_context);
           D3DPERF_SetOptions(0);
    if(FAILED(hr))return false;

    hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&m_backBuffer);
    hr = m_device->CreateRenderTargetView( m_backBuffer, NULL, &m_renderTargetView);

    D3D11_TEXTURE2D_DESC depthStencilDesc;

    depthStencilDesc.Width              = Globals::Get().app.GetSize().x;
    depthStencilDesc.Height             = Globals::Get().app.GetSize().y;
    depthStencilDesc.MipLevels          = 1;
    depthStencilDesc.ArraySize          = 1;
    depthStencilDesc.Format             = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilDesc.SampleDesc.Count   = 1;
    depthStencilDesc.SampleDesc.Quality = 0;
    depthStencilDesc.Usage              = D3D11_USAGE_DEFAULT;
    depthStencilDesc.BindFlags          = D3D11_BIND_DEPTH_STENCIL;
    depthStencilDesc.CPUAccessFlags     = 0; 
    depthStencilDesc.MiscFlags          = 0;

    m_device->CreateTexture2D(&depthStencilDesc, NULL, &m_depthStencilBuffer);
    m_device->CreateDepthStencilView(m_depthStencilBuffer, NULL, &m_depthStencilView);

    return true;
}
/*
 #ifdef _DEBUG
 deviceCreationFlags |= D3D11_CREATE_DEVICE_DEBUG;
 #endif
 ...
 ID3D11Debug *d3dDebug = nullptr;
 if( SUCCEEDED( d3dDevice->QueryInterface( __uuidof(ID3D11Debug), (void**)&d3dDebug ) ) )
 {
 ID3D11InfoQueue *d3dInfoQueue = nullptr;
 if( SUCCEEDED( d3dDebug->QueryInterface( __uuidof(ID3D11InfoQueue), (void**)&d3dInfoQueue ) ) )
 {
 #ifdef _DEBUG
 d3dInfoQueue->SetBreakOnSeverity( D3D11_MESSAGE_SEVERITY_CORRUPTION, true );
 d3dInfoQueue->SetBreakOnSeverity( D3D11_MESSAGE_SEVERITY_ERROR, true );
 #endif
 
 D3D11_MESSAGE_ID hide [] =
 {
 D3D11_MESSAGE_ID_SETPRIVATEDATA_CHANGINGPARAMS,
 // Add more message IDs here as needed
 };
 
 D3D11_INFO_QUEUE_FILTER filter;
 memset( &filter, 0, sizeof(filter) );
 filter.DenyList.NumIDs = _countof(hide);
 filter.DenyList.pIDList = hide;
 d3dInfoQueue->AddStorageFilterEntries( &filter );
 d3dInfoQueue->Release();
 }
 d3dDebug->Release();
 }

 */
//#define D3D_COMPILE_STANDARD_FILE_INCLUDE ((ID3DInclude*)(UINT_PTR)1)

HRESULT CDx11Device::CompileShaderFromFile(const WCHAR* fileName, LPCSTR entryPoint, LPCSTR shaderModel, ID3DInclude* include, ID3DBlob** outBlob)
{
    HRESULT hr = S_OK;

    /*
        D3D10_SHADER_MACRO defines[] = {
        {"MSAA_SAMPLES", msaaSamplesStr.c_str()},
        {0, 0}
    };

     const D3D_SHADER_MACRO defines[] = 
    {
        "EXAMPLE_DEFINE", "1",
        NULL, NULL
    };*/

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

    #if defined(DEBUG) || defined(_DEBUG)
    dwShaderFlags |= D3DCOMPILE_DEBUG;
    dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
    #endif

    ID3DBlob* errorBlob;

    std::ifstream t(fileName);
    std::stringstream buffer;
    buffer << t.rdbuf();
    std::string shaderContent = buffer.str();

    hr = D3DCompile((LPCVOID)shaderContent.c_str(), buffer.str().size(), NULL, NULL,  include , entryPoint, shaderModel, dwShaderFlags, 0, outBlob, &errorBlob);

    if(FAILED(hr))
    {
       
        if(errorBlob != NULL)
            OutputDebugStringA((char*)errorBlob->GetBufferPointer());
        if(errorBlob) errorBlob->Release();
            return hr;
    }

    if( errorBlob ) 
        errorBlob->Release();

    return S_OK;
}


D3D_FEATURE_LEVEL CDx11Device::GetAvailableFeatureLevel(D3D_DRIVER_TYPE DriverType)
{
    D3D_FEATURE_LEVEL FeatureLevel;

    FeatureLevel = m_device->GetFeatureLevel();

    return FeatureLevel;
}
