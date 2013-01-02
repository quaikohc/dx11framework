#pragma once


struct DirectionalLight
{
       __declspec(align(16)) XMFLOAT4 Ambient;
       __declspec(align(16)) XMFLOAT4 Diffuse;
       __declspec(align(16)) XMFLOAT4 Specular;
       __declspec(align(16)) XMFLOAT3 Direction;

   // float Pad;
};


//struct DirectionalLight
//{
//    DirectionalLight() 
//    { 
//        ZeroMemory(this, sizeof(this)); 
//    }
//
//    DirectX::XMFLOAT4 Ambient;
//    DirectX::XMFLOAT4 Diffuse;
//    DirectX::XMFLOAT4 Specular;
//    DirectX::XMFLOAT3 Direction;
//
//    float Pad;
//};

struct PointLight
{
    PointLight() 
    { 
        ZeroMemory(this, sizeof(this)); 
    }

    DirectX::XMFLOAT4 Ambient;
    DirectX::XMFLOAT4 Diffuse;
    DirectX::XMFLOAT4 Specular;

    DirectX::XMFLOAT3 Position;
    float Range;

    DirectX::XMFLOAT3 Att; 
    float Pad;
};



class CLight
{

public:
    CLight();
    ~CLight();
};