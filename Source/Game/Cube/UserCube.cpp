#pragma once
#include "Cube/UserCube.h"

UserCube::UserCube()
{
	value = XMMatrixIdentity();
	mSpin = XMMatrixIdentity();
	mOrbit = XMMatrixIdentity();
	mTranslate = XMMatrixIdentity();
	mScale = XMMatrixIdentity();
}

void UserCube::Update(_In_ FLOAT deltaTime)
{
	mTranslate = XMMatrixTranslation(-4.0f, 0.0f, 0.0f);
	mScale = XMMatrixScaling(1.3f, 1.3f, 1.3f);

	m_world = mScale * mSpin *  mTranslate * mOrbit;

	SetSpin(deltaTime);
	SetOrbit(deltaTime);
	//
}






