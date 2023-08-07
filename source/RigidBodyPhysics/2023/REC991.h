#pragma once

#include "physicshelper.h"

namespace REC991
{
void GlobalInit();
void GlobalTeardown();

rigidbody::f3x3 Simulate(rigidbody::SimulationContext const& context, const rigidbody::f3& answerAxis);

} // namespace REC991
