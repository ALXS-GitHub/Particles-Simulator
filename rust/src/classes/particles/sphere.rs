use std::sync::{Arc, RwLock};
use nalgebra::{Vector3};
use crate::classes::particle::{Particle, Collidable, ParticleTrait};
use crate::classes::container::{Container, ContainerTrait};


pub struct Sphere {
    pub particle: Particle,
    pub radius: f32,
}

impl Sphere {
    pub fn new(position: Vector3<f32>, radius: f32, velocity: Vector3<f32>, acceleration: Vector3<f32>, fixed: bool) -> Self {
        Sphere {
            particle: Particle::new(position, velocity, acceleration, fixed),
            radius,
        }
    }

    pub fn set_position(&mut self, position: Vector3<f32>) {
        self.particle.position = position;
    }
}

impl ParticleTrait for Sphere {

    fn get_position(&self) -> Vector3<f32> {
        self.particle.position
    }

    fn add_offset(&mut self, offset: Vector3<f32>) {
        self.particle.add_offset(offset);
    }

    fn add_force(&mut self, force: Vector3<f32>) {
        self.particle.add_force(force);
    }

    fn update_position(&mut self, dt: f32) {
        self.particle.update_position(dt);
    }

    fn move_by(&mut self, move_by: Vector3<f32>) {
        self.particle.move_by(move_by);
    }

    fn set_updating_enabled(&mut self, enabled: bool) {
        self.particle.set_updating_enabled(enabled);
    }
}

impl Collidable for Sphere {
    fn collide_with_sphere(&mut self, other: Arc<RwLock<Sphere>>) {

        if let Ok(mut other) = other.try_write() {
            // Your code that operates on `other`
            let axis = self.particle.position - other.particle.position;
            let distance = axis.norm();
            let overlap = self.radius + other.radius - distance;
            if overlap > 0.0 {
                let axis = axis.normalize();
                let move_vec = axis * overlap * 0.5;
                self.particle.move_by(move_vec);
                other.particle.move_by(-move_vec);
            }
        } else {
            return
        }
    }

    // TODO : implement the plane structure correctly later (for the moment this is not the most important)
    // fn collide_with_plane(&mut self, plane: &Plane) {
    //     let normal: Vector3<f32> = plane.get_normal();
    //     let size = plane.get_size();
    //     let mut u;
    //     if normal.dot(&Vector3::new(1.0, 0.0, 0.0)).abs() < 0.0001 {
    //         u = normal.cross(&Vector3::new(1.0, 0.0, 0.0)).normalize();
    //     } else {
    //         u = normal.cross(&Vector3::new(0.0, 1.0, 0.0)).normalize();
    //     }
    //     let v = normal.cross(&u);
    //     let projected_position = self.particle.position - normal.dot(&(self.particle.position - plane.position)) * normal;
    //     let axis = self.particle.position - projected_position;
    //     let distance: f32 = axis.norm();
    //     let diff = projected_position - plane.position;
    //     let u_diff = diff.dot(&u);
    //     let v_diff = diff.dot(&v);
    //     let penetration: f32 = self.radius - distance;
    //     if u_diff.abs() <= size.x / 2.0 && v_diff.abs() <= size.y / 2.0 {
    //         if penetration > 0.0 {
    //             // TODO : check if this error disappears later, the problem is supposed to be the normal vector type
    //             self.particle.position += penetration * normal * axis.dot(&normal).signum();
    //         }
    //     }
    // }

    // TODO : fix the multi container implementation
    fn collide_with_container(&mut self, container: Arc<RwLock<dyn ContainerTrait>>) {
        let container = container.write().unwrap();
        container.collide_with(self);
    }
}