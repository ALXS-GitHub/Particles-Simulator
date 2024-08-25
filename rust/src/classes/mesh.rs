extern crate glium;
extern crate nalgebra as na;

use glium::backend::Facade;
use glium::index::PrimitiveType;
use glium::vertex::VertexBuffer;
use glium::{implement_vertex, uniform, Program, Surface, Display, Frame};
use glium::draw_parameters::{DrawParameters};
use glutin::surface::{WindowSurface};
use na::{Vector2, Vector3};
use std::path::Path;
use std::ffi::CString;
use std::fs::File;
use std::io::{BufRead, BufReader};

use crate::classes::camera::{Camera};
use crate::config::{GLuint, MAX_PARTICLES, CAMERA_ASPECT_RATIO, CAMERA_FOV, CAMERA_NEAR, CAMERA_FAR};
use crate::classes::renderer::{InstancePosVertex, InstanceScaleVertex};

#[derive(Copy, Clone)]
struct Vertex {
    position: [f32; 3],
    normal: [f32; 3],
    tex_coords: [f32; 2],
}
implement_vertex!(Vertex, position, normal, tex_coords);

#[derive(Copy, Clone, Debug)]
pub struct SubVboVertexBuffer {
    pub data: [f32; 3],
}
implement_vertex!(SubVboVertexBuffer, data);

#[derive(Copy, Clone)]
pub struct Vec2Buffer {
    pub data: [f32; 2],
}
implement_vertex!(Vec2Buffer, data);

// ! important this has to be the exact names of the shader layouts
#[derive(Copy, Clone)]
pub struct ModelVertex {
    pub aPos: [f32; 3],
    pub aNormal: [f32; 3],
    pub aTexCoords: [f32; 2],
    pub instancePos: [f32; 3],
    pub instanceScale: [f32; 3],
}
implement_vertex!(ModelVertex, aPos, aNormal, aTexCoords, instancePos, instanceScale);

#[derive(Copy, Clone)]
pub struct APosVertex {
    pub aPos: [f32; 3],
}
implement_vertex!(APosVertex, aPos);

#[derive(Copy, Clone)]
pub struct ANormalVertex {
    pub aNormal: [f32; 3],
}
implement_vertex!(ANormalVertex, aNormal);

#[derive(Copy, Clone)]
pub struct ATexCoordsVertex {
    pub aTexCoords: [f32; 2],
}
implement_vertex!(ATexCoordsVertex, aTexCoords);

pub struct Mesh {
    // vao: glium::VertexArrayObject, // * automatically created by glium
    vbo: VertexBuffer<Vertex>,
    vbo_position: VertexBuffer<InstancePosVertex>,
    vbo_scale: VertexBuffer<InstanceScaleVertex>,
    vbo_rot: VertexBuffer<SubVboVertexBuffer>,
    vertices: Vec<Vertex>,
}

impl Mesh {
    pub fn new(display: &Display<WindowSurface>, filename: &str, instanced: bool, single: bool, oriented: bool) -> Self {
        let mut mesh = Mesh {
            vbo: VertexBuffer::empty_dynamic(display, 0).unwrap(),
            vbo_position: VertexBuffer::empty_dynamic(display, 0).unwrap(),
            vbo_scale: VertexBuffer::empty_dynamic(display, 0).unwrap(),
            vbo_rot: VertexBuffer::empty_dynamic(display, 0).unwrap(),
            vertices: Vec::new(),
        };
        mesh.load_from_file(filename);
        // mesh.setup_mesh(display, instanced, single, oriented);
        mesh
    }

    // pub fn create_sub_vbo<F: Facade>(vbo: &mut glium::VertexBuffer<Vector3<f32>>, attribute_index: u32, size: i32, num_per_vertex: i32, stride: i32, pointer: *const std::ffi::c_void, offset: *const std::ffi::c_void, divisor: u32, display: &F) {
    //     let mut data: Vec<SubVboVertexBuffer> = Vec::new();
    //     for i in 0..size {
    //         data.push(SubVboVertexBuffer { data: [0.0, 0.0, 0.0] });
    //     }
    //     glium::VertexBuffer::dynamic(display, &data).unwrap();
    // }

    fn extract_positions(&self) -> Vec<InstancePosVertex> {
        self.vertices.iter().map(|v| InstancePosVertex { instancePos: v.position }).collect()
    }

    fn extract_scales(&self) -> Vec<InstanceScaleVertex> {
        self.vertices.iter().map(|v| InstanceScaleVertex { instanceScale: v.normal }).collect()
    }

    // fn extract_rotations(&self) -> Vec<SubVboVertexBuffer> {
    //     self.vertices.iter().map(|v| SubVboVertexBuffer { data: v.tex_coords }).collect()
    // }

    pub fn setup_mesh(&mut self, display: &Display<WindowSurface>, instanced: bool, single: bool, oriented: bool) {
        if instanced {
            let max_particles = MAX_PARTICLES as usize; 

            if !single { // TODO check if usefull in the rust version
                self.vbo_position = VertexBuffer::new(display, &self.extract_positions()).unwrap();
                self.vbo_scale = VertexBuffer::new(display, &self.extract_scales()).unwrap();
                if oriented {
                    // self.vbo_rot = VertexBuffer::new(display, &self.extract_rotations()).unwrap();
                }
            } else {
                self.vbo_position = VertexBuffer::new(display, &self.extract_positions()).unwrap();
                self.vbo_scale = VertexBuffer::new(display, &self.extract_scales()).unwrap();


                if oriented {
                    // self.vbo_rot = VertexBuffer::new(display, &self.extract_rotations()).unwrap();
                }
            }
        }
    }

    pub fn draw(&mut self, shader_program: &Program, camera: &Camera, positions: &[InstancePosVertex], scales: &[InstanceScaleVertex], display: &Display<WindowSurface>, target: &mut Frame, params: &DrawParameters) {

        let uniforms = camera.get_uniforms(CAMERA_FOV, CAMERA_ASPECT_RATIO, CAMERA_NEAR, CAMERA_FAR);

        self.vbo_position = glium::VertexBuffer::new(display, positions).unwrap();
        self.vbo_scale = glium::VertexBuffer::new(display, scales).unwrap();

        // extract position, normal, and tex_coords from vertices
        let aPos = VertexBuffer::new(display, &self.vertices.iter().map(|v | APosVertex { aPos : v.position}).collect::<Vec<_>>()).unwrap();
        let aNormal = VertexBuffer::new(display, &self.vertices.iter().map(|v| ANormalVertex { aNormal : v.normal}).collect::<Vec<_>>()).unwrap();
        let aTexCoords = VertexBuffer::new(display, &self.vertices.iter().map(|v| ATexCoordsVertex { aTexCoords : v.tex_coords}).collect::<Vec<_>>()).unwrap();

        target.draw((&aPos, &aNormal, &aTexCoords, self.vbo_position.per_instance().unwrap(), self.vbo_scale.per_instance().unwrap()), glium::index::NoIndices(PrimitiveType::TrianglesList), shader_program, &uniforms, &params).unwrap();
        // target.draw((&aPos, &aNormal), glium::index::NoIndices(PrimitiveType::TrianglesList), shader_program, &uniforms, &Default::default()).unwrap();

    }

    pub fn draw_oriented<F: Facade>(&self, shader_program: &Program, camera: &Camera, positions: &[Vector3<f32>], scales: &[Vector3<f32>], rotations: &[Vector3<f32>], display: &F) {
        // Function body will be implemented later
        unimplemented!()
    }

    pub fn load_from_file(&mut self, filename: &str) {
        let file = match File::open(filename) {
            Ok(file) => file,
            Err(error) => panic!("Error opening file: {}", error),
        };
        let reader = BufReader::new(file);

        let mut positions = Vec::new();
        let mut tex_coords = Vec::new();
        let mut normals = Vec::new();

        for line in reader.lines() {
            let line = line.expect("Failed to read line");
            let mut parts = line.split_whitespace();
            let prefix = parts.next().unwrap_or("");

            match prefix {
                "v" => {
                    let x: f32 = parts.next().unwrap().parse().unwrap();
                    let y: f32 = parts.next().unwrap().parse().unwrap();
                    let z: f32 = parts.next().unwrap().parse().unwrap();
                    positions.push(Vector3::new(x, y, z));
                }
                "vt" => {
                    let x: f32 = parts.next().unwrap().parse().unwrap();
                    let y: f32 = parts.next().unwrap().parse().unwrap();
                    tex_coords.push(Vector2::new(x, y));
                }
                "vn" => {
                    let x: f32 = parts.next().unwrap().parse().unwrap();
                    let y: f32 = parts.next().unwrap().parse().unwrap();
                    let z: f32 = parts.next().unwrap().parse().unwrap();
                    normals.push(Vector3::new(x, y, z));
                }
                "f" => {
                    let vertex_data: Vec<&str> = parts.collect();
                    for data in vertex_data {
                        let indices: Vec<&str> = data.split('/').collect();
                        let position_index: usize = indices[0].parse().unwrap();
                        let tex_coord_index: usize = indices[1].parse().unwrap();
                        let normal_index: usize = indices[2].parse().unwrap();

                        let vertex = Vertex {
                            position: positions[position_index - 1].into(),
                            tex_coords: tex_coords[tex_coord_index - 1].into(),
                            normal: normals[normal_index - 1].into(),
                        };
                        self.vertices.push(vertex);
                    }
                }
                _ => {}
            }
        }
    }

}