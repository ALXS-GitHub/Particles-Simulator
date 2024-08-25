use nalgebra::Vector3;
use std::sync::{Arc, RwLock};
use std::collections::HashMap;
use rand::Rng;

use crate::classes::particle::{Particle, ParticleTrait, Collidable};
use crate::classes::particles::sphere::{self, Sphere};
// use crate::classes::particles::molecule::Molecule;
use crate::classes::container::{Container, ContainerTrait};
use crate::classes::containers::cube_container::CubeContainer;
use crate::classes::containers::sphere_container::SphereContainer;
use crate::classes::grid::Grid;
use crate::config::{MAX_PARTICLE_RADIUS};
use crate::{ADD_PARTICLE_NUM, NUM_SUBSTEPS};


pub struct Simulation {
    pub num_particles: i32,
    pub num_threads: i32,
    pub grid: Box<Grid>, // Box is used for heap allocation, Option for nullable
    pub particles: Vec<Arc<RwLock<dyn ParticleTrait + Send + Sync>>>,
    pub spheres: Vec<Arc<RwLock<Sphere>>>,
    // planes: Vec<Arc<RwLock<Plane>>>,
    pub containers: Vec<Arc<RwLock<dyn ContainerTrait>>>,
    pub cube_containers: Vec<Arc<RwLock<CubeContainer>>>,
    pub sphere_containers: Vec<Arc<RwLock<SphereContainer>>>,
    // molecules: Vec<Arc<RwLock<Molecule>>>,
}

impl Simulation {
    pub fn new() -> Self {
        Simulation {
            num_particles: 0,
            num_threads: 4,
            grid: Box::new(Grid::new(MAX_PARTICLE_RADIUS * 2.0)),
            particles: Vec::new(),
            spheres: Vec::new(),
            // planes: Vec::new(),
            containers: Vec::new(),
            cube_containers: Vec::new(),
            sphere_containers: Vec::new(),
            // molecules: Vec::new(),
        }
    }

    pub fn get_num_particles(&self) -> i32 {
        self.num_particles
    }

    pub fn step(&mut self, dt: f32) {
        for particle in &self.particles {
            particle.write().unwrap().update_position(dt);
        }
    }

    pub fn check_collisions(&self) { // regular collision check (without grid)
        // TODO : do multi-threading later
        // TODO : if other particles than spheres are implemented, change behaviors here
        let num_spheres = self.spheres.len();
        for i in 0..num_spheres {
            // * collision with other spheres
            for j in i+1..num_spheres {
                let mut sphere1 = self.spheres[i].write().unwrap();
                let mut sphere2 = self.spheres[j].clone();
                sphere1.collide_with_sphere(sphere2);
            }
            // * collision with containers
            for container in &self.containers {
                let mut sphere = self.spheres[i].write().unwrap();
                sphere.collide_with_container(container.clone());
            }
        }
    }

    pub fn check_grid_collisions(&mut self) {
        // clear grid
        self.grid.clear();
        for sphere in &self.spheres {
            self.grid.insert(sphere);
        }

        // Convert grid to vector
        let grid_as_vector: Vec<_> = (*self.grid).grid.iter().collect();
        let num_cells = grid_as_vector.len();

        // Check collisions
        for i in 0..num_cells {
            let neighbors = self.grid.get_neighbors_in_cell(*grid_as_vector[i].0);
            for s in grid_as_vector[i].1 {
                let sphere_id = {
                    let sphere_read = s.read().unwrap().particle.id;
                    sphere_read
                };
                for neighbor in &neighbors {
                    let neighbor_id = {
                        let neighbor_read = neighbor.read().unwrap().particle.id;
                        neighbor_read
                    };
                    if sphere_id != neighbor_id {
                        let mut sphere = s.write().unwrap();
                        sphere.collide_with_sphere(neighbor.clone());
                    }
                }
                for container in &self.containers {
                    let mut sphere = s.write().unwrap();
                    sphere.collide_with_container(container.clone());
                }
            }
        }

    }

    pub fn add_force(&mut self, force: Vector3<f32>) {
        for particle in &self.particles {
            particle.write().unwrap().add_force(force);
        }
    }

    pub fn create_cube_container(&mut self, position: Vector3<f32>, size: Vector3<f32>, forded_inside: bool) {
        let container = Arc::new(RwLock::new(CubeContainer::new(position, size, forded_inside)));
        self.containers.push(container.clone());
        self.cube_containers.push(container);
    }

    pub fn create_sphere_container(&mut self, position: Vector3<f32>, radius: f32, forded_inside: bool) {
        let size = Vector3::new(radius * 2.0, radius * 2.0, radius * 2.0);
        let container = Arc::new(RwLock::new(SphereContainer::new(position, size, forded_inside)));
        self.containers.push(container.clone());
        self.sphere_containers.push(container);
    }

    pub fn maintain_molecules(&mut self) {
        // Method body to be provided later
    }

    pub fn add_sphere_to_sim(&mut self, sphere: Arc<RwLock<Sphere>>) -> Arc<RwLock<Sphere>> {
        self.particles.push(sphere.clone());
        self.spheres.push(sphere.clone());
        sphere
    }

    pub fn create_sphere(&mut self, position: Vector3<f32>, radius: f32, velocity: Vector3<f32>, acceleration: Vector3<f32>, fixed: bool) -> Arc<RwLock<Sphere>> {
        let sphere = Arc::new(RwLock::new(Sphere::new(position, radius, velocity, acceleration, fixed)));
        self.particles.push(sphere.clone());
        self.spheres.push(sphere.clone());
        sphere
    }

    // pub fn load_molecule(&mut self, filename: String, offset: Vector3<f32>) -> Arc<RwLock<Molecule>> {
    //     // Method body to be provided later
    //     Arc::new(RwLock::new(Molecule {
    //         // Initialize Molecule fields here
    //     }))
    // }

    // pub fn load_world(&mut self, filename: String) {
    //     // Method body to be provided later
    // }

    // § main loop events methods

    pub fn attract_particles_to_origin(&self, force_strength: f32) {
        for particle in &self.particles {
            let mut particle = particle.write().unwrap();
            let particle_pos = particle.get_position();
            particle.add_force(Vector3::new(0.0, 10.0, 0.0) * NUM_SUBSTEPS as f32); // counter gravity
            particle.add_force(-particle_pos * force_strength * NUM_SUBSTEPS as f32);
        }
    }

    pub fn generate_random_particles(&mut self, range: f32) {

        let mut rng = rand::thread_rng();
        let radius = 0.15;
        let fixed = false;
        let velocity = Vector3::new(0.0, 0.0, 0.0);
        let acceleration = Vector3::new(0.0, 0.0, 0.0);

        for _ in 0..ADD_PARTICLE_NUM {
            let position = Vector3::new(
                (rng.gen::<f32>() * range) - range / 2.0,
                (rng.gen::<f32>() * range) - range / 2.0,
                (rng.gen::<f32>() * range) - range / 2.0
            );
            self.create_sphere(position, radius, velocity, acceleration, fixed);
        }
    }

}