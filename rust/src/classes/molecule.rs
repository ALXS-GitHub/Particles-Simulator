use std::{default, sync::{Arc, RwLock}};

use nalgebra::Vector3;

use crate::classes::particles::sphere::Sphere;
use crate::classes::particle::{ParticleTrait};

pub type Link = (Arc<RwLock<Sphere>>, Arc<RwLock<Sphere>>);

pub struct Molecule {
    distance: f32, // distance between teh spheres centers
    strength: f32, // strength of the spring
    pub internal_pressure: f32, // internal pressure of the molecule
    pub spheres: Vec<Arc<RwLock<Sphere>>>,
    pub links: Vec<Link>, // links between the spheres
    pub links_enabled: bool,
    pub use_internal_pressure: bool,
}

impl Default for Molecule {
    fn default() -> Self {
        Molecule {
            distance: 0.5,
            strength: 0.01,
            internal_pressure: 0.001,
            spheres: Vec::new(),
            links: Vec::new(),
            links_enabled: false,
            use_internal_pressure: false,
        }
    }
}

impl Molecule {
    pub fn new(distance: Option<f32>, links_enabled: Option<bool>, strength: Option<f32>, internal_pressure: Option<f32>, use_internal_pressure: Option<bool>) -> Self {
        let default = Molecule::default();
        Molecule {
            distance: distance.unwrap_or(default.distance),
            strength: strength.unwrap_or(default.strength),
            internal_pressure: internal_pressure.unwrap_or(default.internal_pressure),
            spheres: Vec::new(),
            links: Vec::new(),
            links_enabled: links_enabled.unwrap_or(default.links_enabled),
            use_internal_pressure: use_internal_pressure.unwrap_or(default.use_internal_pressure),
        } 
    }

    pub fn add_sphere(&mut self, sphere: Arc<RwLock<Sphere>>) {
        self.spheres.push(sphere);
    }

    pub fn add_link(&mut self, sphere1: Arc<RwLock<Sphere>>, sphere2: Arc<RwLock<Sphere>>) {
        self.links.push((sphere1, sphere2));
    }

    pub fn maintain_distance_all(&mut self) {
        for i in 0..self.spheres.len() {
            for j in i+1..self.spheres.len() {
                self.maintain_distance(self.spheres[i].clone(), self.spheres[j].clone());
            }
        }
    }

    pub fn maintain_distance_links(&mut self) {
        let links: Vec<Link> = self.links.clone();
        for link in links {
            self.maintain_distance(link.0, link.1);
        }
    }



    pub fn maintain_distance(&mut self, sphere1: Arc<RwLock<Sphere>>, sphere2: Arc<RwLock<Sphere>>) {
        if let Ok(mut sphere1) = sphere1.try_write() {
            if let Ok(mut sphere2) = sphere2.try_write() {
                
                let axis = sphere1.get_position() - sphere2.get_position();
                let current_distance = axis.norm();
                
                let correction_distance = (current_distance - self.distance) / current_distance * self.strength;
                let correction_vector = axis * correction_distance;

                sphere1.move_by(-correction_vector);
                sphere2.move_by(correction_vector);
            }
        }
    }

    pub fn add_internal_pressure(&mut self) {
        let mut center = Vector3::new(0.0, 0.0, 0.0);
        for sphere in &self.spheres {
            let sphere_pos = {
                if let Ok(sphere) = sphere.try_read() {
                    sphere.get_position()
                } else {
                    Vector3::new(0.0, 0.0, 0.0)
                }
            };
            center += sphere_pos;
        }
        center /= self.spheres.len() as f32;

        for sphere in &self.spheres {
            if let Ok(mut sphere) = sphere.try_write() {
                let axis = sphere.get_position() - center;
                let current_distance = axis.norm();

                let repulsion_force = self.internal_pressure * current_distance;

                let correction_vector = axis.normalize() * repulsion_force;

                sphere.move_by(correction_vector);
            } else {
                continue; // failed to get write lock
            }
        }
    }

}