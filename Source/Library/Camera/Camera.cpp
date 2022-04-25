#include "Camera/Camera.h"

namespace library
{
    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Camera::Camera

      Summary:  Constructor

      Modifies: [m_yaw, m_pitch, m_moveLeftRight, m_moveBackForward,
                 m_moveUpDown, m_travelSpeed, m_rotationSpeed, 
                 m_padding, m_cameraForward, m_cameraRight, m_cameraUp, 
                 m_eye, m_at, m_up, m_rotation, m_view].
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Camera::Camera definition (remove the comment)
    --------------------------------------------------------------------*/
    Camera::Camera(_In_ const XMVECTOR& position)
        : m_yaw(0.0f)
        , m_pitch(0.0f)
        , m_moveLeftRight(0.0f)
        , m_moveBackForward(0.0f)
        , m_moveUpDown(0.0f)
        , m_travelSpeed(10.0f)
        , m_rotationSpeed(10.0f)
        , m_padding()
        , m_cameraForward(DEFAULT_FORWARD)
        , m_cameraRight(DEFAULT_RIGHT)
        , m_cameraUp(DEFAULT_UP)
        , m_eye(position)
        , m_at()
        , m_up()
        , m_rotation()
        , m_view()
    {};
        

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Camera::GetEye

      Summary:  Returns the eye vector

      Returns:  const XMVECTOR&
                  The eye vector
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Camera::GetEye definition (remove the comment)
    --------------------------------------------------------------------*/
    const XMVECTOR& Camera::GetEye() const
    {
        return m_eye;
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Camera::GetAt

      Summary:  Returns the at vector

      Returns:  const XMVECTOR&
                  The at vector
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Camera::GetAt definition (remove the comment)
    --------------------------------------------------------------------*/
    const XMVECTOR& Camera::GetAt() const
    {
        return m_at;
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Camera::GetUp

      Summary:  Returns the up vector

      Returns:  const XMVECTOR&
                  The up vector
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Camera::GetUp definition (remove the comment)
    --------------------------------------------------------------------*/
    const XMVECTOR& Camera::GetUp() const
    {
        return m_up;
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Camera::GetView

      Summary:  Returns the view matrix

      Returns:  const XMMATRIX&
                  The view matrix
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Camera::GetView definition (remove the comment)
    --------------------------------------------------------------------*/
    const XMMATRIX& Camera::GetView() const
    {
        return m_view;
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Camera::HandleInput

      Summary:  Sets the camera state according to the given input

      Args:     const DirectionsInput& directions 
                  Keyboard directional input
                const MouseRelativeMovement& mouseRelativeMovement
                  Mouse relative movement input
                FLOAT deltaTime
                  Time difference of a frame

      Modifies: [m_yaw, m_pitch, m_moveLeftRight, m_moveBackForward,
                 m_moveUpDown].
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Camera::HandleInput definition (remove the comment)
    --------------------------------------------------------------------*/
    void Camera::HandleInput(_In_ const DirectionsInput& directions, _In_ const MouseRelativeMovement& mouseRelativeMovement, _In_ FLOAT deltaTime)
    {
        /*WCHAR szDebugMessage[64];
        swprintf_s(szDebugMessage, L"uMsg: %n\n", directions.bRight);
        OutputDebugString(szDebugMessage);*/

        float dir_x = 0;
        float dir_y = 0;
        float dir_z = 0;

        if (directions.bRight)
            dir_x = 1.0f;
        else if (directions.bLeft)
            dir_x = -1.0f;
        else if (directions.bLeft && directions.bRight)
            dir_x = 0.0f;

        if (directions.bUp)
            dir_z = 1.0f;
        else if (directions.bDown)
            dir_z = -1.0f;
        else if (directions.bUp && directions.bDown)
            dir_z = 0.0f;

        if (directions.bFront)
            dir_y = 1.0f;
        else if (directions.bBack)
            dir_y = -1.0f;
        else if (directions.bFront && directions.bBack)
            dir_y = 0.0f;

        if ((dir_x != 0) || (dir_y != 0) || (dir_z != 0))
        {
            XMVECTOR DIR = XMVectorSet(dir_x, dir_y, dir_z, 0.0f);
            DIR = XMVector3Normalize(DIR);
            DIR *= m_travelSpeed * deltaTime;
            XMFLOAT3 dir_move;
            XMStoreFloat3(&dir_move, DIR);
            m_moveLeftRight += dir_move.x;
            m_moveBackForward += dir_move.y;
            m_moveUpDown += dir_move.z;
        }

        float rot_x = 0.0f;
        float rot_y = 0.0f;

        rot_x = mouseRelativeMovement.X;
        rot_y = mouseRelativeMovement.Y;

        if ((rot_x != 0) || (rot_y != 0))
        {
            XMVECTOR ROT = XMVectorSet(rot_x, rot_y, 0.0f, 0.0f);
            ROT = XMVector2Normalize(ROT);
            ROT *= m_rotationSpeed * deltaTime;
            XMFLOAT2 rot_move;
            XMStoreFloat2(&rot_move, ROT);
            m_yaw += rot_move.x;
            if (m_pitch < -XM_PIDIV2)
                m_pitch = -XM_PIDIV2;
            else if (XM_PIDIV2 < m_pitch)
                m_pitch = XM_PIDIV2;
            else
                m_pitch += rot_move.y;

        }

    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Camera::Update

      Summary:  Updates the camera based on its state

      Args:     FLOAT deltaTime
                  Time difference of a frame

      Modifies: [m_rotation, m_at, m_cameraRight, m_cameraUp, 
                 m_cameraForward, m_eye, m_moveLeftRight, 
                 m_moveBackForward, m_moveUpDown, m_up, m_view].
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Camera::Update definition (remove the comment)
    --------------------------------------------------------------------*/
    void Camera::Update(_In_ FLOAT deltaTime)
    {
        m_rotation = XMMatrixRotationRollPitchYaw(m_pitch, m_yaw, 0);
        m_at = XMVector3TransformCoord(DEFAULT_FORWARD, m_rotation);
        m_at = XMVector3Normalize(m_at);

        XMMATRIX RotateYTempMatrix;
        RotateYTempMatrix = XMMatrixRotationY(m_yaw);

        m_cameraRight = XMVector3TransformCoord(DEFAULT_RIGHT, RotateYTempMatrix);
        m_cameraUp = XMVector3TransformCoord(DEFAULT_UP, RotateYTempMatrix);
        m_cameraForward = XMVector3TransformCoord(DEFAULT_FORWARD, RotateYTempMatrix);

        m_eye += m_moveLeftRight * m_cameraRight;
        m_eye += m_moveBackForward * m_cameraForward;
        m_eye += m_moveUpDown * m_cameraUp;

        //m_up = 
        m_at = m_eye + XMVector3TransformCoord(DEFAULT_FORWARD, m_rotation);
        m_up = XMVector3TransformCoord(DEFAULT_UP, m_rotation);

        m_moveLeftRight = 0.0f;
        m_moveBackForward = 0.0f;
        m_moveUpDown = 0.0f;

        m_view = XMMatrixLookAtLH(m_eye, m_at, m_up);

    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M++M+++M+++M+++M+++M+++M+++M
  Method:   Camera::Initialize+

  Summary:  Initialize the view matrix constant buffers

  Args:     ID3D11Device* pDevice
              Pointer to a Direct3D 11 device

  Modifies: [m_cbChangeOnCameraMovement].
M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    HRESULT Camera::Initialize(_In_ ID3D11Device* device)
    {

        // Create the constant buffer
        D3D11_BUFFER_DESC bd = {};
        HRESULT hr = S_OK;
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = sizeof(CBChangeOnCameraMovement);
        bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bd.CPUAccessFlags = 0;
        hr = device->CreateBuffer(&bd, nullptr, m_cbChangeOnCameraMovement.GetAddressOf()); // &m_constantBuffer
        if (FAILED(hr))
            return hr;

        return hr;
    }

    ComPtr<ID3D11Buffer>& Camera::GetConstantBuffer()
    {
        return m_cbChangeOnCameraMovement;
    }

}