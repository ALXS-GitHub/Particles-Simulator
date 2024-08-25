use glium::{glutin, Display};
use glium::winit::event::{ElementState, Event, MouseButton, WindowEvent, KeyEvent};
use glium::winit::keyboard::{KeyCode, PhysicalKey};
use nalgebra::Vector3;
use std::sync::{Arc, RwLock};

// TODO make multi key press working

use crate::classes::camera::Camera;

pub struct CameraState {
    pub moving_forward: bool,
    pub moving_backward: bool,
    pub moving_left: bool,
    pub moving_right: bool,
    pub moving_up: bool,
    pub moving_down: bool,
    pub rotating_left: bool,
    pub rotating_right: bool,
    pub rotating_up: bool,
    pub rotating_down: bool,
}

impl CameraState {
    pub fn new() -> Self {
        CameraState {
            moving_forward: false,
            moving_backward: false,
            moving_left: false,
            moving_right: false,
            moving_up: false,
            moving_down: false,
            rotating_left: false,
            rotating_right: false,
            rotating_up: false,
            rotating_down: false,
        }
    }
}

// private function
fn handle_key_event(camera_state: &Arc<RwLock<CameraState>>, key_code: KeyCode, pressed: bool) {
    let mut camera_state = camera_state.write().unwrap();
    match key_code {
        KeyCode::KeyW => camera_state.moving_forward = pressed,
        KeyCode::KeyS => camera_state.moving_backward = pressed,
        KeyCode::KeyD => camera_state.moving_right = pressed,
        KeyCode::KeyA => camera_state.moving_left = pressed,
        KeyCode::KeyE => camera_state.moving_up = pressed,
        KeyCode::KeyQ => camera_state.moving_down = pressed,
        KeyCode::ArrowLeft => camera_state.rotating_left = pressed,
        KeyCode::ArrowRight => camera_state.rotating_right = pressed,
        KeyCode::ArrowUp => camera_state.rotating_up = pressed,
        KeyCode::ArrowDown => camera_state.rotating_down = pressed,
        _ => (),
    }
}

pub fn handle_motion(camera: &Arc<RwLock<Camera>>, camera_state: &Arc<RwLock<CameraState>>) {
    let mut camera = camera.write().unwrap();
    let camera_state = camera_state.write().unwrap();
    if camera_state.moving_forward {
        camera.move_forward(0.1);
    }
    if camera_state.moving_backward {
        camera.move_forward(-0.1);
    }
    if camera_state.moving_right {
        camera.move_right(0.1);
    }
    if camera_state.moving_left {
        camera.move_right(-0.1);
    }
    if camera_state.moving_up {
        camera.move_up(0.1);
    }
    if camera_state.moving_down {
        camera.move_up(-0.1);
    }
    if camera_state.rotating_left {
        camera.rotate_horizontal(0.1);
    }
    if camera_state.rotating_right {
        camera.rotate_horizontal(-0.1);
    }
    if camera_state.rotating_up {
        camera.rotate_vertical(0.1);
    }
    if camera_state.rotating_down {
        camera.rotate_vertical(-0.1);
    }
}

pub fn handle_camera_motion(event: &WindowEvent, camera: &Arc<RwLock<Camera>>, last_cursor_pos: &mut Option<(f64, f64)>, right_mouse_button_pressed: &Arc<RwLock<bool>>, camera_state: &Arc<RwLock<CameraState>>) {
        match event {
            WindowEvent::CursorMoved { position, .. } => {
                let (x, y) = (position.x, position.y);
                if let Some((last_x, last_y)) = last_cursor_pos {
                    let dx = x - *last_x;
                    let dy = y - *last_y;

                    // right button motion
                    if *right_mouse_button_pressed.write().unwrap() {
                        let mut camera = camera.write().unwrap();
                        // if glium::winit::event::ElementState::Pressed == glium::winit::event::ElementState::Pressed {
                            camera.rotate_horizontal(dx as f32 * 0.1);
                            camera.rotate_vertical(dy as f32 * -0.1);
                        // }
                    }

                    
                }
                *last_cursor_pos = Some((x, y));
            }
            WindowEvent::MouseInput { state, button, .. } => {
                if *button == MouseButton::Right {
                    let mut right_pressed = right_mouse_button_pressed.write().unwrap();
                    *right_pressed = *state == ElementState::Pressed;
                }
            }
            WindowEvent::MouseWheel { delta, .. } => {
                let mut camera = camera.write().unwrap();
                match delta {
                    glium::winit::event::MouseScrollDelta::LineDelta(_, y) => {
                        camera.move_in_direction(*y as f32 * 0.1);
                    }
                    glium::winit::event::MouseScrollDelta::PixelDelta(pos) => {
                        camera.move_in_direction(pos.y as f32 * 0.1);
                    }
                }
            }
            WindowEvent::KeyboardInput {
                event:
                    KeyEvent {
                        physical_key: PhysicalKey::Code(key_code),
                        state,
                        repeat: false,
                        
                        ..
                    },
                ..
            } => {
                let pressed = state == &ElementState::Pressed;
                handle_key_event(&camera_state, *key_code, pressed);
                
            }
                _ => (),
            }
}