use crate::classes::camera::Camera;
use crate::classes::simulation::Simulation;
use crate::classes::{particle::ParticleTrait, particles::sphere::Sphere};
use crate::utils::ray::Ray;
use glium::winit::event::{ElementState, Event, KeyEvent, MouseButton, WindowEvent};
use glium::winit::keyboard::{KeyCode, PhysicalKey};
use nalgebra::{Vector3, Vector4};
use std::f32::INFINITY;
use std::sync::{Arc, RwLock};

use crate::config::{
    CAMERA_ASPECT_RATIO, CAMERA_FAR, CAMERA_FOV, CAMERA_NEAR, WINDOW_HEIGHT, WINDOW_WIDTH,
};

pub struct DragState {
    pub left_mouse_button_pressed: bool,
    pub last_cursor_pos: Option<(f64, f64)>,
    pub current_cursor_pos: Option<(f64, f64)>,
}

impl DragState {
    pub fn new() -> Self {
        DragState {
            left_mouse_button_pressed: false,
            last_cursor_pos: None,
            current_cursor_pos: None,
        }
    }
}

pub struct DragParticles {
    last_cursor_pos: (f32, f32),
    is_dragging: bool,
    dragged_particle: Option<Arc<RwLock<Sphere>>>,
    drag_distance: f32,
    fixed_drag: bool,
}

impl DragParticles {
    pub fn new(fixed_drag: bool) -> Self {
        DragParticles {
            last_cursor_pos: (0., 0.),
            is_dragging: false,
            dragged_particle: None,
            drag_distance: 0.0,
            fixed_drag: fixed_drag,
        }
    }

    pub fn set_dragged_particle(&mut self, particle: Arc<RwLock<Sphere>>) {
        particle.write().unwrap().set_updating_enabled(false);
        self.dragged_particle = Some(particle);
    }

    pub fn unset_dragged_particle(&mut self) {
        if let Some(particle) = &self.dragged_particle {
            particle.write().unwrap().set_updating_enabled(true);
        }
        self.dragged_particle = None;
    }

    pub fn set_drag_distance(&mut self, distance: f32) {
        self.drag_distance = distance;
    }

    pub fn set_is_dragging(&mut self, is_dragging: bool) {
        self.is_dragging = is_dragging;
    }

    pub fn process_drag(&mut self, camera: &Arc<RwLock<Camera>>, sim: &mut Simulation, drag_state: &Arc<RwLock<DragState>>) {
        if drag_state.read().unwrap().left_mouse_button_pressed {
            // supposing we are currently pressing the left mouse button

            let camera_read = camera.read().unwrap();
            let drag_state_read = drag_state.read().unwrap();

            let camera_position = camera_read.position;
            let view_matrix = camera_read.get_view_matrix();
            let projection_matrix = camera_read.get_projection_matrix(CAMERA_FOV, CAMERA_ASPECT_RATIO, CAMERA_NEAR, CAMERA_FAR);

            if !self.is_dragging {
                self.is_dragging = true;
                self.last_cursor_pos = (
                    drag_state_read.current_cursor_pos.unwrap().0 as f32,
                    drag_state_read.current_cursor_pos.unwrap().1 as f32,
                );

                let ray_clip = Vector4::new(
                    (2.0 * self.last_cursor_pos.0 / WINDOW_WIDTH) - 1.0,
                    1.0 - (2.0 * self.last_cursor_pos.1 / WINDOW_HEIGHT),
                    -1.0,
                    1.0,
                );
                let mut ray_eye: Vector4<f32>;
                if let Some(inverse_projection_matrix) = projection_matrix.try_inverse() {
                    ray_eye = inverse_projection_matrix * ray_clip;
                    ray_eye = Vector4::new(ray_eye.x, ray_eye.y, -1.0, 0.0);
                } else {
                    eprintln!("Failed to invert the projection matrix.");
                    return;
                }

                let ray_world: Vector3<f32>;
                if let Some(inverse_view_matrix) = view_matrix.try_inverse() {
                    let ray_world_in = inverse_view_matrix * ray_eye;
                    ray_world = Vector3::new(ray_world_in.x, ray_world_in.y, ray_world_in.z);
                } else {
                    eprintln!("Failed to invert the view matrix.");
                    return;
                }

                let ray = Ray::new(camera_position, ray_world.normalize());

                let mut selected_particle = None;
                let mut min_distance = INFINITY;
                for sphere in sim.spheres.iter() {
                    let distance = ray.intersect(sphere.clone());
                    if distance < min_distance {
                        selected_particle = Some(sphere.clone());
                        min_distance = distance;
                    }
                }

                if let Some(particle) = selected_particle {
                    self.set_dragged_particle(particle);
                    self.set_drag_distance(min_distance);
                }
            }

            if let Some(dragged_particle) = self.dragged_particle.as_ref() {
                if !self.fixed_drag {
                    let mouse_pos_diff = (
                        drag_state_read.current_cursor_pos.unwrap().0 as f32 - self.last_cursor_pos.0,
                        drag_state_read.current_cursor_pos.unwrap().1 as f32 - self.last_cursor_pos.1,
                    );

                    let mut dragged_particle = dragged_particle.write().unwrap();

                    let mut movement = Vector3::new(mouse_pos_diff.0, -mouse_pos_diff.1, 0.0);

                    let distance = (camera_position - dragged_particle.get_position()).norm();

                    let scale_factor = distance / 10000.0;

                    movement *= scale_factor;

                    dragged_particle.move_by(movement);

                    self.last_cursor_pos = (
                        drag_state_read.current_cursor_pos.unwrap().0 as f32,
                        drag_state_read.current_cursor_pos.unwrap().1 as f32,
                    );
                } else {
                    let ray_clip = Vector4::new(
                        (2.0 * self.last_cursor_pos.0 / WINDOW_WIDTH) - 1.0,
                        1.0 - (2.0 * self.last_cursor_pos.1 / WINDOW_HEIGHT),
                        -1.0,
                        1.0,
                    );
                    let mut ray_eye: Vector4<f32>;
                    if let Some(inverse_projection_matrix) = projection_matrix.try_inverse() {
                        ray_eye = inverse_projection_matrix * ray_clip;
                        ray_eye = Vector4::new(ray_eye.x, ray_eye.y, -1.0, 0.0);
                    } else {
                        eprintln!("Failed to invert the projection matrix.");
                        return;
                    }
    
                    let ray_world: Vector3<f32>;
                    if let Some(inverse_view_matrix) = view_matrix.try_inverse() {
                        let ray_world_in = inverse_view_matrix * ray_eye;
                        ray_world = Vector3::new(ray_world_in.x, ray_world_in.y, ray_world_in.z).normalize();
                    } else {
                        eprintln!("Failed to invert the view matrix.");
                        return;
                    }

                    let new_position = camera_position + ray_world * self.drag_distance;

                    dragged_particle.write().unwrap().set_position(new_position);

                    self.last_cursor_pos = (
                        drag_state_read.current_cursor_pos.unwrap().0 as f32,
                        drag_state_read.current_cursor_pos.unwrap().1 as f32,
                    );
                }
            }
        } else {
            self.is_dragging = false;
            self.unset_dragged_particle();
        }
    }

    pub fn handle_drag_controls(&self, event: &WindowEvent, last_cursor_pos: &mut Option<(f64, f64)>, drag_state: &Arc<RwLock<DragState>>) {
        match event {
            WindowEvent::CursorMoved { position, .. } => {
                let (x, y) = (position.x, position.y);
                if let Some((last_x, last_y)) = last_cursor_pos {
                    let dx = x - *last_x;
                    let dy = y - *last_y;

                    let mut drag_state_write = drag_state.write().unwrap();
                    drag_state_write.last_cursor_pos = drag_state_write.current_cursor_pos;
                    drag_state_write.current_cursor_pos = Some((x, y));

                    // process drag in main loop
                }
                *last_cursor_pos = Some((x, y));
            }
            WindowEvent::MouseInput { state, button, .. } => {
                if *button == MouseButton::Left {
                    let mut drag_state = drag_state.write().unwrap();
                    drag_state.left_mouse_button_pressed = match state {
                        ElementState::Pressed => true,
                        ElementState::Released => false,
                    };
                }
            }
            _ => (),
        }
    }
}
