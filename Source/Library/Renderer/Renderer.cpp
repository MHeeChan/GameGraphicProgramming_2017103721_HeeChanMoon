#include "Renderer/Renderer.h"

namespace library
{
    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::Renderer

      Summary:  Constructor

      Modifies: [m_driverType, m_featureLevel, m_d3dDevice, m_d3dDevice1,
                 m_immediateContext, m_immediateContext1, m_swapChain,
                 m_swapChain1, m_renderTargetView, m_depthStencil,
                 m_depthStencilView, m_cbChangeOnResize, m_camera,
                 m_projection, m_renderables, m_vertexShaders,
                 m_pixelShaders].
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Renderer::Renderer definition (remove the comment)
    --------------------------------------------------------------------*/
    Renderer::Renderer():
        m_driverType(D3D_DRIVER_TYPE_NULL), 
        m_featureLevel(D3D_FEATURE_LEVEL_11_0),
        m_d3dDevice(nullptr), m_d3dDevice1(nullptr),
        m_immediateContext(nullptr), m_immediateContext1(nullptr),
        m_swapChain(nullptr), m_swapChain1(nullptr),
        m_renderTargetView(nullptr),
        m_pszMainSceneName(),
        m_camera(XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f)),
        m_projection(),
        m_renderables(std::unordered_map<std::wstring, std::shared_ptr<Renderable>>()),
        m_vertexShaders(std::unordered_map<std::wstring, std::shared_ptr<VertexShader>>()),
        m_pixelShaders(std::unordered_map<std::wstring, std::shared_ptr<PixelShader>>()) ,
        m_scenes(std::unordered_map<std::wstring, std::shared_ptr<Scene>>())
        {};

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::Initialize
      Summary:  Creates Direct3D device and swap chain
      Args:     HWND hWnd
                  Handle to the window
      Modifies: [m_d3dDevice, m_featureLevel, m_immediateContext,
                 m_d3dDevice1, m_immediateContext1, m_swapChain1,
                 m_swapChain, m_renderTargetView, m_cbChangeOnResize,
                 m_projection, m_cbLights, m_camera, m_vertexShaders,
                 m_pixelShaders, m_renderables].
      Returns:  HRESULT
                  Status code
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Renderer::Initialize definition (remove the comment)
    --------------------------------------------------------------------*/

    HRESULT Renderer::Initialize(_In_ HWND hWnd) 
    {
        if (m_renderables.empty()) {}

        HRESULT hr = S_OK;

        RECT rc;
        GetClientRect(hWnd, &rc);
        UINT width = rc.right - rc.left;
        UINT height = rc.bottom - rc.top;

        UINT createDeviceFlags = 0;
#ifdef _DEBUG
        createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

        D3D_DRIVER_TYPE driverTypes[] =
        {
            D3D_DRIVER_TYPE_HARDWARE,
            D3D_DRIVER_TYPE_WARP,
            D3D_DRIVER_TYPE_REFERENCE,
        };
        UINT numDriverTypes = ARRAYSIZE(driverTypes);

        D3D_FEATURE_LEVEL featureLevels[] =
        {
            D3D_FEATURE_LEVEL_11_1,
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_10_1,
            D3D_FEATURE_LEVEL_10_0,
        };
        UINT numFeatureLevels = ARRAYSIZE(featureLevels);

        for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
        {
            m_driverType = driverTypes[driverTypeIndex];
            hr = D3D11CreateDevice(nullptr, m_driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
                D3D11_SDK_VERSION, &m_d3dDevice, &m_featureLevel, &m_immediateContext);

            if (hr == E_INVALIDARG)
            {
                hr = D3D11CreateDevice(nullptr, m_driverType, nullptr, createDeviceFlags, &featureLevels[1], numFeatureLevels - 1,
                    D3D11_SDK_VERSION, &m_d3dDevice, &m_featureLevel, &m_immediateContext);
            }

            if (SUCCEEDED(hr))
                break;
        }
        if (FAILED(hr))
            return hr;

        ComPtr<IDXGIFactory1>           dxgiFactory(nullptr);
        {
            ComPtr<IDXGIDevice>           dxgiDevice(nullptr);
            hr = m_d3dDevice.As(&dxgiDevice);
            if (SUCCEEDED(hr))
            {
                ComPtr<IDXGIAdapter>           adapter(nullptr);

                hr = dxgiDevice->GetAdapter(adapter.GetAddressOf());
                if (SUCCEEDED(hr))
                {
                    hr = adapter->GetParent(__uuidof(IDXGIFactory1), (&dxgiFactory));
                }
            }
        }
        if (FAILED(hr))
            return hr;

        //create swap chain
        ComPtr<IDXGIFactory2>           dxgiFactory2(nullptr);
        hr = dxgiFactory.As(&dxgiFactory2);
        if (dxgiFactory2)
        {
            // DirectX 11.1 or later
            hr = m_d3dDevice.As(&m_d3dDevice1);
            if (SUCCEEDED(hr))
            {
                hr = m_immediateContext.As(&m_immediateContext1);
            }

            DXGI_SWAP_CHAIN_DESC1 sd = {};
            sd.Width = width;
            sd.Height = height;
            sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            sd.SampleDesc.Count = 1;
            sd.SampleDesc.Quality = 0;
            sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            sd.BufferCount = 1;

            hr = dxgiFactory2->CreateSwapChainForHwnd(m_d3dDevice.Get(), hWnd, &sd, nullptr, nullptr, m_swapChain1.GetAddressOf());
            if (SUCCEEDED(hr))
            {
                hr = m_swapChain1.As(&m_swapChain);
            }
        }
        else
        {
            // DirectX 11.0 systems
            DXGI_SWAP_CHAIN_DESC sd = {};
            sd.BufferCount = 1;
            sd.BufferDesc.Width = width;
            sd.BufferDesc.Height = height;
            sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            sd.BufferDesc.RefreshRate.Numerator = 60;
            sd.BufferDesc.RefreshRate.Denominator = 1;
            sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            sd.OutputWindow = hWnd;
            sd.SampleDesc.Count = 1;
            sd.SampleDesc.Quality = 0;
            sd.Windowed = TRUE;

            hr = dxgiFactory->CreateSwapChain(m_d3dDevice.Get(), &sd, m_swapChain.GetAddressOf());
        }

        // Note this tutorial doesn't handle full-screen swapchains so we block the ALT+ENTER shortcut
        dxgiFactory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER);


        if (FAILED(hr))
            return hr;

        // Create a render target view
        ComPtr<ID3D11Texture2D> pBackBuffer;

        hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(pBackBuffer.GetAddressOf()));

        if (FAILED(hr))
            return hr;

        hr = m_d3dDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, m_renderTargetView.GetAddressOf());
        if (FAILED(hr))
            return hr;

        m_immediateContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), nullptr);

        //Create Depth stencil texture
        D3D11_TEXTURE2D_DESC descDepth = {
        .Width = width,
        .Height = height,
        .MipLevels = 1,
        .ArraySize = 1,
        .Format = DXGI_FORMAT_D24_UNORM_S8_UINT,
        .SampleDesc = {.Count = 1,
                       .Quality = 0},
        .Usage = D3D11_USAGE_DEFAULT,
        .BindFlags = D3D11_BIND_DEPTH_STENCIL,
        .CPUAccessFlags = 0,
        .MiscFlags = 0
        };
        hr = m_d3dDevice->CreateTexture2D(&descDepth, nullptr, &m_depthStencil);
        if (FAILED(hr))
            return hr;

        // Create the depth stencil view
        D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
        descDSV.Format = descDepth.Format;
        descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        descDSV.Texture2D.MipSlice = 0;
        hr = m_d3dDevice->CreateDepthStencilView(m_depthStencil.Get(), &descDSV, &m_depthStencilView);
        if (FAILED(hr))
            return hr;

        m_immediateContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());

        // Setup the viewport
        D3D11_VIEWPORT vp;
        vp.Width = (FLOAT)width;
        vp.Height = (FLOAT)height;
        vp.MinDepth = 0.0f;
        vp.MaxDepth = 1.0f;
        vp.TopLeftX = 0;
        vp.TopLeftY = 0;
        m_immediateContext->RSSetViewports(1, &vp);

        //Camera initialize
        m_camera.Initialize(m_d3dDevice.Get());

        //Create a projection matrix
        m_projection = XMMatrixPerspectiveFovLH(XM_PIDIV2, (FLOAT)width / (FLOAT)height, 0.01f, 100.0f);

        //Create the constant buffer
        D3D11_BUFFER_DESC bd = {
            .ByteWidth = sizeof(CBChangeOnResize),
            .Usage = D3D11_USAGE_DEFAULT,
            .BindFlags = D3D11_BIND_CONSTANT_BUFFER,
            .CPUAccessFlags = 0
        };
        hr = m_d3dDevice->CreateBuffer(&bd, nullptr, m_cbChangeOnResize.GetAddressOf());
        if (FAILED(hr))
            return hr;

        //Set CBChangeOnResize constant buffer

        CBChangeOnResize cbChangesOnResize;
        cbChangesOnResize.Projection = XMMatrixTranspose(m_projection);
        m_immediateContext->UpdateSubresource(m_cbChangeOnResize.Get(), 0, nullptr, &cbChangesOnResize, 0, 0);

        //Create Light Constant buffer
        D3D11_BUFFER_DESC bd1 = {
           .ByteWidth = sizeof(CBLights),
           .Usage = D3D11_USAGE_DEFAULT,
           .BindFlags = D3D11_BIND_CONSTANT_BUFFER,
           .CPUAccessFlags = 0
        };

        CBLights cbLights;

        hr = m_d3dDevice->CreateBuffer(&bd1, nullptr, &m_cbLights);
        if (FAILED(hr))
            return hr;


        //Initialize renderables
        for (auto& renderables : m_renderables)
        {
            hr = renderables.second->Initialize(m_d3dDevice.Get(), m_immediateContext.Get());
            if (FAILED(hr))
                return hr;
        }

        //Initialize vertexShaders
        for (auto& vertexShaders : m_vertexShaders)
        {
            hr = vertexShaders.second->Initialize(m_d3dDevice.Get());
            if (FAILED(hr))
                return hr;
        }

        //Initialize pixelShaders
        for (auto& pixelShaders : m_pixelShaders)
        {
            hr = pixelShaders.second->Initialize(m_d3dDevice.Get());
            if (FAILED(hr))
                return hr;
        }
        // Set primitive topology

        //Initialize Scene
        for (auto& it : m_scenes) {
            it.second->Initialize(m_d3dDevice.Get(), m_immediateContext.Get());

        }
        m_immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::AddRenderable

      Summary:  Add a renderable object and initialize the object

      Args:     PCWSTR pszRenderableName
                  Key of the renderable object
                const std::shared_ptr<Renderable>& renderable
                  Unique pointer to the renderable object

      Modifies: [m_renderables].

      Returns:  HRESULT
                  Status code.
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    HRESULT Renderer::AddRenderable(_In_ PCWSTR pszRenderableName, _In_ const std::shared_ptr<Renderable>& renderable) {
        if (m_renderables.count(pszRenderableName))
        {
            return E_FAIL;
        }
        else
        {
            m_renderables.insert(std::make_pair(pszRenderableName, renderable));

            if (m_renderables.empty()) {}
            return S_OK;
        }
    }


    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::AddPointLight
      Summary:  Add a point light
      Args:     size_t index
                  Index of the point light
                const std::shared_ptr<PointLight>& pointLight
                  Shared pointer to the point light object
      Modifies: [m_aPointLights].
      Returns:  HRESULT
                  Status code.
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    HRESULT Renderer::AddPointLight(_In_ size_t index, _In_ const std::shared_ptr<PointLight>& pointLight)
    {
        if (index >= NUM_LIGHTS)
            return E_FAIL;
        m_aPointLights[index] = pointLight;
        return S_OK;
    }


    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::AddVertexShader

      Summary:  Add the vertex shader into the renderer

      Args:     PCWSTR pszVertexShaderName
                  Key of the vertex shader
                const std::shared_ptr<VertexShader>&
                  Vertex shader to add

      Modifies: [m_vertexShaders].

      Returns:  HRESULT
                  Status code
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    HRESULT Renderer::AddVertexShader(_In_ PCWSTR pszVertexShaderName, _In_ const std::shared_ptr<VertexShader>& vertexShader) {
        if (m_vertexShaders.count(pszVertexShaderName))
        {
            return E_FAIL;
        }
        else
        {
            m_vertexShaders.insert(std::make_pair(pszVertexShaderName, vertexShader));
            return S_OK;
        }
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::AddPixelShader

      Summary:  Add the pixel shader into the renderer

      Args:     PCWSTR pszPixelShaderName
                  Key of the pixel shader
                const std::shared_ptr<PixelShader>&
                  Pixel shader to add

      Modifies: [m_pixelShaders].

      Returns:  HRESULT
                  Status code
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    HRESULT Renderer::AddPixelShader(_In_ PCWSTR pszPixelShaderName, _In_ const std::shared_ptr<PixelShader>& pixelShader)
    {
        if (m_pixelShaders.count(pszPixelShaderName))
        {
            return E_FAIL;
        }
        else
        {
            m_pixelShaders.insert(std::make_pair(pszPixelShaderName, pixelShader));
            return S_OK;
        }
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::AddScene

      Summary:  Add a scene

      Args:     PCWSTR pszSceneName
                  Key of a scene
                const std::filesystem::path& sceneFilePath
                  File path to initialize a scene

      Modifies: [m_scenes].

      Returns:  HRESULT
                  Status code
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    HRESULT Renderer::AddScene(_In_ PCWSTR pszSceneName, const std::filesystem::path& sceneFilePath) {
        if (m_scenes.count(pszSceneName))
        {
            return E_FAIL;
        }
        else
        {
            m_scenes.insert(std::make_pair(pszSceneName, std::make_shared<library::Scene>(sceneFilePath)));
            return S_OK;
        }
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::SetMainScene

      Summary:  Set the main scene

      Args:     PCWSTR pszSceneName
                  Name of the scene to set as the main scene

      Modifies: [m_pszMainSceneName].

      Returns:  HRESULT
                  Status code
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    HRESULT Renderer::SetMainScene(_In_ PCWSTR pszSceneName) 
    {
        m_pszMainSceneName = pszSceneName;
        return S_OK;
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
    Method:   Renderer::HandleInput
    Summary:  Add the pixel shader into the renderer and initialize it
    Args:     const DirectionsInput& directions
                  Data structure containing keyboard input data
                const MouseRelativeMovement& mouseRelativeMovement
                Data structure containing mouse relative input data
    Modifies: [m_camera].
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    void Renderer::HandleInput(_In_ const DirectionsInput& directions,_In_ const MouseRelativeMovement& mouseRelativeMovement,_In_ FLOAT deltaTime)
    {
        m_camera.HandleInput(directions, mouseRelativeMovement,deltaTime);
    }



    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::Update

      Summary:  Update the renderables each frame

      Args:     FLOAT deltaTime
                  Time difference of a frame
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    void Renderer::Update(_In_ FLOAT deltaTime) 
    {
        for (auto& renderable : m_renderables)
        {
            renderable.second->Update(deltaTime);
        }
        for (auto& point_light : m_aPointLights)
        {
            point_light.get()->Update(deltaTime);
        }
        m_camera.Update(deltaTime);
    }


    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::Render

      Summary:  Render the frame
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    void Renderer::Render() 
    {
        //Clear the back buffer
        m_immediateContext->ClearRenderTargetView(m_renderTargetView.Get(), Colors::MidnightBlue);

        //Clear the depth buffer to 1.0 (maximum depth)
        m_immediateContext->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

        // Create camera constant buffer and update
        CBChangeOnCameraMovement CBC = 
        {
               .View = XMMatrixTranspose(m_camera.GetView())
        };

        XMStoreFloat4(&CBC.CameraPosition, m_camera.GetEye());

        m_immediateContext->VSSetConstantBuffers(0, 1, m_camera.GetConstantBuffer().GetAddressOf());
        m_immediateContext->PSSetConstantBuffers(0, 1, m_camera.GetConstantBuffer().GetAddressOf());
        m_immediateContext->UpdateSubresource(m_camera.GetConstantBuffer().Get(), 0, nullptr, &CBC, 0, 0);

        //Update the lights constant buffer

        CBLights CBL;
        CBL.LightPositions[0] = m_aPointLights[0].get()->GetPosition();
        CBL.LightPositions[1] = m_aPointLights[1].get()->GetPosition();
        CBL.LightColors[0] = m_aPointLights[0].get()->GetColor();
        CBL.LightColors[1] = m_aPointLights[1].get()->GetColor();

        m_immediateContext->VSSetConstantBuffers(3, 1, m_cbLights.GetAddressOf());
        m_immediateContext->PSSetConstantBuffers(3, 1, m_cbLights.GetAddressOf());

        m_immediateContext->UpdateSubresource(m_cbLights.Get(), 0, nullptr, &CBL, 0, 0);


        for (auto& each : m_renderables)
        {
            std::shared_ptr<Renderable> renderable = each.second;
            //Set vertex buffer
            UINT stride = sizeof(SimpleVertex);
            UINT offset = 0;
            m_immediateContext->IASetVertexBuffers(0, 1, renderable->GetVertexBuffer().GetAddressOf(), &stride, &offset);

            //Set Index buffer
            m_immediateContext->IASetIndexBuffer(renderable->GetIndexBuffer().Get(), DXGI_FORMAT_R16_UINT, 0);

            //Set input layer
            m_immediateContext->IASetInputLayout(renderable->GetVertexLayout().Get());

            //Create and update constant buffer
            CBChangesEveryFrame CBE = {
                .World = XMMatrixTranspose(renderable->GetWorldMatrix()),
                .OutputColor = renderable->GetOutputColor()
            };
            m_immediateContext->UpdateSubresource(renderable->GetConstantBuffer().Get(), 0, NULL, &CBE, 0, 0);

            //Rendering triangles
            m_immediateContext->VSSetShader(renderable->GetVertexShader().Get(), nullptr, 0);


            m_immediateContext->VSSetConstantBuffers(1, 1, m_cbChangeOnResize.GetAddressOf());
            m_immediateContext->VSSetConstantBuffers(2, 1, renderable->GetConstantBuffer().GetAddressOf());

            m_immediateContext->PSSetShader(renderable->GetPixelShader().Get(), nullptr, 0);
            m_immediateContext->PSSetConstantBuffers(2, 1, renderable->GetConstantBuffer().GetAddressOf());


            if (renderable->HasTexture())
            {
                for (UINT i = 0; i < renderable->GetNumMeshes(); i++)
                {
                    m_immediateContext->PSSetShaderResources(0, 1, renderable->GetMaterial(renderable->GetMesh(i).uMaterialIndex).pDiffuse->GetTextureResourceView().GetAddressOf());
                    m_immediateContext->PSSetSamplers(0, 1, renderable->GetMaterial(renderable->GetMesh(i).uMaterialIndex).pDiffuse->GetSamplerState().GetAddressOf());

                    m_immediateContext->DrawIndexed(renderable->GetMesh(i).uNumIndices, renderable->GetMesh(i).uBaseIndex, renderable->GetMesh(i).uBaseVertex);
                }
            }
            else 
            {
                m_immediateContext->DrawIndexed(renderable->GetNumIndices(), 0, 0);
            }

        }

        for (UINT i = 0u; i < m_scenes.find(m_pszMainSceneName)->second->GetVoxels().size(); i++) 
        {
            UINT stride[2] = { sizeof(SimpleVertex),sizeof(InstanceData) };
            UINT offset[2] = { 0,0 };

            CBChangesEveryFrame CBE;
            CBE.World = XMMatrixTranspose(m_scenes.find(m_pszMainSceneName)->second->GetVoxels()[i]->GetWorldMatrix());
            CBE.OutputColor = m_scenes.find(m_pszMainSceneName)->second->GetVoxels()[i]->GetOutputColor();
            m_immediateContext->UpdateSubresource(m_scenes.find(m_pszMainSceneName)->second->GetVoxels()[i]->GetConstantBuffer().Get(),0, nullptr, &CBE, 0, 0);

            ComPtr<ID3D11Buffer> sc[2] =
            {
                m_scenes.find(m_pszMainSceneName)->second->GetVoxels()[i]->GetVertexBuffer(),
                m_scenes.find(m_pszMainSceneName)->second->GetVoxels()[i]->GetInstanceBuffer()
            };

            m_immediateContext->IASetVertexBuffers(0, 2, sc->GetAddressOf(), stride, offset);
            m_immediateContext->IASetIndexBuffer(m_scenes.find(m_pszMainSceneName)->second->GetVoxels()[i]->GetIndexBuffer().Get(),DXGI_FORMAT_R16_UINT, 0);
            m_immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            m_immediateContext->IASetInputLayout(m_scenes.find(m_pszMainSceneName)->second->GetVoxels()[i]->GetVertexLayout().Get());

            m_immediateContext->VSSetShader(m_scenes.find(m_pszMainSceneName)->second->GetVoxels()[i]->GetVertexShader().Get(), nullptr, 0);
            m_immediateContext->VSSetConstantBuffers(0u, 1u, m_camera.GetConstantBuffer().GetAddressOf());
            m_immediateContext->VSSetConstantBuffers(1u, 1u, m_cbChangeOnResize.GetAddressOf());
            m_immediateContext->VSSetConstantBuffers(2u, 1u, m_scenes.find(m_pszMainSceneName)->second->GetVoxels()[i]->GetConstantBuffer().GetAddressOf());
            m_immediateContext->VSSetConstantBuffers(3u, 1, m_cbLights.GetAddressOf());

            m_immediateContext->PSSetShader(m_scenes.find(m_pszMainSceneName)->second->GetVoxels()[i]->GetPixelShader().Get(), nullptr, 0);
            m_immediateContext->PSSetConstantBuffers(0u, 1u, m_camera.GetConstantBuffer().GetAddressOf());
            m_immediateContext->PSSetConstantBuffers(2u, 1u, m_scenes.find(m_pszMainSceneName)->second->GetVoxels()[i]->GetConstantBuffer().GetAddressOf());
            m_immediateContext->PSSetConstantBuffers(3u, 1u, m_cbLights.GetAddressOf());

            m_immediateContext->DrawIndexedInstanced(m_scenes.find(m_pszMainSceneName)->second->GetVoxels()[i]->GetNumIndices()
                ,m_scenes.find(m_pszMainSceneName)->second->GetVoxels()[i]->GetNumInstances()
                ,0u
                ,0
                ,0u
                );
        }
        m_swapChain->Present(0, 0);
    }



    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::SetVertexShaderOfRenderable

      Summary:  Sets the vertex shader for a renderable

      Args:     PCWSTR pszRenderableName
                  Key of the renderable
                PCWSTR pszVertexShaderName
                  Key of the vertex shader

      Modifies: [m_renderables].

      Returns:  HRESULT
                  Status code
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    HRESULT Renderer::SetVertexShaderOfRenderable(_In_ PCWSTR pszRenderableName, _In_ PCWSTR pszVertexShaderName)
    {
        HRESULT hr = S_OK;
        if (!m_renderables.contains(pszRenderableName) || !m_vertexShaders.contains(pszVertexShaderName))
        {
            hr == E_FAIL;
            return hr;
        }
        else
            m_renderables[pszRenderableName]->SetVertexShader(m_vertexShaders[pszVertexShaderName]);

        return hr;
    }


    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::SetPixelShaderOfRenderable

      Summary:  Sets the pixel shader for a renderable

      Args:     PCWSTR pszRenderableName
                  Key of the renderable
                PCWSTR pszPixelShaderName
                  Key of the pixel shader

      Modifies: [m_renderables].

      Returns:  HRESULT
                  Status code
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    HRESULT Renderer::SetPixelShaderOfRenderable(_In_ PCWSTR pszRenderableName, _In_ PCWSTR pszPixelShaderName)
    {

        if (!m_renderables.count(pszRenderableName))
        {
            return E_FAIL;
        }
        else
        {
            if (m_vertexShaders.count(pszPixelShaderName))
            {
                m_renderables.find(pszRenderableName)->second->SetPixelShader(m_pixelShaders.find(pszPixelShaderName)->second);
                return S_OK;
            }
        }
        return E_FAIL;
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::SetVertexShaderOfScene

      Summary:  Sets the vertex shader for the voxels in a scene

      Args:     PCWSTR pszSceneName
                  Key of the scene
                PCWSTR pszVertexShaderName
                  Key of the vertex shader

      Modifies: [m_scenes].

      Returns:  HRESULT
                  Status code
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    HRESULT Renderer::SetVertexShaderOfScene(_In_ PCWSTR pszSceneName, _In_ PCWSTR pszVertexShaderName) 
    {
        if (m_scenes.contains(pszSceneName)) 
        {
            if (m_vertexShaders.contains(pszVertexShaderName)) 
            {
                for (auto& it : m_scenes.find(pszSceneName)->second->GetVoxels()) 
                {
                    it->SetVertexShader(m_vertexShaders.find(pszVertexShaderName)->second);
                }
            }
            return S_OK;
        }
        return E_FAIL;
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::SetPixelShaderOfScene

      Summary:  Sets the pixel shader for the voxels in a scene

      Args:     PCWSTR pszRenderableName
                  Key of the renderable
                PCWSTR pszPixelShaderName
                  Key of the pixel shader

      Modifies: [m_renderables].

      Returns:  HRESULT
                  Status code
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    HRESULT Renderer::SetPixelShaderOfScene(_In_ PCWSTR pszSceneName, _In_ PCWSTR pszPixelShaderName) {
        if (m_scenes.contains(pszSceneName)) 
        {
            if (m_pixelShaders.contains(pszPixelShaderName)) 
            {
                for (auto& it : m_scenes.find(pszSceneName)->second->GetVoxels()) 
                {
                    it->SetPixelShader(m_pixelShaders.find(pszPixelShaderName)->second);
                }
            }
            return S_OK;
        }
        return E_FAIL;
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::GetDriverType

      Summary:  Returns the Direct3D driver type

      Returns:  D3D_DRIVER_TYPE
                  The Direct3D driver type used
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Renderer::GetDriverType definition (remove the comment)
    --------------------------------------------------------------------*/
    D3D_DRIVER_TYPE Renderer::GetDriverType() const {
        return m_driverType;
    }


}