//============================================================
//                                Rigid Body Simulation
//============================================================

#include <cstdio>
#include <cstddef>
#include <cmath>

#include "all.h"

#include <Helpers.h>
#include "physicshelper.h"

#ifndef CANDIDATE
#define CANDIDATE REC991
#endif

namespace
{
static constexpr float simulation_epsilon = 1e-5f;

using f3 = rigidbody::f3;

// The simulations are ordered in increasing difficulty level.
rigidbody::SimulationContext contexts[] =
{
    {1.f,   f3(1.f, 1.f, 1.f),  f3(1.f, 0.f, 0.f),     f3(0.f, 0.5f, 0.f),  60.f},
    {0.5f,  f3(4.f, 4.f, 2.f),  f3(10.f, 5.f, 10.f),   f3(2.f, 2.f, 1.f),   60.f},

    {10.f,  f3(3.f, 4.f, 2.f),  f3(32.f, 40.f, 10.f),  f3(-.75f,2.f,  .5f), 60.f},
    {5.f,   f3(2.f, 5.f, 2.f),  f3(3.f,   4.f, 1.f),   f3(-.4f, 2.5f, 1.f), 60.f},
    {5.f,   f3(1.f, 1.f, 4.f),  f3(20.f, 10.f, 2.f),   f3(0.f,  .5f, -2.f), 60.f},
    {1.f,   f3(4.f, 1.f, .2f),  f3(2.f,   5.f, 3.f),   f3(1.1f, .5f, .01f), 60.f},
    {1.f,   f3(1.f, 5.f, 2.f),  f3(32.f, 40.f, 10.f),  f3(-.3f, 2.5f, .5f), 60.f},
    {.1f,   f3(1.f, 5.f, 2.f),  f3(32.f, 40.f, 10.f),  f3(-.3f, 2.5f, .5f),140.f},
};

rigidbody::f3x3 reference_solutions[] =
{
    {f3(-0.598460f, -0.801153f,  0.000000f), f3( 0.801153f, -0.598460f,  0.000000f), f3( 0.000000f,  0.000000f,  1.000000f)},
    {f3(-0.339649f,  0.006674f, -0.940528f), f3( 0.242838f,  0.966693f, -0.080835f), f3( 0.908663f, -0.255851f, -0.329957f)},
    {f3( 0.729723f, -0.551345f, -0.404379f), f3( 0.421279f,  0.828374f, -0.369215f), f3( 0.538542f,  0.099068f,  0.836754f)},
    {f3( 0.057271f,  0.504104f, -0.861742f), f3( 0.787798f, -0.553035f, -0.271159f), f3(-0.613266f, -0.663349f, -0.428805f)},
    {f3( 0.903508f, -0.299942f,  0.306117f), f3(-0.427578f, -0.679460f,  0.596248f), f3( 0.029155f, -0.669604f, -0.742146f)},
    {f3( 0.946214f,  0.177947f, -0.270211f), f3(-0.267789f, -0.037955f, -0.962730f), f3(-0.181571f,  0.983308f,  0.011738f)},
    {f3(-0.857230f,  0.341992f,  0.384966f), f3(-0.449442f, -0.861786f, -0.235218f), f3( 0.251316f, -0.374655f,  0.892454f)},
    {f3( 0.971540f, -0.020698f, -0.235971f), f3( 0.076390f, -0.915576f,  0.394823f), f3(-0.224222f, -0.401612f, -0.887937f)}
};

rigidbody::f frobenius_norm(rigidbody::f3x3 const& A)
{
    auto B = A * A.transpose();
    return B.trace();
};

template<typename T, size_t N>
constexpr size_t array_size(const T(&)[N])
{
    return N;
}

} // namespace anonymous

extern "C" int _tmain(int /* argc */, TCHAR** /* argv */)
{
    using namespace rigidbody;

    try
    {
        CANDIDATE::GlobalInit();

        const size_t arraySize = array_size(contexts);
        for (size_t i = 5 ; i < arraySize; ++i)
        {
            f3x3 const result = CANDIDATE::Simulate(contexts[i]);
            f diff = frobenius_norm(result - reference_solutions[i]);
            if (std::abs(diff) < simulation_epsilon)
            {
                std::printf("OK:      Simulation %zd: Difference with reference %f\n", i, diff);
                std::cout << "Output is : \n" << result << "\nshould have been : \n" << reference_solutions[i] << "\n";
            }
            else
            {
                std::printf("TOO FAR: Simulation %zd: Difference with reference %f\n", i, diff);
                std::cout << "disired quat value : " << matrixToQuaternion(reference_solutions[i]) << "\n";
                std::cout << "Output is : \n" << result << "\nshould have been : \n" << reference_solutions[i] << "\n";
            }
            
#ifdef HAVE_CHECK
            // plug more tests here ?
#endif
        }
        
        CANDIDATE::GlobalTeardown();
    }
    catch(const std::exception& e)
    {
        fprintf(stderr, "An std::exception was thrown: %s\n", e.what());
        return EXIT_FAILURE;
    }
    catch (...)
    {
        fprintf(stderr, "An exception was thrown\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
