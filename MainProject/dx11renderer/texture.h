#pragma once




class CTexture
{
    std::wstring fileName;

public:
    CTexture();
    ~CTexture();

    bool  LoadFromFile(std::wstring fileName);



};