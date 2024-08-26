use nalgebra::Vector3;
use std::sync::{Arc, RwLock};
use std::collections::HashMap;
use rand::Rng;
use std::fs::File;
use std::io::{BufReader, Read};
use serde_json::{Value};
use rayon::prelude::*; // for parallel processing
use std::any::Any;

use crate::classes::particle::{Particle, ParticleTrait, Collidable};
use crate::classes::particles::sphere::{self, Sphere};
use crate::classes::molecule::Molecule;
use crate::classes::container::{Container, ContainerTrait};
use crate::classes::containers::cube_container::CubeContainer;
use crate::classes::containers::sphere_container::SphereContainer;
use crate::classes::grid::Grid;
use crate::config::{MAX_PARTICLE_RADIUS};
use crate::{ADD_PARTICLE_NUM, NUM_SUBSTEPS};
use crate::utils::parser::{parse_container, parse_sphere, parse_molecule};


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
    pub molecules: Vec<Arc<RwLock<Molecule>>>,
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
            molecules: Vec::new(),
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

        // * parallel version
        grid_as_vector.par_iter().enumerate().for_each(|(index, (&cell, spheres))| {
            let neighbors = self.grid.get_neighbors_in_cell(cell);
            for s in spheres.iter() {
                let sphere_id = {
                    if let Ok(sphere_read) = s.try_read() {
                        sphere_read.particle.id
                    } else {
                        continue; // Skip if read lock is not available
                    }
                };
                for neighbor in &neighbors {
                    let neighbor_id = {
                        if let Ok(neighbor_read) = neighbor.try_read() {
                            neighbor_read.particle.id
                        } else {
                            continue; // Skip if read lock is not available
                        }
                    };
                    if sphere_id != neighbor_id {
                        if let Ok(mut sphere) = s.try_write() {
                            sphere.collide_with_sphere(neighbor.clone());
                        }
                    }
                }
                for container in &self.containers {
                    if let Ok(mut sphere) = s.try_write() {
                        sphere.collide_with_container(container.clone());
                    }
                }
            }
        });

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
        for molecule in &self.molecules {
            if let Ok(mut molecule) = molecule.try_write() {
                if molecule.links_enabled {
                    molecule.maintain_distance_links();
                } else {
                    molecule.maintain_distance_all();
                }
                if molecule.use_internal_pressure {
                    molecule.add_internal_pressure();
                }
            }
        }
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

    pub fn load_molecule(&mut self, filename: String, offset: Vector3<f32>) -> Arc<RwLock<Molecule>> {
        let file = File::open(filename).expect("Unable to open file");
        let reader = BufReader::new(file);
        let j: Value = serde_json::from_reader(reader).expect("Unable to parse JSON");

        // Create a new molecule
        let mut internal_pressure = 0.001;
        let mut use_internal_pressure = false;
        if let Some(pressure) = j.get("internalPressure") {
            internal_pressure = pressure.as_f64().unwrap() as f32;
            use_internal_pressure = true;
        }
        let molecule = Arc::new(RwLock::new(Molecule::new(
            Some(j["distance"].as_f64().unwrap() as f32),
            j["linksEnabled"].as_bool(),
            Some(j["strength"].as_f64().unwrap() as f32),
            Some(internal_pressure),
            Some(use_internal_pressure),
        )));

        // Create a vector to store the spheres
        let mut spheres: Vec<Arc<RwLock<Sphere>>> = Vec::new();

        // Iterate over the spheres in the molecule
        for j_sphere in j["spheres"].as_array().unwrap() {
            // Init the parameters
            let mut position = Vector3::new(
                j_sphere["position"][0].as_f64().unwrap() as f32,
                j_sphere["position"][1].as_f64().unwrap() as f32,
                j_sphere["position"][2].as_f64().unwrap() as f32,
            ) + offset;
            let radius = j_sphere["radius"].as_f64().unwrap() as f32;
            let mut velocity = Vector3::new(0.0, 0.0, 0.0);
            let mut acceleration = Vector3::new(0.0, 0.0, 0.0);
            let mut fixed = false;

            // Check for optional parameters
            if let Some(fixed_value) = j_sphere.get("fixed") {
                fixed = fixed_value.as_bool().unwrap();
            }

            if let Some(velocity_value) = j_sphere.get("velocity") {
                velocity = Vector3::new(
                    velocity_value[0].as_f64().unwrap() as f32,
                    velocity_value[1].as_f64().unwrap() as f32,
                    velocity_value[2].as_f64().unwrap() as f32,
                );
            }

            if let Some(acceleration_value) = j_sphere.get("acceleration") {
                acceleration = Vector3::new(
                    acceleration_value[0].as_f64().unwrap() as f32,
                    acceleration_value[1].as_f64().unwrap() as f32,
                    acceleration_value[2].as_f64().unwrap() as f32,
                );
            }

            // Create a new sphere
            let sphere = self.create_sphere(position, radius, velocity, acceleration, fixed);

            // Add the sphere to the molecule and the spheres vector
            molecule.write().unwrap().add_sphere(sphere.clone());
            spheres.push(sphere);
        }

        // Iterate over the links in the molecule
        for j_link in j["links"].as_array().unwrap() {
            // Add a link between the spheres
            molecule.write().unwrap().add_link(
                spheres[j_link[0].as_u64().unwrap() as usize].clone(),
                spheres[j_link[1].as_u64().unwrap() as usize].clone(),
            );
        }

        // Add the molecule to the simulation
        self.molecules.push(molecule.clone());

        molecule
    }


    pub fn load_world(&mut self, filename: String) {
        let file = File::open(&filename).expect(&format!("file not found: {}", &filename));
        let reader = BufReader::new(file);
        let j: Value = serde_json::from_reader(reader).expect("Unable to parse JSON");

        // load the containers
        for j_container in j["containers"].as_array().unwrap() {
            let container = parse_container(j_container);
            self.containers.push(container.clone());
            if let Ok(container_lock) = container.try_read() {
                if let Some(cube_container) = container_lock.as_any().downcast_ref::<CubeContainer>() {
                    self.cube_containers.push(Arc::new(RwLock::new(cube_container.clone())));
                } else if let Some(sphere_container) = container_lock.as_any().downcast_ref::<SphereContainer>() {
                    self.sphere_containers.push(Arc::new(RwLock::new(sphere_container.clone())));
                }
            };
        }

        // load the spheres
        for j_sphere in j["spheres"].as_array().unwrap() {
            let sphere = parse_sphere(j_sphere);
            self.particles.push(sphere.clone());
            self.spheres.push(sphere.clone());
        }

        // load the molecules
        for j_molecule in j["molecules"].as_array().unwrap() {
            let molecule = parse_molecule(j_molecule);
            for sphere in &molecule.read().unwrap().spheres {
                self.particles.push(sphere.clone());
                self.spheres.push(sphere.clone());
            }
            self.molecules.push(molecule.clone());
        }
    }

    // § main loop events methods

    pub fn attract_particles_to_origin(&self, force_strength: f32) {
        for particle in &self.particles {
            let mut particle = particle.write().unwrap();
            let particle_pos = particle.get_position();
            particle.add_force(Vector3::new(0.0, 10.0, 0.0) * NUM_SUBSTEPS as f32); // counter gravity
            particle.add_force(-particle_pos.normalize() * force_strength * NUM_SUBSTEPS as f32);
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