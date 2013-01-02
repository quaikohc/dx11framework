#pragma once



struct Vertex
{
    Vertex()
    {
    }
    Vertex(const XMFLOAT3& p, const XMFLOAT3& n, const XMFLOAT3& t, const XMFLOAT2& uv) : Position(p), Normal(n), TangentU(t), TexC(uv)
    {
    }
    Vertex(float px, float py, float pz, 
        float nx, float ny, float nz,
        float tx, float ty, float tz,
        float u, float v): Position(px,py,pz), Normal(nx,ny,nz), TangentU(tx, ty, tz), TexC(u,v)
    {
    }

    XMFLOAT3 Position;
    XMFLOAT3 Normal;
    XMFLOAT3 TangentU;
    XMFLOAT2 TexC;
};

struct MeshData
{
    std::vector<Vertex> Vertices;
    std::vector<UINT>   Indices;
};


class GeometryGenerator
{
public:
    void        CreateBox(float width, float height, float depth, UINT numSubdivisions, MeshData& meshData);
    void        CreateSphere(float radius, UINT sliceCount, UINT stackCount, MeshData& meshData);

    void        CreatePlane(float width, MeshData& meshData);
    void        CreateGrid(float width, float depth, UINT m, UINT n, MeshData& meshData);

    void        CreateFullscreenQuad(MeshData& meshData);

    void        Subdivide(MeshData& meshData);
};