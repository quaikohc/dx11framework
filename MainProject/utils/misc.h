#pragma once


std::wstring StringToWString(const std::string& s);
std::string WStringToString(const std::wstring& s);


static DirectX::XMMATRIX InverseTranspose( DirectX::CXMMATRIX M)
{
    DirectX::XMMATRIX A = M;
    A.r[3] =  DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

    DirectX::XMVECTOR det =  DirectX::XMMatrixDeterminant(A);
    return  DirectX::XMMatrixTranspose( DirectX::XMMatrixInverse(&det, A));
}