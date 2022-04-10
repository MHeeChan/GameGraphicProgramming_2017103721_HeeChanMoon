#pragma once
#include "Cube/BaseCube.h"

class UserCube : public BaseCube
{
public:
    virtual void Update(_In_ FLOAT deltaTime);
    UserCube();
private:
    XMMATRIX value;
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
        
        if((-deltaTime/2.0f)* (-deltaTime / 2.0f) > 1 )
            mOrbit *= XMMatrixRotationY(-deltaTime * 2.0f);
        else
            mOrbit *= XMMatrixRotationY(deltaTime * 2.0f);
    }


};


