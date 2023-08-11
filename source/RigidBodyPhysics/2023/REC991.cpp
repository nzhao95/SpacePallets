#include <Helpers.h>
#include "REC991.h"
#include <cmath>
#include "draw.h"
#include <vector>

#include <chrono>

namespace REC991
{
using namespace rigidbody;
static bool shouldDraw = false;

void GlobalInit()
{
    std::cout << "Do you want to display the simulations ? (Time step will be fixed and there will be computation errors in visualization mode)" << "\n" << "0 : No" << "\n" << "1 : Yes" << "\n";
    std::cin >> shouldDraw;
    if (shouldDraw)
        draw::Init();
}

void GlobalTeardown()
{
    if (shouldDraw)
        draw::End();
}

rigidbody::f3x3 Simulate(rigidbody::SimulationContext const& context)
{
    using namespace rigidbody;

    const f final_time = context.final_time;
	const f3x3& I = context.ComputeInertiaTensor();
	const f3x3& invI = context.ComputeInvInertiaTensor();
    const f3& eulerMotionVector{ (I[1][1] - I[2][2]) / I[0][0],
                                 (I[2][2] - I[0][0]) / I[1][1],
                                 (I[0][0] - I[1][1]) / I[2][2] };

	f3 frame_angular_velocity = context.ComputeInitialAngularVelocity(invI);
    f3 global_angular_velocity = frame_angular_velocity;

    std::vector<f3> velocities;
    velocities.push_back(global_angular_velocity);

    quat orientation;

    f current_time = 0.0;

    // Very arbitrary stuff based on the input given.
    const f dynamic_time_step = shouldDraw ? 0.01 : 5*pow(10, -std::max((int)std::log10(context.initial_impulse.norm() / context.density * context.final_time), 3) - 1);
    std::cout << "Time step : " << dynamic_time_step << "\n";

    while (current_time < final_time)
    {
        orientation.applyRotationStep(global_angular_velocity, dynamic_time_step);

        frame_angular_velocity = context.UpdateAngularVelocity(eulerMotionVector, frame_angular_velocity, dynamic_time_step);

        global_angular_velocity = orientation.rotate(frame_angular_velocity);

        if (shouldDraw)
        {
            velocities.push_back(global_angular_velocity * 10.0);
            draw::display(context, quaternionToMatrix(orientation.normalized()), velocities);
        }

        current_time += dynamic_time_step;
    }

    return quaternionToMatrix(orientation.normalized());
}

} // namespace REC991
