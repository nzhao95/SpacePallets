#pragma once

#include "physicshelper.h"

namespace REC991
{
void GlobalInit();
void GlobalTeardown();

rigidbody::f3x3 Simulate(rigidbody::SimulationContext const& context);

} // namespace REC991