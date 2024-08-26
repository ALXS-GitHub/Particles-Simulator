use nalgebra::{Vector3};
use std::ops::{Add, Mul};
use std::sync::{Arc, RwLock};

use crate::classes::particles::sphere::Sphere;
use crate::classes::container::{Container, ContainerTrait};
use std::sync::atomic::{AtomicUsize, Ordering};

static PARTICLE_ID_COUNTER: AtomicUsize = AtomicUsize::new(0);

pub trait Collidable {
    fn collide_with_sphere(&mut self, sphere: Arc<RwLock<Sphere>>);
    // fn collide_with_plane(&self, plane: Rc<RefCell<Plane>>); // impl this later (for the moment not important)
    fn collide_with_container(&mut self, container: Arc<RwLock<dyn ContainerTrait>>);
}

pub struct Particle {
    pub id: usize,
    pub updating_enabled: bool,
    pub previous_position: Vector3<f32>,
    pub position: Vector3<f32>,
    pub velocity: Vector3<f32>,
    pub acceleration: Vector3<f32>,
    pub fixed: bool,
}

impl Particle {
    pub fn new(position: Vector3<f32>, velocity: Vector3<f32>, acceleration: Vector3<f32>, fixed: bool) -> Self {
        let id = PARTICLE_ID_COUNTER.fetch_add(1, Ordering::SeqCst);
        Particle {
            id,
            updating_enabled: true,
            previous_position: position,
            position,
            velocity,
            acceleration,
            fixed,
        }
    }
    
    pub fn add_force(&mut self, force: Vector3<f32>) {
        self.acceleration += force;
    }

    pub fn update_position(&mut self, dt: f32) {
        if !self.fixed {
            let position_copy = self.position.clone();
            if self.updating_enabled {
                self.position += self.position - self.previous_position + self.acceleration * dt * dt;
            }
            self.previous_position = position_copy;
            self.velocity = (self.position - self.previous_position) / dt;
            self.acceleration = Vector3::new(0.0, 0.0, 0.0);
        }
    }


    pub fn move_by(&mut self, move_by: Vector3<f32>) {
        if !self.fixed {
            self.position += move_by;
        }
    }

    pub fn add_offset(&mut self, offset: Vector3<f32>) {
        self.position += offset;
        self.previous_position = self.position
    }

    pub fn set_updating_enabled(&mut self, enabled: bool) {
        self.updating_enabled = enabled;
    }
}

pub trait ParticleTrait {
    fn get_position(&self) -> Vector3<f32>;
    fn add_offset(&mut self, offset: Vector3<f32>);
    fn add_force(&mut self, force: Vector3<f32>);
    fn update_position(&mut self, dt: f32);
    fn move_by(&mut self, move_by: Vector3<f32>);
    fn set_updating_enabled(&mut self, enabled: bool);
}