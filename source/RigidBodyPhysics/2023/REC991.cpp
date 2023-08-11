#include <Helpers.h>
#include "REC991.h"
#include <cmath>
#include "draw.h"
#include <vector>

namespace REC991
{
using namespace rigidbody;
static constexpr f time_step = 0.001;
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
	f3 angular_velocity = context.ComputeInitialAngularVelocity(invI);

    std::vector<f3> velocities;
    velocities.push_back(angular_velocity);

    const f H = (cross(context.initial_impulse_application_point, context.initial_impulse)).norm();

    //f3 orientation;
    //f3x3 orientation{ f3x3::id() };
    quat orientation;

	f3 axis = f3(angular_velocity.x, angular_velocity.y, angular_velocity.z);
    f angle = axis.norm();
    f3 direction = axis.normalized();

    quat rotQuat = angleAxisToQuaternion(angle * time_step, direction);

    f current_time = 0.0;
    int iter = 0;

    while (current_time < final_time)
    {
        //const f3 axis = f3(angular_velocity.x, angular_velocity.y, angular_velocity.z);
        //const f angle_mag = axis.norm();
        //const f3 axisNormed = axis.normalized();
        //const f3x3 W = velocityTensor(angular_velocity);

        orientation.applyRotationStep(angular_velocity, time_step);
	    ////f prevAngle = angle;
        
        //f3x3 mat = eulerAngleToMatrix(orientation);
        //quaternionToAxisAngle(orientation, angle, direction);
        angular_velocity = context.UpdateAngularVelocity(I, angular_velocity, time_step);
        velocities.push_back(angular_velocity * 10.0);

        //if (iter % 1000 == 0)
        //    std::cout << energy / H << " energy is" << H << " and velocity is " << angular_velocity << "\n";

	    ////std::cout << angle - prevAngle << " vs. " << axis.norm() * time_step << "\n"; 
        if (shouldDraw)
        {
            draw::display(context, quaternionToMatrix(orientation.normalized()), velocities);
        }

        current_time += time_step;
        iter += 1;
    }

    f energy = (I * angular_velocity).norm();
    std::cout << "energy loss : " << 1.0 - energy / H << "\n";
    //orientation.applyRotationStep(angular_velocity, final_time);
    //return eulerAngleToMatrix(orientation);
    std::cout << "final quat value : " << orientation <<"\n";
    return quaternionToMatrix(orientation.normalized());
    //return matrixFromAxisAngle(angle_mag * final_time, axisNormed);
}

} // namespace REC991
