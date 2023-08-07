#pragma once

#include "physicshelper.h"

namespace draw
{
using namespace rigidbody;

void reshape(int width, int height);

void Init();

void End();

void drawCube(const f angle, const f3& axis, rigidbody::SimulationContext const& context);

void drawLine(const f3& direction, const f3& origin = f3(0.f, 0.f, 0.f));

void display(const rigidbody::SimulationContext& context, const f angle, const f3& axis);
}