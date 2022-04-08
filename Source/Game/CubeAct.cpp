#pragma once
#include "Cube/CubeAct.h"

CubeAct::CubeAct()
{
	value = XMMatrixIdentity();
	mSpin = XMMatrixIdentity();
	mOrbit = XMMatrixIdentity();
	mTranslate = XMMatrixIdentity();
	mScale = XMMatrixIdentity();
}

void CubeAct::Update(_In_ FLOAT deltaTime)
{
	mTranslate = XMMatrixTranslation(-4.0f, 0.0f, 0.0f);
	mScale = XMMatrixScaling(0.3f, 0.3f, 0.3f);
	
	m_world = mScale * mSpin * mTranslate  * mOrbit;

	SetSpin(deltaTime);
	SetOrbit(deltaTime);
	//
}






