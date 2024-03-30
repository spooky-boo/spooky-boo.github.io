#include <iostream>
#include <math.h>
#include <random>
#include <vector>

#include "cloth.h"
#include "collision/plane.h"
#include "collision/sphere.h"

using namespace std;

Cloth::Cloth(double width, double height, int num_width_points,
             int num_height_points, float thickness) {
  this->width = width;
  this->height = height;
  this->num_width_points = num_width_points;
  this->num_height_points = num_height_points;
  this->thickness = thickness;

  buildGrid();
  buildClothMesh();
}

Cloth::~Cloth() {
  point_masses.clear();
  springs.clear();

  if (clothMesh) {
    delete clothMesh;
  }
}

void Cloth::buildGrid() {
  // TODO (Part 1): Build a grid of masses and springs.
  
  // Masses
  for (int i = 0; i < num_height_points; i++) {
    for (int j = 0; j < num_width_points; j++) {
      // Masses need to be evenly spaced
      double x = (width / num_width_points) * j;
      double y = (height / num_height_points) * i;

      bool pin = false;
      // If the cloth's orientation is HORIZONTAL, y coordinate == 1 while varying positions over the xz plane
      if (orientation == HORIZONTAL) {
        for (int k = 0; k < pinned.size(); k++) {
          if (j == pinned[k][0] && i == pinned[k][1]) {
            pin = true;
            break;
          }
        }
        PointMass p = PointMass(Vector3D(x, 1, y), pin);
        point_masses.push_back(p);
      } else {
        // Else: the orientation is VERTICAL, generate a small random offset between -1/1000 and 1/1000 for each point mass and use that as the z coordinate while varying positions over the xy plane.
        for (int k = 0; k < pinned.size(); k++) {
          if (j == pinned[k][0] && i == pinned[k][1]) {
            pin = true;
            break;
          }
        }
        double z = (double)rand() / RAND_MAX * (1 / 1000) - (1 / 2000);
        PointMass p = PointMass(Vector3D(x, y, z), pin);
        point_masses.emplace_back(p);
      }
    }
  }

  // Springs (STRUCTURAL, SHEARING, or BENDING)
  for (int i = 0; i < num_height_points; i++) { 
    for (int j = 0; j < num_width_points; j++) {
      // Structural == point mass and the point mass to its left + the point mass above it.
      if (j > 0) {
        Spring leftSpring = Spring(&point_masses[i * num_width_points + j], &point_masses[i * num_width_points + (j-1)], STRUCTURAL);
        springs.emplace_back(leftSpring);
      }
      if (i+1 < num_height_points) {
        Spring aboveSpring = Spring(&point_masses[i * num_width_points + j], &point_masses[(i+1) * num_width_points + j], STRUCTURAL);
        springs.emplace_back(aboveSpring);
      }

      // Shearing constraints == point mass and the point mass to its diagonal upper left + the point mass to its diagonal upper right.
      if (j > 0 && i > 0) {
        Spring diagonalLeftSpring = Spring(&point_masses[i * num_width_points + j], &point_masses[(i-1) * num_width_points + (j-1)], SHEARING);
        springs.emplace_back(diagonalLeftSpring);
      }
      if (j+1 < num_width_points && i > 0) {
        Spring diagonalRightSpring = Spring(&point_masses[i * num_width_points + j], &point_masses[(i-1) * num_width_points + (j+1)], SHEARING);
        springs.emplace_back(diagonalRightSpring);
      }
      
      // // Bending constraints == a point mass and the point mass two away to its left + the point mass two above it.
      if (j-1 > 0) {
        Spring twoLeftSpring = Spring(&point_masses[i * num_width_points + j], &point_masses[i * num_width_points + (j-2)], BENDING);
        springs.emplace_back(twoLeftSpring);
      }
      if (i-1 > 0) {
        Spring twoAboveSpring = Spring(&point_masses[i * num_width_points + j], &point_masses[(i-2) * num_width_points + j], BENDING);
        springs.emplace_back(twoAboveSpring);
      }
    }
  }
}

void Cloth::simulate(double frames_per_sec, double simulation_steps, ClothParameters *cp,
                     vector<Vector3D> external_accelerations,
                     vector<CollisionObject *> *collision_objects) {
  double mass = width * height * cp->density / num_width_points / num_height_points;
  double delta_t = 1.0f / frames_per_sec / simulation_steps;

  // TODO (Part 2): Compute total force acting on each point mass.

  // Compute a total external force based on the external_accelerations and the mass
  for (int i = 0; i < point_masses.size(); i++) {
    PointMass* p = &point_masses[i];
    for (int j = 0; j < external_accelerations.size(); j++) {
      p->forces += mass * external_accelerations[j];
    }
  }

  // Apply the spring correction forces
  for (int i = 0; i < springs.size(); i++) {
    Spring* s = &springs[i];
    double ks = cp->ks; // spring constant
    Vector3D p_a = s->pm_a->position;
    Vector3D p_b = s->pm_b->position;
    double l = s-> rest_length; // rest length 

    // Bending constraint should be weaker than structural or shearing constraints
    if (cp->enable_bending_constraints) {
      s->pm_a->forces += (ks * 0.2) * ((p_a-p_b).norm() - l);
      s->pm_b->forces += (ks * 0.2) * ((p_a-p_b).norm() - l);
    } else if (cp->enable_structural_constraints || cp->enable_shearing_constraints) {
      s->pm_a->forces += (ks) * ((p_a-p_b).norm() - l);
      s->pm_b->forces += (ks) * ((p_a-p_b).norm() - l);
    }
  }

  // TODO (Part 2): Use Verlet integration to compute new point mass positions
  for (int i = 0; i < point_masses.size(); i++) {
    PointMass* p = &point_masses[i];

    // Do stuff if point mass isn't pinned
    if(p->pinned == false) {
      Vector3D currPos = p -> position;
      double d = (cp -> damping) / 100;
      Vector3D a_t = p -> forces / mass;
      p->position = currPos + (1 - d) * (currPos - p->last_position) + a_t * (delta_t * delta_t);
      p->last_position = currPos;
    }
  }

  // TODO (Part 4): Handle self-collisions.


  // TODO (Part 3): Handle collisions with other primitives.


  // TODO (Part 2): Constrain the changes to be such that the spring does not change
  // in length more than 10% per timestep [Provot 1995].

  // For each spring, apply this constraint by correcting the two point masses' positions such that 
  // the spring's length is at most 10% greater than its rest_length at the end of any time step.
  for (int i = 0; i < springs.size(); i++) {
    Spring* s = &springs[i];
    Vector3D p_a = s->pm_a->position;
    Vector3D p_b = s->pm_b->position;
    double l = (p_a-p_b).norm() - (s->rest_length * 1.1); // spring's length is at most 10% greater than its rest_length at the end of any time step
    bool adjust = !(l <= 0 || (s->pm_a->pinned && s->pm_b->pinned)); // no adjusting if both are pinned

    if (adjust) {
      if (s->pm_a->pinned) {
        p_b += l;
      } else if (s->pm_b->pinned) {
        p_a += l;
      } else {
        p_a += l / 2;
        p_b += l / 2;
      }
    }
  }
}

void Cloth::build_spatial_map() {
  for (const auto &entry : map) {
    delete(entry.second);
  }
  map.clear();

  // TODO (Part 4): Build a spatial map out of all of the point masses.

}

void Cloth::self_collide(PointMass &pm, double simulation_steps) {
  // TODO (Part 4): Handle self-collision for a given point mass.

}

float Cloth::hash_position(Vector3D pos) {
  // TODO (Part 4): Hash a 3D position into a unique float identifier that represents membership in some 3D box volume.

  return 0.f; 
}

///////////////////////////////////////////////////////
/// YOU DO NOT NEED TO REFER TO ANY CODE BELOW THIS ///
///////////////////////////////////////////////////////

void Cloth::reset() {
  PointMass *pm = &point_masses[0];
  for (int i = 0; i < point_masses.size(); i++) {
    pm->position = pm->start_position;
    pm->last_position = pm->start_position;
    pm++;
  }
}

void Cloth::buildClothMesh() {
  if (point_masses.size() == 0) return;

  ClothMesh *clothMesh = new ClothMesh();
  vector<Triangle *> triangles;

  // Create vector of triangles
  for (int y = 0; y < num_height_points - 1; y++) {
    for (int x = 0; x < num_width_points - 1; x++) {
      PointMass *pm = &point_masses[y * num_width_points + x];
      // Get neighboring point masses:
      /*                      *
       * pm_A -------- pm_B   *
       *             /        *
       *  |         /   |     *
       *  |        /    |     *
       *  |       /     |     *
       *  |      /      |     *
       *  |     /       |     *
       *  |    /        |     *
       *      /               *
       * pm_C -------- pm_D   *
       *                      *
       */
      
      float u_min = x;
      u_min /= num_width_points - 1;
      float u_max = x + 1;
      u_max /= num_width_points - 1;
      float v_min = y;
      v_min /= num_height_points - 1;
      float v_max = y + 1;
      v_max /= num_height_points - 1;
      
      PointMass *pm_A = pm                       ;
      PointMass *pm_B = pm                    + 1;
      PointMass *pm_C = pm + num_width_points    ;
      PointMass *pm_D = pm + num_width_points + 1;
      
      Vector3D uv_A = Vector3D(u_min, v_min, 0);
      Vector3D uv_B = Vector3D(u_max, v_min, 0);
      Vector3D uv_C = Vector3D(u_min, v_max, 0);
      Vector3D uv_D = Vector3D(u_max, v_max, 0);
      
      
      // Both triangles defined by vertices in counter-clockwise orientation
      triangles.push_back(new Triangle(pm_A, pm_C, pm_B, 
                                       uv_A, uv_C, uv_B));
      triangles.push_back(new Triangle(pm_B, pm_C, pm_D, 
                                       uv_B, uv_C, uv_D));
    }
  }

  // For each triangle in row-order, create 3 edges and 3 internal halfedges
  for (int i = 0; i < triangles.size(); i++) {
    Triangle *t = triangles[i];

    // Allocate new halfedges on heap
    Halfedge *h1 = new Halfedge();
    Halfedge *h2 = new Halfedge();
    Halfedge *h3 = new Halfedge();

    // Allocate new edges on heap
    Edge *e1 = new Edge();
    Edge *e2 = new Edge();
    Edge *e3 = new Edge();

    // Assign a halfedge pointer to the triangle
    t->halfedge = h1;

    // Assign halfedge pointers to point masses
    t->pm1->halfedge = h1;
    t->pm2->halfedge = h2;
    t->pm3->halfedge = h3;

    // Update all halfedge pointers
    h1->edge = e1;
    h1->next = h2;
    h1->pm = t->pm1;
    h1->triangle = t;

    h2->edge = e2;
    h2->next = h3;
    h2->pm = t->pm2;
    h2->triangle = t;

    h3->edge = e3;
    h3->next = h1;
    h3->pm = t->pm3;
    h3->triangle = t;
  }

  // Go back through the cloth mesh and link triangles together using halfedge
  // twin pointers

  // Convenient variables for math
  int num_height_tris = (num_height_points - 1) * 2;
  int num_width_tris = (num_width_points - 1) * 2;

  bool topLeft = true;
  for (int i = 0; i < triangles.size(); i++) {
    Triangle *t = triangles[i];

    if (topLeft) {
      // Get left triangle, if it exists
      if (i % num_width_tris != 0) { // Not a left-most triangle
        Triangle *temp = triangles[i - 1];
        t->pm1->halfedge->twin = temp->pm3->halfedge;
      } else {
        t->pm1->halfedge->twin = nullptr;
      }

      // Get triangle above, if it exists
      if (i >= num_width_tris) { // Not a top-most triangle
        Triangle *temp = triangles[i - num_width_tris + 1];
        t->pm3->halfedge->twin = temp->pm2->halfedge;
      } else {
        t->pm3->halfedge->twin = nullptr;
      }

      // Get triangle to bottom right; guaranteed to exist
      Triangle *temp = triangles[i + 1];
      t->pm2->halfedge->twin = temp->pm1->halfedge;
    } else {
      // Get right triangle, if it exists
      if (i % num_width_tris != num_width_tris - 1) { // Not a right-most triangle
        Triangle *temp = triangles[i + 1];
        t->pm3->halfedge->twin = temp->pm1->halfedge;
      } else {
        t->pm3->halfedge->twin = nullptr;
      }

      // Get triangle below, if it exists
      if (i + num_width_tris - 1 < 1.0f * num_width_tris * num_height_tris / 2.0f) { // Not a bottom-most triangle
        Triangle *temp = triangles[i + num_width_tris - 1];
        t->pm2->halfedge->twin = temp->pm3->halfedge;
      } else {
        t->pm2->halfedge->twin = nullptr;
      }

      // Get triangle to top left; guaranteed to exist
      Triangle *temp = triangles[i - 1];
      t->pm1->halfedge->twin = temp->pm2->halfedge;
    }

    topLeft = !topLeft;
  }

  clothMesh->triangles = triangles;
  this->clothMesh = clothMesh;
}
