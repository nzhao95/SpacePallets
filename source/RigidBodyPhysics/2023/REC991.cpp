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
static f time_step = 0.0005;

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

    int required_steps = static_cast<int>(floor(final_time / time_step));

    std::cout << "Time step : " << time_step << "\n";

    for (int step = 0; step < required_steps; ++step)
    {
        orientation.applyRotationStep(eulerMotionVector, frame_angular_velocity, time_step);

        if (step % 100 == 0)
        {
            orientation.normalize();
        }

        if (shouldDraw)
        {
            global_angular_velocity = orientation.rotate(frame_angular_velocity);
            velocities.push_back(global_angular_velocity * 10.0);
            draw::display(context, quaternionToMatrix(orientation.normalized()), velocities);
        }
    }

    orientation.applyRotationStep(eulerMotionVector, frame_angular_velocity, final_time - f(required_steps * time_step));

    return quaternionToMatrix(orientation.normalized());
}

} // namespace REC991
