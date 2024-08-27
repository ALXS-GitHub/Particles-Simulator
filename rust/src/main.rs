// importing libraries
use glium::winit::event::WindowEvent;
use glium::Surface;
use nalgebra::{Vector, Vector3};
use rayon::ThreadPoolBuilder;
use std::sync::{Arc, RwLock};
use std::thread::sleep;
use std::time::{Duration, Instant};

// importing modules
pub mod classes;
mod config;
mod cmd;
pub mod utils;

use classes::container::ContainerTrait;
use classes::particles::sphere::{self, Sphere};
use classes::simulation::Simulation;
use classes::{camera, container, mesh, renderer};

use crate::config::*;
use crate::cmd::*;

use utils::camera_utils::{handle_camera_motion, handle_motion, CameraState};
use utils::key_events::{handle_key_events, process_key_events, KeyState};
use utils::drag_particles::{self, DragParticles, DragState};

pub const TARGET_FPS: i32 = 60;
pub const NUM_SUBSTEPS: i32 = 8;
pub const ADD_PARTICLE_NUM: i32 = 10;

fn main() {
    // Set the number of threads to 4
    ThreadPoolBuilder::new()
        .num_threads(1)
        .build_global()
        .unwrap();

    // window and glium config
    let config_template_builder = glutin::config::ConfigTemplateBuilder::new()
        .with_multisampling(1) // antialiasing
        .with_transparency(true);

    // create window
    let event_loop = glium::winit::event_loop::EventLoop::builder()
        .build()
        .expect("event loop building");
    let (window, display) = glium::backend::glutin::SimpleWindowBuilder::new()
        .with_inner_size(WINDOW_WIDTH as u32, WINDOW_HEIGHT as u32)
        .with_config_template_builder(config_template_builder)
        .build(&event_loop);

    // depth and backface culling
    let params = glium::DrawParameters {
        depth: glium::Depth {
            test: glium::DepthTest::IfLess,
            write: true,
            ..Default::default()
        },
        backface_culling: glium::draw_parameters::BackfaceCullingMode::CullClockwise,
        blend: glium::Blend {
            color: glium::BlendingFunction::Addition {
                source: glium::LinearBlendingFactor::SourceAlpha,
                destination: glium::LinearBlendingFactor::OneMinusSourceAlpha,
            },
            alpha: glium::BlendingFunction::Addition {
                source: glium::LinearBlendingFactor::SourceAlpha,
                destination: glium::LinearBlendingFactor::OneMinusSourceAlpha,
            },
            constant_value: (0.0, 0.0, 0.0, 0.0),
        },
        ..Default::default()
    };

    // simulation components
    let renderer = renderer::Renderer::new(&display).unwrap();
    let camera = camera::Camera::new(
        Vector3::new(0.0, 0.0, 15.0),
        Vector3::new(0.0, 0.0, -1.0),
        Vector3::new(0.0, 1.0, 0.0),
    );
    let mut camera = Arc::new(RwLock::new(camera));
    let camera_state = Arc::new(RwLock::new(CameraState::new()));

    let key_state = Arc::new(RwLock::new(KeyState::new()));

    let mut sphereMesh =
        mesh::Mesh::new(&display, "../models/sphere_ico_low.obj", true, true, false);
    let mut cubeContainerMesh = mesh::Mesh::new(&display, "../models/cube.obj", true, true, true);
    let mut sphereContainerMesh =
        mesh::Mesh::new(&display, "../models/sphere.obj", true, true, true);
    let mut linkMesh = mesh::Mesh::new(&display, "../models/cylinder.obj", true, false, true);

    let mut sim = Simulation::new();
    let mut cmd = Cmd {
        sim: &mut sim,
        world_file: String::from("")
    };

    let mut drag_particles = DragParticles::new(true);
    let mut drag_state = Arc::new(RwLock::new(DragState::new()));

    // sim.create_cube_container(Vector3::new(0.0, 0.0, 0.0), Vector3::new(10.0, 10.0, 10.0), true);
    // sim.create_sphere_container(Vector3::new(0.0, 0.0, 0.0), 5.0, true);
    // sim.create_sphere(
    //     Vector3::new(0.0, 0.0, 0.0),
    //     0.15,
    //     Vector3::new(0.0, 0.0, 0.0),
    //     Vector3::new(0.0, 0.0, 0.0),
    //     false,
    // );
    // sim.create_sphere(
    //     Vector3::new(4.0, 0.0, 0.0),
    //     0.15,
    //     Vector3::new(0.0, 0.0, 0.0),
    //     Vector3::new(0.0, 0.0, 0.0),
    //     false,
    // );

    // sim.load_molecule(
    //     String::from("../data/icosphere.json"),
    //     Vector3::new(0.0, 0.0, 0.0),
    // );
    // sim.load_world(String::from("../data/world_test.json"));
    // cmd.world_file_command(String::from("../data/world2.json"));

    let args: Vec<String> = std::env::args().collect();
    cmd.parse(args);

    let mut last_cursor_pos: Option<(f64, f64)> = None;
    let right_mouse_button_pressed = Arc::new(RwLock::new(false));

    let mut last_update_time = Instant::now();
    let dt = 0.1;

    // event loop
    #[allow(deprecated)]
    let _ = event_loop.run(move |event, window_target| {
        static mut LAST_FRAME_TIME: Option<Instant> = None;
        static mut FRAME_COUNT: u32 = 0;
        static mut FPS: f32 = 0.0;

        let now = Instant::now();
        let elapsed = now.duration_since(last_update_time);

        if elapsed >= Duration::from_secs_f64(1.0 / TARGET_FPS as f64) {
            unsafe {
                if let Some(last_frame_time) = LAST_FRAME_TIME {
                    let duration = now.duration_since(last_frame_time);
                    FRAME_COUNT += 1;
                    if duration >= Duration::from_secs(1) {
                        FPS = FRAME_COUNT as f32 / duration.as_secs_f32();
                        FRAME_COUNT = 0;
                        LAST_FRAME_TIME = Some(now);
                        window.set_title(&format!(
                            "FPS: {:.2}, NUM_PARTICLES: {}",
                            FPS,
                            sim.spheres.len()
                        ));
                    }
                } else {
                    LAST_FRAME_TIME = Some(now);
                }
            }

            let dt = elapsed.as_secs_f32();
            let substep_dt = dt / NUM_SUBSTEPS as f32;

            if !key_state.write().unwrap().pause_simulation {
                for _ in 0..NUM_SUBSTEPS {
                    sim.check_grid_collisions();
                    sim.maintain_molecules();
                    sim.add_force(Vector3::new(0.0, -10.0, 0.0));
                    sim.step(substep_dt);
                }
            }

            let mut frame = display.draw();
            frame.clear_color_and_depth((0.53, 0.81, 0.92, 1.0), 1.0);

            renderer.draw(
                &display,
                &mut frame,
                &*camera.write().unwrap(),
                &sim.spheres,
                &mut sphereMesh,
                &params,
            );

            renderer.draw_molecule_links(
                &display,
                &mut frame,
                &*camera.write().unwrap(),
                &sim.molecules,
                &mut linkMesh,
                &params,
            );

            // in case we need to separate cube and sphere containers, we should use this to pass it to the draw_container method
            let container_trait_vec: Vec<Arc<RwLock<dyn ContainerTrait>>> = sim
                .cube_containers
                .iter()
                .map(|container| container.clone() as Arc<RwLock<dyn ContainerTrait>>)
                .collect();

            renderer.draw_container(
                &display,
                &mut frame,
                &*camera.write().unwrap(),
                &container_trait_vec,
                &mut cubeContainerMesh,
                &params,
            );

            // in case we need to separate cube and sphere containers, we should use this to pass it to the draw_container method
            let container_trait_vec: Vec<Arc<RwLock<dyn ContainerTrait>>> = sim
                .sphere_containers
                .iter()
                .map(|container| container.clone() as Arc<RwLock<dyn ContainerTrait>>)
                .collect();

            renderer.draw_container(
                &display,
                &mut frame,
                &*camera.write().unwrap(),
                &container_trait_vec,
                &mut sphereContainerMesh,
                &params,
            );

            frame.finish().unwrap();

            handle_motion(&camera, &camera_state);

            process_key_events(&key_state, &mut sim);

            drag_particles.process_drag(&camera, &mut sim, &drag_state);

            // println!("Internal pressure of the first molecule: {}", sim.molecules[0].write().unwrap().internal_pressure);

            last_update_time = now;
        }

        match event {
            glium::winit::event::Event::WindowEvent { event, .. } => {
                handle_camera_motion(
                    &event,
                    &camera,
                    &mut last_cursor_pos,
                    &right_mouse_button_pressed,
                    &camera_state,
                );
                handle_key_events(&event, &key_state);
                drag_particles.handle_drag_controls(
                    &event,
                    &mut last_cursor_pos,
                    &drag_state
                );
                match event {
                    glium::winit::event::WindowEvent::CloseRequested => window_target.exit(),
                    // We now need to render everyting in response to a RedrawRequested event due to the animation
                    glium::winit::event::WindowEvent::RedrawRequested => {
                        // nothing here, it has been moved up in the timed section
                    }
                    // Because glium doesn't know about windows we need to resize the display
                    // when the window's size has changed.
                    glium::winit::event::WindowEvent::Resized(window_size) => {
                        display.resize(window_size.into());
                    }
                    _ => (),
                };
            }
            // By requesting a redraw in response to a AboutToWait event we get continuous rendering.
            // For applications that only change due to user input you could remove this handler.
            glium::winit::event::Event::AboutToWait => {
                window.request_redraw();
            }
            _ => (),
        };
    });
}
