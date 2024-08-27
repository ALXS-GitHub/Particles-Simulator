use std::sync::{Arc, RwLock};
use std::f32::consts::PI;
use std::f32::INFINITY;
use nalgebra::Vector3;
use crate::classes::particle::ParticleTrait;
use crate::classes::particles::sphere::Sphere;


#[derive(Debug)]
pub struct Ray {
    origin: Vector3<f32>,
    direction: Vector3<f32>,
}

impl Ray {
    pub fn new(origin: Vector3<f32>, direction: Vector3<f32>) -> Self {
        Ray { origin, direction }
    }

    pub fn intersect(&self, sphere: Arc<RwLock<Sphere>>) -> f32 {
        let sphere = match sphere.try_read() {
            Ok(s) => s,
            Err(e) => {
                return INFINITY;
            }
        };
        let oc = self.origin - sphere.get_position();
        let a = self.direction.dot(&self.direction);
        let b = 2.0 * oc.dot(&self.direction);
        let c = oc.dot(&oc) - sphere.radius * sphere.radius;
        let discriminant = b * b - 4.0 * a * c;

        if discriminant < 0.0 {
            INFINITY
        } else {
            (-b - discriminant.sqrt()) / (2.0 * a)
        }
    }
}
