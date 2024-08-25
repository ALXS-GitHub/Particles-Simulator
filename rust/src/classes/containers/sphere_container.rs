use nalgebra::{Vector3};

use crate::classes::particles::sphere::Sphere;
use crate::classes::container::{ContainerTrait, Container};

pub struct SphereContainer {
    container: Container,
    size: Vector3<f32>,
}

impl SphereContainer {
    pub fn new(position: Vector3<f32>, size: Vector3<f32>, forced_inside: bool) -> Self {
        SphereContainer {
            container: Container::new(position, forced_inside),
            size,
        }
    }
}

impl ContainerTrait for SphereContainer {
    fn get_position(&self) -> Vector3<f32> {
        self.container.position
    }
    
    fn set_position(&mut self, position: Vector3<f32>) {
        self.container.position = position;
    }
    
    fn get_size(&self) -> Vector3<f32> {
        self.size
    }

    fn set_size(&mut self, size: Vector3<f32>) {
        self.size = size;
    }

    fn collide_with(&self, sphere: &mut Sphere) {
        let sphere_position = sphere.particle.position;
        let sphere_radius = sphere.radius;
        let radius = self.size.x;

        let axis = sphere_position - self.container.position;
        let distance = axis.norm();

        if !self.container.forced_inside && distance > radius {
            // the sphere is outside the container
            return;
        }

        // collision resolution code
        if (distance + sphere_radius) > radius {
            let penetration = distance + sphere_radius - radius;
            sphere.particle.position -= penetration * axis.normalize();
        }
    }
}