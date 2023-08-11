#include <Helpers.h>
#include "REC991.h"
#include <cmath>
#include "draw.h"
#include <vector>

namespace REC991
{
using namespace rigidbody;
static constexpr f time_step = 0.0001;
static bool shouldDraw = false;

void GlobalInit()
{
    std::cout << "Do you want to display the simulations ? " << "\n" << "0 : No" << "\n" << "1 : Yes" << "\n";
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
    const f3& invIDiag = f3(invI[0][0], invI[1][1], invI[2][2]);
	f3 frame_angular_velocity = context.ComputeInitialAngularVelocity(invI);
    f3 global_angular_velocity = frame_angular_velocity;

    std::vector<f3> velocities;
    velocities.push_back(global_angular_velocity);

    const f H = (cross(context.initial_impulse_application_point, context.initial_impulse)).norm();

    quat orientation;

    f current_time = 0.0;
    int iter = 0;

    f dynamic_time_step = pow(10, -(int)std::log10(context.initial_impulse.norm() / context.density * context.final_time) - 2);
    std::cout << "time step : " << dynamic_time_step << "\n";

    while (current_time < final_time)
    {
        orientation.applyRotationStep(global_angular_velocity, dynamic_time_step);

        frame_angular_velocity = context.UpdateAngularVelocity(I, frame_angular_velocity, dynamic_time_step);
        global_angular_velocity = orientation.rotate(frame_angular_velocity);
        //std::cout << newI << ", " << angular_velocity<<   "\n";
        velocities.push_back(global_angular_velocity * 10.0);

        //f energy = (I * frame_angular_velocity).norm();
        //if (iter % 1000 == 0)
        //    std::cout << energy / H << " energy is" << H << " and velocity is " << angular_velocity << "\n";

	    ////std::cout << angle - prevAngle << " vs. " << axis.norm() * time_step << "\n"; 
        if (shouldDraw)
        {
            draw::display(context, quaternionToMatrix(orientation.normalized()), velocities);
        }

        current_time += dynamic_time_step;
        iter += 1;
    }

    std::cout << "final quat value : " << orientation <<"\n";
    return quaternionToMatrix(orientation.normalized());
}

} // namespace REC991
