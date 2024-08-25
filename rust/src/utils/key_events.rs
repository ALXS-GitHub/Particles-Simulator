use glium::{glutin, Display};
use glium::winit::event::{ElementState, Event, MouseButton, WindowEvent, KeyEvent};
use glium::winit::keyboard::{KeyCode, PhysicalKey};
use nalgebra::Vector3;
use std::sync::{Arc, RwLock};

use crate::classes::simulation::Simulation;

pub struct KeyState {
    pub add_particles: bool,
    pub pause_simulation: bool,
    pub attract_particles: bool,
}

impl KeyState {
    pub fn new() -> Self {
        KeyState {
            add_particles: false,
            pause_simulation: false,
            attract_particles: false,
        }
    }
}

fn handle_key_event(key_state: &Arc<RwLock<KeyState>>, key_code: KeyCode, state:&ElementState) {
    let pressed = state == &ElementState::Pressed;
    let mut key_state = key_state.write().unwrap();
    match key_code {
        KeyCode::KeyP => if pressed { key_state.pause_simulation = !key_state.pause_simulation },
        KeyCode::KeyG => key_state.add_particles = pressed,
        KeyCode::KeyT => key_state.attract_particles = pressed,
        _ => (),
    }
}

pub fn process_key_events(key_state: &Arc<RwLock<KeyState>>, sim: &mut Simulation) {
    let mut key_state = key_state.write().unwrap();
    if key_state.add_particles {
        sim.generate_random_particles(9.0);
    }
    if key_state.attract_particles {
        sim.attract_particles_to_origin(10.0);
    }

    // managed outside of this function
    // if key_state.pause_simulation {
    // }
}

pub fn handle_key_events(event: &WindowEvent, key_state: &Arc<RwLock<KeyState>>) {
    match event {
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
            handle_key_event(&key_state, *key_code, state);  
        }
            _ => (),
        }
}