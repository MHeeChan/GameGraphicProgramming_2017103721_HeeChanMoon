#include "Renderer/InstancedRenderable.h"

namespace library
{
    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   InstancedRenderable::InstancedRenderable

      Summary:  Constructor

      Args:     const XMFLOAT4& outputColor
                  Default color of the renderable
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    InstancedRenderable::InstancedRenderable(_In_ const XMFLOAT4& outputColor)
        :Renderable(outputColor), m_padding()
    {}
    


    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   InstancedRenderable::InstancedRenderable

      Summary:  Constructor

      Args:     std::vector<InstanceData>&& aInstanceData
                  An instance data
                const XMFLOAT4& outputColor
                  Default color of the renderable

      Modifies: [m_instanceBuffer, m_aInstanceData].
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    InstancedRenderable::InstancedRenderable
    (_In_ std::vector<InstanceData>&& aInstanceData, _In_ const XMFLOAT4& outputColor)
        : Renderable(outputColor),
        m_padding(), 
        m_instanceBuffer(nullptr),
        m_aInstanceData(std::move(aInstanceData)) {}

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   InstancedRenderable::SetInstanceData

      Summary:  Sets the instance data

      Args:     std::vector<InstanceData>&& aInstanceData
                  Instance data

      Modifies: [m_aInstanceData].
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    void InstancedRenderable::SetInstanceData(_In_ std::vector<InstanceData>&& aInstanceData) 
    {
        m_aInstanceData = std::move(aInstanceData);
    }


    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   InstancedRenderable::GetInstanceBuffer

      Summary:  Returns the instance buffer

      Returns:  ComPtr<ID3D11Buffer>&
                  Instance buffer
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    ComPtr<ID3D11Buffer>& InstancedRenderable::GetInstanceBuffer() 
    {
        return m_instanceBuffer;
    }


    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   InstancedRenderable::GetNumInstances

      Summary:  Returns the number of instances

      Returns:  UINT
                  Number of instances
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    UINT InstancedRenderable::GetNumInstances() const 
    {
        return m_aInstanceData.size();
    }


    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   InstancedRenderable::initializeInstance

      Summary:  Creates an instance buffer

      Args:     ID3D11Device* pDevice
                  Pointer to a Direct3D 11 device

      Modifies: [m_instanceBuffer].

      Returns:  HRESULT
                  Status code
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    HRESULT InstancedRenderable::initializeInstance(_In_ ID3D11Device* pDevice) 
    {
        D3D11_BUFFER_DESC bd;
        bd.ByteWidth = m_aInstanceData.size() * sizeof(InstanceData);
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bd.CPUAccessFlags = 0;
        bd.MiscFlags = 0;
        bd.StructureByteStride = 0;

        D3D11_SUBRESOURCE_DATA initData = 
        {
            .pSysMem = &m_aInstanceData[0],
            .SysMemPitch = 0,
            .SysMemSlicePitch = 0
        };

        HRESULT hr = pDevice->CreateBuffer(&bd,&initData,m_instanceBuffer.GetAddressOf());

        if (FAILED(hr))
        {
            return hr;
        }

    }

}
