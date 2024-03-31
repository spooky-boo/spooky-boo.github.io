Website: https://cal-cs184-student.github.io/hw-webpages-sp24-christyquang/hw4/index.html

# Part 1
The goal of this section was to create a grid of masses and springs. First, we iterated through `num_height_points` and `num_width_points` via an inner loop to generate the point masses in row-major order, alongside the masses being evenly spaced. Based on the orientation (horizontal or vertical), we varied across the `xy` plane or the `xz` plane. If the point mass's `(x, y)` index is within the cloth's `pinned` vector, we set the point mass's `pinned` boolean to `true`. The `pinned` boolean indicates whether the point mass will remain stationary/pinned throughout the simulation.

After creating the grid of point masses, we needed to create springs to apply the structural, shear, and bending constraints between point masses. These are the restraints indicating what constraints would be applied:
- Structural: A point mass and the point mass to its left + the point mass above it.
- Shearing: A point mass and the point mass to its diagonal upper left + the point mass to its diagonal upper right.
- Bending: A point mass and the point mass two away to its left + the point mass two above it.

Below, we can see images of the grid with the different constraints applied:
[Include the 3 pics]

# Part 2
## Task 1: Compute total force acting on each point mass
In this task, we first calculated the external forces done by looping through the `external_accelerations`. All accelerations are applied uniformly so we summed all of the `external_forces` (a `Vector3D`) using Newton's 2nd Law (`F = ma`). Afterwards, we looped through all of the point masses and set each `forces` to `external_force`.

Next, we needed to apply the spring correction forces using Hooke's Law, `F_s = k_s * (||p_a - p_b|| - l)`. For each spring, we apply `F_s` force to the point mass on one end and then an equal, but negated force on the other end. Bending constraints should be weaker than the other two constraints which is why we multiplied `k_s * 0.2`.

## Task 2: Use Verlet integration to compute new point mass positions
In this task, we compute a point mass's new position by using Verlet integration on all unpinned point masses. The new position is calculated by `x_(t+dt) = x_t + (1 - d) * (x_t - x_(t-dt)) + a_t * dt^2`. We also needed to update the point mass's `last_position` to the previous position prior to using Verlet integration.

## Task 3: Constrain position updates

### Changing spring constant (ks)

### Changing density

### Changing damping

# Part 3


# Part 4


# Part 5

