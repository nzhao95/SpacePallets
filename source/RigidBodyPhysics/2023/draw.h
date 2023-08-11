#pragma once

#include "physicshelper.h"
#include <vector>

namespace draw
{
using namespace rigidbody;

void reshape(int width, int height);

void Init();

void End();

void drawCube(const SimulationContext& context, const f3x3& rotMat);

void drawLine(const f3& direction, const f3& origin = f3());

void display(const SimulationContext& context, const f3x3& rotMat, const std::vector<f3>& angularVelocities);
}