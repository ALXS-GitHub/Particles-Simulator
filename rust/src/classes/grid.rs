use nalgebra::Vector3;
use std::collections::HashMap;
use std::sync::{Arc, RwLock};
use crate::classes::particles::sphere::Sphere;

#[derive(Hash, Eq, PartialEq, Debug, Clone, Copy)]
pub struct IVec3 {
    x: i32,
    y: i32,
    z: i32,
}

impl From<Vector3<f32>> for IVec3 {
    fn from(vec: Vector3<f32>) -> Self {
        IVec3 {
            x: vec.x.floor() as i32,
            y: vec.y.floor() as i32,
            z: vec.z.floor() as i32,
        }
    }
}

pub struct Grid {
    cell_size: f32,
    pub grid: HashMap<IVec3, Vec<Arc<RwLock<Sphere>>>>,
}

impl Grid {
    pub fn new(cell_size: f32) -> Self {
        Grid {
            cell_size,
            grid: HashMap::new(),
        }
    }

    fn get_cell(&self, position: Vector3<f32>) -> IVec3 {
        IVec3::from(position / self.cell_size)
    }

    pub fn insert(&mut self, sphere: &Arc<RwLock<Sphere>>) {
        let position = sphere.write().unwrap().particle.position;
        let cell = self.get_cell(position);
        self.grid.entry(cell).or_insert_with(Vec::new).push(sphere.clone());
    }

    pub fn clear(&mut self) {
        self.grid.clear();
    }

    // pub fn get_neighbors(&self, sphere: Arc<RwLock<Sphere>>) -> Vec<Arc<RwLock<Sphere>>> {
        // let position = sphere.lock().unwrap().particle.position;
        // let cell = self.get_cell(position);
        // self.get_neighbors_in_cell(cell, Some(sphere))
    // }

    pub fn get_neighbors_in_cell(&self, cell: IVec3) -> Vec<Arc<RwLock<Sphere>>> {
        let mut neighbors = Vec::new();
        for x in -1..=1 {
            for y in -1..=1 {
                for z in -1..=1 {
                    let neighbor_cell = IVec3 {
                        x: cell.x + x,
                        y: cell.y + y,
                        z: cell.z + z,
                    };
                    if let Some(temp) = self.grid.get(&neighbor_cell) {
                        for sphere in temp {
                            neighbors.push(sphere.clone());
                        }
                    }
                }
            }
        }
        neighbors
    }
}