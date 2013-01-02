#pragma once

#include <Gdiplus.h>
#include <directxmath.h>


// based on Frank's Luna sample

class DxFont
{

public:
    DxFont();
    ~DxFont();

    bool                           Initialize(const std::wstring & FontName, float FontSize, WORD FontStyle, bool AntiAliased);

    ID3D11ShaderResourceView*      GetFontSheetSRV();
    const CD3D11_RECT&             GetCharRect(WCHAR c);
    int                            GetSpaceWidth();
    int                            GetCharHeight();

    enum 
    {
        STYLE_NORMAL      = 0,
        STYLE_BOLD        = 1,
        STYLE_ITALIC      = 2,
        STYLE_BOLD_ITALIC = 3,
        STYLE_UNDERLINE   = 4,
        STYLE_STRIKEOUT   = 8
    };

private:

    void    MeasureChars(Gdiplus::Font& font, Gdiplus::Graphics& charGraphics);
    void    BuildFontSheetBitmap(Gdiplus::Font& font, Gdiplus::Graphics& charGraphics, Gdiplus::Bitmap& charBitmap, Gdiplus::Graphics& fontSheetGraphics);
    bool    BuildFontSheetTexture(Gdiplus::Bitmap& fontSheetBitmap);
    int     GetCharMinX(Gdiplus::Bitmap& charBitmap);
    int     GetCharMaxX(Gdiplus::Bitmap& charBitmap);
    int     GetEncoderClsid(const WCHAR* format, CLSID * pClsid);

    bool                         Initialized;

    static const WCHAR           StartChar = 33;
    static const WCHAR           EndChar   = 127;
    static const UINT            NumChars  = EndChar - StartChar;


    ID3D11ShaderResourceView*    FontSheetSRV;
    ID3D11Texture2D*             FontSheetTex;
    UINT                         TexWidth;
    UINT                         TexHeight;
    CD3D11_RECT                  CharRects[NumChars];
    int                          SpaceWidth;
    int                          CharHeight;
};


class DxSprite
{
public:
    DxSprite();
    ~DxSprite();

    bool                    Initialize();
    void                    BeginBatch(ID3D11ShaderResourceView* texSRV);
    void                    EndBatch();
    void                    Draw(const CD3D11_RECT& destinationRect, const CD3D11_RECT& sourceRect, XMCOLOR color);
    void                    DrawString(int X, int Y, const std::wstring& text, int R, int G, int B, int A, DxFont& F);


private:
    ID3D11VertexShader*     VS;
    ID3D11PixelShader*      PS;

    struct Sprite
    {
        Sprite():Color(1.0f, 1.0f, 1.0f, 1.0f), Z(0.0f), Angle(0.0f), Scale(1.0f)
        {
        }

        CD3D11_RECT         SrcRect;
        CD3D11_RECT         DestRect;

        XMCOLOR             Color;

        float               Z;
        float               Angle;
        float               Scale;
    };

    struct SpriteVertex
    {
        XMFLOAT3            Pos;
        XMFLOAT2            Tex;
        XMCOLOR             Color;
    };

    void                        DrawBatch(UINT startSpriteIndex, UINT spriteCount);
    XMFLOAT3                    PointToNdc(int x, int y, float z);
    void                        BuildSpriteQuad(const Sprite & sprite, SpriteVertex v[4]);

    bool                        Initialized;
    ID3D11BlendState*           TransparentBS;


    ID3D11ShaderResourceView*   BatchTexSRV;
    ID3D11InputLayout*          InputLayout;
    ID3D11Buffer*               VB;
    ID3D11Buffer*               IB;
    UINT                        TexWidth;
    UINT                        TexHeight;
    std::vector<Sprite>         SpriteList;
    float                       ScreenWidth;
    float                       ScreenHeight;
};


extern DxSprite                 D3DSprite;


void DrawString(int X, int Y, const std::wstring& text, int R, int G, int B, int A, DxFont& F);
void DrawString(int X, int Y, char* txt, int R, int G, int B, int A, DxFont& F);
