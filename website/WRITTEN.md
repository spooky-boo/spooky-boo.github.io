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
## Task 1: Handling collisions with spheres
In this task, we handle collisions with spheres by registering when a collision "occurs" (depends on sphere vs. plane) and then calculate a correction to the point mass's current position so that it is just outside the surface of the object being collided into. A collision occurs when the point mass has a distance to the center of the sphere less than its radius. If a collision occurs, we calculate the tangent point along the sphere where the collision would have occured and use that value to calculate the correction vector. When updating the point mass's position, we make sure to scale by `1 - friction` when multiplying by the correction vector. We also needed to update `Cloth::simulate` to check for collisions between every `PointMass` and `CollisionObject`.

### Changing spring constant (ks)
We can see through the images below that when we increase `ks`, this results in increasing the stiffness of the spring. The cloth becomes more rigid and there is more structure to the folds. The material also goes from looking like velvet/silk to a stiffer material. This makes sense because a stiff spring will resist the deformation from gravity. In contrast, at lower `ks` values, the cloth is more flexible because of a weaker spring, hence why it appears more "loose" and drapes easier.

## Task 2: Handling collisions with planes
In this task, we handle collisions with planes - for a plane, a collision occurs when the point mass has crossed the plane. We determine if a point had crossed the plane by checking if the dot product of the movement vector and the normal vector is negative. If so, we apply a correction which uses the tangent point at which the collision between the point mass and the plane would have occurred. We also needed to factor in `SURFACE_OFFSET` when multiplying by the normal vector. The correction vector is also similar to collisions with a sphere. Lastly, we also needed to update `Cloth::simulate` to check for collisions between every `PointMass` and `CollisionObject`.

# Part 4
Prior to this section, our cloth when colliding with itself would just clip through itself. To prevent this, we would need to implement that whenever the cloth fell on itself, it would fold. For each point mass, we need to check whether it is within some small number (`2 * thickness`) away from another point (not including itself). If it is too close, then we apply a correction/repulsive collision force (to maintain a minimum of `2 * thickness` distance away).

## Task 1: Cloth::hash_position
In this task, we implemented `Cloth::hash_position` which takes in a point mass's position and calculates its 3D box volume's index in our spatial hash table. The idea is to check within a point's 3D box volume instead of throughout the entire cloth. This was done by first calculating its 3D position `(w, h, t)` where `w = 3 * width / num_width_points`, `h = 3 * height / num_height_points`, and `t = max(w, h)`. We determined the 3D box coordinates by calculating `x = (pos.x - fmod(pos.x, w))/w`, `y = (pos.y - fmod(pos.y, h))/h`, and `z = (pos.z - fmod(pos.z, t))/t`. Afterwards, we transformed the 3D position to 1D through our hash expression.

## Task 2: Cloth::build_spatial_map
In this task, we constructed our spatial map by iterating through all `point_masses` in the cloth and calculated its `hash`. If there isn't already a `vector<PointMass *>` at that position, a new one is initialized and we also `push_back` the current point mass. 

## Task 3: Cloth::self_collide
In this task, we check each point mass to see whether the normalized distance between it and candidate point masses is less than `2 * thickness`. If a collision is detected, a correction vector is calculated based on the difference between the desired separation distance (`2 * thickness`) and the actual distance between the point masses.

If any corrections were calculated, the average correction vector is computed and the position of the given point mass is adjusted by adding the correction vector to it.

We also needed to update `Cloth::simulate` to build our spatial map and iterate through the point masses to call `self_collide` on them. 

### Changing spring constant (ks)
When maintaining the default parameters (`density`, `damping`) and only modifying the spring constant, `ks`, we can see that at a lower `ks` value (`ks = 50 N/m`), the cloth has a bunch of folds, similar to a piece of aluminum foil or tissue paper. The ripples are very small and there are a bunch of self collisions, which makes sense intuitively since the spring constant is smaller. With a lower `ks`, the cloth is more malleable/flexible since it has less structure. We can compare this to `ks` being a greater value — when `ks = 50,000 N/m`, there are less folds ("bigger" folds compared to when `ks = 50 N/m`) and the strength of the cloth is greater due to the higher spring constant.

### Changing density
When maintaining the default parameters (`ks`, `damping`) and only modifying the `density`, we can see that at a lower density (`density = 1 g/cm^2`), the cloth mimics gift wrapping paper in the way it folds (and its appearance). This makes sense because paper generally has low density and doesn't "bunch up" together. A lower density equates to a lower mass, hence why there is less force applied and less self collisions. In contrast, at a higher density, there is greater mass so a larger force is applied to the cloth, causing more self-collisions. This is depicted when `density = 50 g/cm^2`, where the cloth has more folds/ripples (less able to hold its structure) and appears "heavier" like a blanket.

# Part 5
For this part, we are creating various shaders using the language GLSL. Two shader types — vertex and fragment — work together to create the lighting and material effects that we want. The vertex shader deals with the position and transforms of the vertices themselves. After processing the vertex positions, it will save the final vertex position. The output of the vertex shader is inputted into the fragment shader, which essentially just calculates the final color to be outputted. It does this by reading the geometric attributes passed in, and doing the appropiate calculations to get the exact color to be outputted by that fragment.

## Task 1: Diffuse Shading
In this task, we implemented diffuse shading, which is related to Lambert's Law. To implement this, we used the equation given in lecture: we first found the dot product between the normal vector and the direction vector of the light. Then we ensured that it was a positive number, then multiplied it with the light intensity to get the final out color that we need.


## Task 2: Blinn-Phong Shading
The Blinn-Phong shading model essentially takes in three different aspects — ambient, diffuse, and specular — and adds them together to create the final shader. The ambient shading accounts for the constant color without any other factors involved. The diffuse shading accounts for the light and how it interacts with the curve of the object, but it is independent of the view direction. The specular shading accounts for view direction, adding in the intensity of the light based on that. Adding together these three components creates a very realistic shader, accounting for the different parts of the environment.

In this task, we implemented these three parts to create the final Blinn-Phong shader. For the diffuse shader, we took the code from Task 1. For the ambient portion, we simply added the input color itself. For the specular portion, we wanted an effect similar to a mirror. Instead of using the regular normal vector, we used the bisector vector of the light and view vectors. This way, it would depend on the view direction as we want. Finally, we just added all three of these parts to get the final out color.

[INSERT BLINN PHONG IMAGES]

## Task 3: Texture Mapping
In this task, we wanted to create a shader that maps a given texture to the object. We implemented this taking advantage of the texture() function in GLSL that allows us to sample from the given texture at a certain location. Using this, we can get our final out color needed.

Here is an image of the texture mapping shader using our own custom texture.
[INSERT TEXTURE PHOTO HERE]

## Task 4: Displacement and Bump Mapping
In this task, we implemented shaders that account for bumps and displacement. For the bump mapping, we used a height map to calculate a new normal vector at a certain point, and used that vector for the Blinn-Phong shading instead of the original normal. To calculate this new normal, we look at how the height changes using the height map. We used the r component of the color to figure this out, and used the vector position as well as height and normal scalars to find the final normal vector. For displacement mapping on the other hand, we are also adjusting the actual position of the vertices to match the map, creating the displacement that we see on the object. So together, we have the bump created by the new normals that we calculated, as well as varying vertex positions to account for corresponding displacements. 

By changing the sphere's mesh coarseness from 16 to 128, the sphere itself becomes a lot more smooth. When using the bump shader, the bumps are less apparent and look smoother overall. When using the displacement shader, the displacements are more uniform around the sphere. So the cloth lands relatively evenly on the sphere since the displacements still match the shape of the sphere. When the coarseness is back to 16, the bumps and displacements are a lot more apparent, with the displacements being a lot more uneven. This causes the cloth to fall a little unevenly on the displaced vertices of the sphere.

[INSERT PHOTOS OF BUMPS AND DISPLACEMENTS]

## Task 5: Environment-mapped Reflections
In this task, we want to create a shader that creates a direct reflection of the environment, like a mirror. We do this by finding the outgoing eye-ray, and then reflecting it exactly to get the incident ray. It is perfectly reflected so that we get an exact image of the environment, which we can then put in as the out color. 

Here are images of the mirror shader on the cloth and the sphere
[INSERT MIRROR PHOTOS HERE]

## Extra Credit: Custom Shader