#pragma once
#include "Cube/MyCube.h"

void MyCube::Update(_In_ FLOAT deltaTime)
{
	m_world = m_world * XMMatrixRotationY(deltaTime);
}
