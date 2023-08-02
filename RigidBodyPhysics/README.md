# Rigid body physics

In 2378, several astronauts are conducting experiments in an
asteroid belt. Each time their ship encounters an asteroid, they
shoot a pellet at it and observe how it moves.

We are only interested in the orientation of the asteroid. To
further simplify the problem, we consider an asteroid to be a
rigid rectangular parallelepiped of uniform mass density.

Also note that this happens in a void, so no force is applied to the
asteroid.

The local frame of the parallelepiped is aligned with its principal
axes. The origin of the local frame is placed at the center of mass.
Initially, at t = 0-, the asteroid is at rest and its orientation
coincides with the world frame, i.e., it is the identity matrix at
t = 0-.

At t = 0+, the pellet impacts the asteroid. We model this as if an
impulse was applied at a specific point.

You will have to write the Simulate function, which takes a context
containing this impulse and the impact point. All physical quantities are
expressed using the International System of Units.

We ask you to compute the orientation of an asteroid at the final
time, also provided in the context.

We provide a basic matrix library. This library has not been
designed for speed, nor completeness, so feel free to modify or add
your own code if necessary.

Don't hesitate to use a graphics library to visually check your
solutions (raylib, etc.). However, you should not use external libraries
in your simulation.

The Simulate function shouldn't be too slow. It should take less
than 1 second on a recent computer.
