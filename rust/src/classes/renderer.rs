use glium::{Program, Display, VertexBuffer, Surface, Frame};
use glium::index::NoIndices;
use glium::program::ProgramCreationError;
use glium::uniforms::UniformsStorage;
use glium::implement_vertex;
use glium::draw_parameters::{DrawParameters};
use glutin::surface::{WindowSurface};
use std::fs::File;
use std::io;
use std::io::Read;
use std::path::Path;
use std::error::Error;
use std::sync::{Arc, RwLock};

use crate::config::{GLuint, CAMERA_ASPECT_RATIO, CAMERA_FOV, CAMERA_FAR, CAMERA_NEAR};
use crate::classes::particles::sphere::{Sphere};
use crate::classes::container::{ContainerTrait, Container};
use crate::classes::camera::{Camera};
use crate::classes::mesh::{Mesh, SubVboVertexBuffer};

// TODO do the renderer after

#[derive(Copy, Clone)]
struct SphereVertexBuffer {
    position: [f32; 3],
    radius: f32,
}
implement_vertex!(SphereVertexBuffer, position, radius);

#[derive(Copy, Clone)]
pub struct InstancePosVertex {
    pub instancePos: [f32; 3],
}
implement_vertex!(InstancePosVertex, instancePos);

#[derive(Copy, Clone)]
pub struct InstanceScaleVertex {
    pub instanceScale: [f32; 3],
}
implement_vertex!(InstanceScaleVertex, instanceScale);

pub struct Renderer {
    shader_program: Program,
    floor_shader_program: Program,
    // floor_vao: GLuint,
    // floor_vbo: GLuint,
    // floor_texture: GLuint,
    model_shader_program: Program,
    container_shader_program: Program,
    molecule_links_shader_program: Program,
}

impl Renderer {
    pub fn new(display: &Display<WindowSurface>) -> Result<Renderer, Box<dyn Error>> {
        let shader_program = Renderer::create_shader_program_with_geometry(display, "../shaders/vertexShader.glsl", "../shaders/geometryShader.glsl", "../shaders/fragmentShader.glsl")?;
        let floor_shader_program = Renderer::create_shader_program(display, "../shaders/floorVertexShader.glsl", "../shaders/floorFragmentShader.glsl")?;
        let model_shader_program = Renderer::create_shader_program(display, "../shaders/modelVertexShader.glsl", "../shaders/modelFragmentShader.glsl")?;
        let container_shader_program = Renderer::create_shader_program(display, "../shaders/containerVertexShader.glsl", "../shaders/containerFragmentShader.glsl")?;
        let molecule_links_shader_program = Renderer::create_shader_program(display, "../shaders/modelOrientedVertexShader.glsl", "../shaders/modelFragmentShader.glsl")?;

        Ok(Renderer {
            shader_program,
            floor_shader_program,
            model_shader_program,
            container_shader_program,
            molecule_links_shader_program,
        })
    }

    pub fn draw(&self, display: &Display<WindowSurface>, target: &mut Frame, camera: &Camera, spheres: &Vec<Arc<RwLock<Sphere>>>, mesh: &mut Mesh, params: &DrawParameters) {
        let mut positions = Vec::new();
        let mut scales = Vec::new();

        for sphere in spheres {
            let sphere = sphere.write().unwrap();
            positions.push(InstancePosVertex { instancePos: sphere.particle.position.into() });
            scales.push(InstanceScaleVertex { instanceScale: [sphere.radius, sphere.radius, sphere.radius] });
        }
        
        mesh.draw(&self.model_shader_program, camera, &positions, &scales, display, target, params);
    }

    pub fn draw_container(&self, display: &Display<WindowSurface>, target: &mut Frame, camera: &Camera, containers: &Vec<Arc<RwLock<dyn ContainerTrait>>>, mesh: &mut Mesh, params: &DrawParameters) {
        let mut positions = Vec::new();
        let mut scales = Vec::new();

        for container in containers {
            let container = container.write().unwrap();
            positions.push(InstancePosVertex { instancePos: container.get_position().into() });
            scales.push(InstanceScaleVertex { instanceScale: [container.get_size().x, container.get_size().y, container.get_size().z] });
        }
        
        mesh.draw(&self.container_shader_program, camera, &positions, &scales, display, target, params);
    }

    fn load_and_compile_shader<P: AsRef<Path>>(path: P) -> Result<String, Box<dyn Error>> {
        let mut file = match File::open(&path)  {
            Ok(file) => file,
            Err(e) => {
                eprintln!("Failed to open file: {}", path.as_ref().display());
                return Err(Box::new(e));
            }
        };
        let mut src = String::new();
        file.read_to_string(&mut src)?;
        Ok(src)
    }

    fn create_shader_program(display: &Display<WindowSurface>, vertex_shader_file: &str, fragment_shader_file: &str) -> Result<Program, ProgramCreationError> {
        let vertex_shader_src = Renderer::load_and_compile_shader(vertex_shader_file).unwrap();
        let fragment_shader_src = Renderer::load_and_compile_shader(fragment_shader_file).unwrap();

        Program::from_source(display, &vertex_shader_src, &fragment_shader_src, None)
    }

    fn create_shader_program_with_geometry(display: &Display<WindowSurface>, vertex_shader_file: &str, geometry_shader_file: &str, fragment_shader_file: &str) -> Result<Program, ProgramCreationError> {
        let vertex_shader_src = Renderer::load_and_compile_shader(vertex_shader_file).unwrap();
        let fragment_shader_src = Renderer::load_and_compile_shader(fragment_shader_file).unwrap();
        let geometry_shader_src = Renderer::load_and_compile_shader(geometry_shader_file).unwrap();

        Program::from_source(display, &vertex_shader_src, &fragment_shader_src, Some(&geometry_shader_src))
    }

}