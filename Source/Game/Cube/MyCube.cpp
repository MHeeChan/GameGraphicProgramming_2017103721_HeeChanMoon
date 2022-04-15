#include "Cube/MyCube.h"

/*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
  Method:   Cube::Cube

  Summary:  Constructor

  Args:     const std::filesystem::path& textureFilePath
              Path to the texture to use
M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
/*--------------------------------------------------------------------
  TODO: Cube::Cube definition (remove the comment)
--------------------------------------------------------------------*/
MyCube::MyCube(const std::filesystem::path& textureFilePath) :BaseCube(textureFilePath)
{}

/*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
  Method:   Cube::Update

  Summary:  Updates the cube every frame

  Args:     FLOAT deltaTime
              Elapsed time

  Modifies: [m_world].
M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
void MyCube::Update(_In_ FLOAT deltaTime)
{
    static FLOAT s_totalTime = 0.0f;
    s_totalTime += deltaTime;
    m_world = XMMatrixTranslation(5.0f, XMScalarSin(s_totalTime), 0.0f) * XMMatrixRotationY(s_totalTime);
    /// 시간 나면 움직임만 바꿔보기
}