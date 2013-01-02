


// wrapper class for managing shaders const buffers
// WORK IN PROGRESS

// TODO:
// size validation using shader reflection

template<typename T>
class CConstBuffer
{
    ID3D11Buffer* m_constBuffer;
    T             m_data;

public:
    CConstBuffer() : m_constBuffer(nullptr)
    {
    }
    ~CConstBuffer()
    {
    }

    ID3D11Buffer* GetD3DBuffer() const
    {
        return m_data;	
    }

    CConstBuffer(CConstBuffer&& p): ptr(p.m_constBuffer) 
    {
        p.m_constBuffer = nullptr;
    }

    CConstBuffer& operator=(CConstBuffer&& p)
    {
        std::swap(m_constBuffer, p.m_constBuffer);
        return *this;
    }

    bool Initialize() 
    {
        HRESULT hr = S_OK;

        D3D11_BUFFER_DESC cbbd;
        ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));

        cbbd.Usage          = D3D11_USAGE_DEFAULT;
        cbbd.ByteWidth      = static_cast<UINT>(sizeof(T) + (16 - (sizeof(T) % 16))); // make multiple of 16
        cbbd.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
        cbbd.CPUAccessFlags = 0;

        hr = GetDX11Device()->CreateBuffer(&cbbd, NULL, &m_constBuffer);
        if(FAILED(hr)) assert(false);
    }

    bool ApplyChanges(T data)
    {
         GetDX11Context()->UpdateSubresource(data, 0, NULL, &m_constBuffer, 0, 0);
    }

    bool InitializeForMapping() 
    {
        HRESULT hr = S_OK;

        D3D11_BUFFER_DESC desc;
        desc.Usage               = D3D11_USAGE_DYNAMIC;
        desc.BindFlags           = D3D11_BIND_CONSTANT_BUFFER;
        desc.CPUAccessFlags      = D3D11_CPU_ACCESS_WRITE;
        desc.MiscFlags           = 0;  
        desc.ByteWidth           = static_cast<UINT>(sizeof(T) + (16 - (sizeof(T) % 16)));
        desc.StructureByteStride = 0;
        
        hr = GetDX11Device()->CreateBuffer(&desc, NULL, &m_constBuffer);
        if(FAILED(hr)) assert(false);
    }

    bool ApplyChangesWithMapping(T data)
    {
        D3D11_MAPPED_SUBRESOURCE mappedResource;
        GetDX11Context()->Map(m_constBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
        CopyMemory(mappedResource.pData, &data, sizeof(T));
        GetDX11Context()->Unmap(m_constBuffer, 0);
    }

private:
    CConstBuffer(const CConstBuffer<T>& rhs);
    CConstBuffer<T>& operator=(const CConstBuffer<T>& rhs);
};
