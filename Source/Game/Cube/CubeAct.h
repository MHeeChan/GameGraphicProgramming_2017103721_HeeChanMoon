#pragma once
#include "Cube/BaseCube.h"

class CubeAct : public BaseCube
{
public:
    virtual void Update(_In_ FLOAT deltaTime);
    CubeAct();
private:

    XMMATRIX mSpin;
    XMMATRIX mOrbit;
    XMMATRIX mTranslate;
    XMMATRIX mScale;
  
    void SetSpin(_In_ FLOAT deltaTime)
    {
        mSpin *= XMMatrixRotationZ(deltaTime);
    }
    
    void SetOrbit(_In_ FLOAT deltaTime)
    {
        mOrbit *= XMMatrixRotationY(-deltaTime * 2.0f);
    }


};


