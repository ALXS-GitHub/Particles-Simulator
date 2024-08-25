use nalgebra::Vector3;

use crate::classes::particles::sphere::Sphere;

pub struct Container {
    pub position: Vector3<f32>,
    pub forced_inside: bool,
}

impl Container {
    pub fn new(position: Vector3<f32>, forced_inside: bool) -> Self {
        Container {
            position,
            forced_inside,
        }
    }

    pub fn get_position(&self) -> Vector3<f32> {
        self.position
    }

    pub fn set_position(&mut self, position: Vector3<f32>) {
        self.position = position;
    }

    pub fn get_forced_inside(&self) -> bool {
        self.forced_inside
    }
}

pub trait ContainerTrait {
    fn collide_with(&self, sphere: &mut Sphere);
    fn get_position(&self) -> Vector3<f32>;
    fn set_position(&mut self, position: Vector3<f32>);
    fn get_size(&self) -> Vector3<f32>;
    fn set_size(&mut self, size: Vector3<f32>);
}