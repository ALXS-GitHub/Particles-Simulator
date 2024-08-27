extern crate glium;
extern crate nalgebra as na;

use glium::uniforms::{UniformValue, UniformsStorage, EmptyUniforms};
use glium::Program;
use glium::uniform;
use na::{Matrix4, Point3, Vector3};
use std::f32::consts::PI;

pub struct Camera {
    pub position: Vector3<f32>,
    direction: Vector3<f32>,
    up: Vector3<f32>,
}

impl Camera {
    pub fn new(position: Vector3<f32>, direction: Vector3<f32>, up: Vector3<f32>) -> Self {
        Camera {
            position,
            direction,
            up,
        }
    }

    pub fn get_view_matrix(&self) -> Matrix4<f32> {
        Matrix4::look_at_rh(
            &Point3::from(self.position),
            &Point3::from(self.position + self.direction),
            &self.up,
        )
    }

    pub fn get_projection_matrix(&self, fov: f32, aspect: f32, near: f32, far: f32) -> Matrix4<f32> {
        Matrix4::new_perspective(aspect, fov, near, far)
    }

    // pub fn load_matrices_into_shader(&self, program: &Program, fov: f32, aspect: f32, near: f32, far: f32) {
    //     let view_matrix = self.get_view_matrix();
    //     let projection_matrix = self.get_projection_matrix(fov, aspect, near, far);

    //     let uniforms = UniformsStorage::new("viewMatrix", UniformValue::Mat4(view_matrix.as_slice()))
    //         .add("projectionMatrix", UniformValue::Mat4(projection_matrix.as_slice()));

    //     program.uniforms().set(&uniforms);
    // }

    pub fn get_uniforms(&self, fov: f32, aspect: f32, near: f32, far: f32) -> UniformsStorage<[[f32; 4]; 4], UniformsStorage<[[f32; 4]; 4], EmptyUniforms>> {
        let view_matrix = self.get_view_matrix();
        let projection_matrix = self.get_projection_matrix(fov, aspect, near, far);

        let view_matrix_array: [[f32; 4]; 4] = view_matrix.into();
        let projection_matrix_array: [[f32; 4]; 4] = projection_matrix.into();

        let uni = uniform! {
            viewMatrix: view_matrix_array,
            projectionMatrix: projection_matrix_array,
        };

        uni

    }

    pub fn move_forward(&mut self, distance: f32) {
        let mut horizontal_direction = self.direction;
        horizontal_direction.y = 0.0;
        horizontal_direction = horizontal_direction.normalize();
        self.position += horizontal_direction * distance;
    }

    pub fn move_in_direction(&mut self, distance: f32) {
        self.position += self.direction.normalize() * distance;
    }

    pub fn move_right(&mut self, distance: f32) {
        let mut horizontal_direction = self.direction;
        horizontal_direction.y = 0.0;
        horizontal_direction = horizontal_direction.normalize();
        let right = horizontal_direction.cross(&self.up).normalize();
        self.position += right * distance;
    }

    pub fn move_up(&mut self, distance: f32) {
        self.position += self.up * distance;
    }

    pub fn move_y(&mut self, distance: f32) {
        self.position.y += distance;
    }

    pub fn rotate(&mut self, yaw: f32, pitch: f32) {
        let yaw_matrix = Matrix4::new_rotation(self.up * yaw.to_radians());
        self.direction = (yaw_matrix * self.direction.to_homogeneous()).xyz().normalize();
        let right = self.direction.cross(&self.up).normalize();
        let pitch_matrix = Matrix4::new_rotation(right * pitch.to_radians());
        self.direction = (pitch_matrix * self.direction.to_homogeneous()).xyz().normalize();
        self.up = right.cross(&self.direction).normalize();
    }

    pub fn rotate_horizontal(&mut self, angle: f32) {
        // let mut horizontal_direction = self.direction;
        // horizontal_direction.y = 0.0;
        // horizontal_direction = horizontal_direction.normalize();
        let rotation = Matrix4::new_rotation(Vector3::y() * angle.to_radians());
        self.direction = (rotation * self.direction.to_homogeneous()).xyz().normalize();
        self.up = (rotation * self.up.to_homogeneous()).xyz().normalize();
        let right = self.direction.cross(&Vector3::y()).normalize();
        self.up = right.cross(&self.direction);
    }

    pub fn rotate_vertical(&mut self, angle: f32) {
        let right = self.up.cross(&self.direction).normalize();
        let new_direction = (Matrix4::new_rotation(right * angle.to_radians()) * self.direction.to_homogeneous()).xyz().normalize();
        if new_direction.dot(&self.up).abs() < 0.95 {
            self.direction = new_direction;
        }
    }

    pub fn reset_yaw(&mut self) {
        self.up = Vector3::y();
    }
}