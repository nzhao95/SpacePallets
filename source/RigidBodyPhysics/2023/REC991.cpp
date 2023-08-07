#include <Helpers.h>
#include "REC991.h"
#include <cmath>
#include "draw.h"

namespace REC991
{
using namespace rigidbody;
static constexpr f time_step = 1e-3f;
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
	f3 angular_velocity = context.ComputeInitialAngularVelocity(invI);

    //f3x3 orientation{ f3x3::id() };
    quat orientation;

	f3 axis = f3(angular_velocity.x, angular_velocity.y, angular_velocity.z);
    f angle = axis.norm();
    f3 direction = axis.normalized();

    quat rotQuat = angleAxisToQuaternion(angle * time_step, direction);


    f current_time = 0.f;

    while (current_time < final_time)
    {
        angular_velocity = context.UpdateAngularVelocity(I, invI, angular_velocity, time_step);

        //const f3 axis = f3(angular_velocity.x, angular_velocity.y, angular_velocity.z);
        //const f angle_mag = axis.norm();
        //const f3 axisNormed = axis.normalized();
        //const f3x3 W = velocityTensor(angular_velocity);

        orientation.applyRotationStep(angular_velocity, time_step);
		f prevAngle = angle;
        quaternionToAxisAngle(orientation, angle, direction);

		//std::cout << angle - prevAngle << " vs. " << axis.norm() * time_step << "\n"; 
        if (shouldDraw)
        {
            draw::display(context, angle, direction);
        }

        current_time += time_step;
    }

    //return orientation;
    return quaternionToMatrix(orientation.normalized());
    //return matrixFromAxisAngle(angle_mag * final_time, axisNormed);
}

} // namespace REC991
