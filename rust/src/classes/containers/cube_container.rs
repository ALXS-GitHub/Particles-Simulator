use nalgebra::{Vector3};

use crate::classes::particles::sphere::Sphere;
use crate::classes::container::{ContainerTrait, Container};

pub struct CubeContainer {
    container: Container,
    size: Vector3<f32>,
}

impl CubeContainer {
    pub fn new(position: Vector3<f32>, size: Vector3<f32>, forced_inside: bool) -> Self {
        CubeContainer {
            container: Container::new(position, forced_inside),
            size,
        }
    }
}

impl ContainerTrait for CubeContainer {
    fn get_size(&self) -> Vector3<f32> {
        self.size
    }

    fn get_position(&self) -> Vector3<f32> {
        self.container.position
    }

    fn set_position(&mut self, position: Vector3<f32>) {
        self.container.position = position;
    }

    fn set_size(&mut self, size: Vector3<f32>) {
        self.size = size;
    }

    fn collide_with(&self, sphere: &mut Sphere) {
        let sphere_position = sphere.particle.position;
        let sphere_radius = sphere.radius;
        let min = self.container.position - self.size / 2.0;
        let max = self.container.position + self.size / 2.0;

        if !self.container.forced_inside && !(min.x <= sphere_position.x && sphere_position.x <= max.x && min.y <= sphere_position.y && sphere_position.y <= max.y && min.z <= sphere_position.z && sphere_position.z <= max.z) {
            return;
        }

        if sphere_position.x - sphere_radius < min.x {
            sphere.particle.position.x = min.x + sphere_radius;
        }
        if sphere_position.x + sphere_radius > max.x {
            sphere.particle.position.x = max.x - sphere_radius;
        }
        if sphere_position.y - sphere_radius < min.y {
            sphere.particle.position.y = min.y + sphere_radius;
        }
        if sphere_position.y + sphere_radius > max.y {
            sphere.particle.position.y = max.y - sphere_radius;
        }
        if sphere_position.z - sphere_radius < min.z {
            sphere.particle.position.z = min.z + sphere_radius;
        }
        if sphere_position.z + sphere_radius > max.z {
            sphere.particle.position.z = max.z - sphere_radius;
        }
    }
}