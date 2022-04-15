#include "Renderer/Renderable.h"
#include "Texture/DDSTextureLoader.h"

namespace library
{
    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
  Method:   Renderable::Renderable

  Summary:  Constructor

  Args:     const std::filesystem::path& textureFilePath
              Path to the texture to use

  Modifies: [m_vertexBuffer, m_indexBuffer, m_constantBuffer,
             m_textureRV, m_samplerLinear, m_vertexShader,
             m_pixelShader, m_textureFilePath, m_world].
M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    Renderable::Renderable(_In_ const std::filesystem::path& textureFilePath)
        : m_vertexBuffer()
        , m_indexBuffer()
        , m_constantBuffer()
        , m_textureRV()
        , m_samplerLinear()
        , m_vertexShader()
        , m_pixelShader()
        , m_textureFilePath(textureFilePath)
        , m_world() {}

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
  Method:   Renderable::initialize

  Summary:  Initializes the buffers, texture, and the world matrix

  Args:     ID3D11Device* pDevice
              The Direct3D device to create the buffers
            ID3D11DeviceContext* pImmediateContext
              The Direct3D context to set buffers

  Modifies: [m_vertexBuffer, m_indexBuffer, m_constantBuffer,
             m_textureRV, m_samplerLinear, m_world].

  Returns:  HRESULT
              Status code
M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    HRESULT Renderable::initialize(_In_ ID3D11Device* pDevice, _In_ ID3D11DeviceContext* pImmediateContext)
    {
        D3D11_BUFFER_DESC bd = {};
        HRESULT hr = S_OK;
        // Create vertex buffer

        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = GetNumVertices() * sizeof(SimpleVertex); // ;  
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bd.CPUAccessFlags = 0;

        D3D11_SUBRESOURCE_DATA InitData = {};
        InitData.pSysMem = getVertices();
        hr = pDevice->CreateBuffer(&bd, &InitData, m_vertexBuffer.GetAddressOf());
        if (FAILED(hr))
            return hr;

        // Create Index Buffer

        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = sizeof(WORD) * GetNumIndices();    //     // 36 vertices needed for 12 triangles in a triangle list
        bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
        bd.CPUAccessFlags = 0;
        InitData.pSysMem = getIndices();
        hr = pDevice->CreateBuffer(&bd, &InitData, m_indexBuffer.GetAddressOf());
        if (FAILED(hr))
            return hr;  

         

        bd.ByteWidth = sizeof(CBChangesEveryFrame);
        hr = pDevice->CreateBuffer(&bd, nullptr, m_constantBuffer.GetAddressOf());

        m_world = XMMatrixIdentity();
 
//      Shaders and renderables are stored in Hash maps
//      Strings are used as the key, and the shader / renderable objects are the value
//      When iterating, use iterators

        //hr = D3DX11CreateShaderResourceViewFromFile(g_pd3dDevice, L"seafloor.dds", NULL, NULL,&g_pTextureRV, NULL);

        //create sample state
        D3D11_SAMPLER_DESC sampDesc;
        ZeroMemory(&sampDesc, sizeof(sampDesc));
        sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
        sampDesc.MinLOD = 0;
        sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
        hr = pDevice->CreateSamplerState(&sampDesc, m_samplerLinear.GetAddressOf());

        hr = CreateDDSTextureFromFile(pDevice, m_textureFilePath.filename().wstring().c_str(), nullptr, m_textureRV.GetAddressOf());
        //textureFilePath.filename().wstring().c_str()
        if (FAILED(hr)) 
            return hr;
        ////////////////// ¿Ã∫Œ∫–?

        return hr;
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderable::SetVertexShader

      Summary:  Sets the vertex shader to be used for this renderable 
                object

      Args:     const std::shared_ptr<VertexShader>& vertexShader
                  Vertex shader to set to

      Modifies: [m_vertexShader].
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    void Renderable::SetVertexShader(_In_ const std::shared_ptr<VertexShader>& vertexShader)
    {
        m_vertexShader = vertexShader;
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderable::SetPixelShader

      Summary:  Sets the pixel shader to be used for this renderable
                object

      Args:     const std::shared_ptr<PixelShader>& pixelShader
                  Pixel shader to set to

      Modifies: [m_pixelShader].
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    void Renderable::SetPixelShader(_In_ const std::shared_ptr<PixelShader>& pixelShader)
    {
        m_pixelShader = pixelShader;
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderable::GetVertexShader

      Summary:  Returns the vertex shader

      Returns:  ComPtr<ID3D11VertexShader>&
                  Vertex shader. Could be a nullptr
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    ComPtr<ID3D11VertexShader>& Renderable::GetVertexShader()
    {
        return m_vertexShader -> GetVertexShader();
    }
     
    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderable::GetPixelShader

      Summary:  Returns the vertex shader

      Returns:  ComPtr<ID3D11PixelShader>&
                  Pixel shader. Could be a nullptr
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    ComPtr<ID3D11PixelShader>& Renderable::GetPixelShader()
    {
        return m_pixelShader -> GetPixelShader();
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderable::GetVertexLayout

      Summary:  Returns the vertex input layout

      Returns:  ComPtr<ID3D11InputLayout>&
                  Vertex input layout
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    ComPtr<ID3D11InputLayout>& Renderable::GetVertexLayout()
    {
        return m_vertexShader->GetVertexLayout();
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderable::GetVertexBuffer

      Summary:  Returns the vertex buffer

      Returns:  ComPtr<ID3D11Buffer>&
                  Vertex buffer
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    ComPtr<ID3D11Buffer>& Renderable::GetVertexBuffer()
    {
        return m_vertexBuffer;
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderable::GetIndexBuffer

      Summary:  Returns the index buffer

      Returns:  ComPtr<ID3D11Buffer>&
                  Index buffer
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    ComPtr<ID3D11Buffer>& Renderable::GetIndexBuffer()
    {
        return m_indexBuffer;
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderable::GetConstantBuffer

      Summary:  Returns the constant buffer

      Returns:  ComPtr<ID3D11Buffer>&
                  Constant buffer
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    ComPtr<ID3D11Buffer>& Renderable::GetConstantBuffer()
    {
        return m_constantBuffer;
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderable::GetWorldMatrix

      Summary:  Returns the world matrix

      Returns:  const XMMATRIX&
                  World matrix
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    const XMMATRIX& Renderable::GetWorldMatrix() const
    {
        return m_world;
    }

    ComPtr<ID3D11ShaderResourceView>& Renderable::GetTextureResourceView()
    {
        return m_textureRV;
    }

    ComPtr<ID3D11SamplerState>& Renderable::GetSamplerState()
    {
        return m_samplerLinear;
    }
}
