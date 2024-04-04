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
To help keep springs from being unreasonably deformed during each time step, we implemented an additional feature based on the SIGGRAPH 1995 Provot paper on deformation constraints in mass-spring models. For each spring, we applied this constraint to prevent springs from extending more than 10% greater than its `rest_length` at the end of any time step.

If the spring's length doesn't extend more than 10% greater than its `rest_length`, we perform the following conditions to the point masses:
- Both point masses are pinned: Do nothing
- One point mass is pinned: Correction entirely by the other point mass
- Neither point masses are pinned: Correct by half to each point mass

Below are images of running `./clothsim -f ../scene/pinned2.json` with the default parameters of `ks = 5000 N/m, density = 15 g/cm^2, damping = 0.2%`) to help showcase how experimenting with parameters causes the cloth to change.

[insert the two pics]

### Changing spring constant (ks)
When maintaining the default parameters (`density`, `damping`) and only modifying the spring constant, `ks`, we can see that as `ks` increases, the cloth becomes less malleable. This is because increasing the spring constant results in increasing the spring's stiffness, causing the cloth to appear less "creased" and more rigid/flat. At lower `ks` values, the strength of the springs is lower, hence why the cloth is more flexible and has more folds/creases. 

### Changing density
When maintaining the default parameters (`ks`, `damping`) and only modifying the `density`, we can see that its effect on the cloth is almost inversely to `ks` — increasing the `density` has similar results to the cloth as decreasing the `ks`. At lower `density` values, the cloth has lower mass. Using Newton's Second Law (`F = ma`), this means that the forces accumulated at each point mass will be less, causing less forces to pull the cloth down and ultimately resulting in less creases/folds in the cloth (more flat). At higher `density` values, the forces accumulated at each point mass are greater which is why there are more creases/folds in the cloth. 

### Changing damping
When maintaining the default parameters (`ks`, `density`) and only modifying `damping`, we can immediately see that modifying `damping` impacts the speed at which the simulation/cloth reaches a state of rest. When `damping` is increased, this causes the cloth to stop its motion at a faster speed compared to a lower `damping` value. A higher `damping` causes the cloth to swing back and forth more rapidly. `damping` impacts the velocity value in Verlet integration so when `damping` is a lower value, there is no loss of energy, hence why the cloth rapidly swings. When `damping` is increased, the forces acting on the cloth are "dampened" and there is a loss of energy, hence why the cloth swings less rapidly. This is most evidently seen when comparing `damping = 1 g/cm^2` and `damping = 50000 g/cm^2`.

Below are images of running `./clothsim -f ../scene/pinned4.json` with the default parameters in the final resting state (wireframe and normal appearances).

[insert pics]

# Part 3


# Part 4
### Changing spring constant (ks)

### Changing density


# Part 5

