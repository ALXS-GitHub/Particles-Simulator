use nalgebra::Vector3;
use serde::Deserialize;
use serde_json::Value;
use std::fs::File;
use std::io::BufReader;
use std::sync::{Arc, RwLock};

// TODO implement the Molecule structure later

use crate::classes::container::{Container, ContainerTrait};
use crate::classes::containers::cube_container::CubeContainer;
use crate::classes::containers::sphere_container::SphereContainer; // TODO implement the SphereContainer structure later
use crate::classes::molecule::{Molecule, Link};
use crate::classes::particle::ParticleTrait;
use crate::classes::particles::sphere::Sphere;

pub fn parse_sphere(j: &Value) -> Arc<RwLock<Sphere>> {
    let position = Vector3::new(j["position"][0].as_f64().unwrap() as f32, j["position"][1].as_f64().unwrap() as f32, j["position"][2].as_f64().unwrap() as f32);
    let radius = j["radius"].as_f64().unwrap() as f32;
    let velocity = j.get("velocity").and_then(|v| Some(Vector3::new(v[0].as_f64().unwrap() as f32, v[1].as_f64().unwrap() as f32, v[2].as_f64().unwrap() as f32))).unwrap_or(Vector3::new(0.0, 0.0, 0.0));
    let acceleration = j.get("acceleration").and_then(|a| Some(Vector3::new(a[0].as_f64().unwrap() as f32, a[1].as_f64().unwrap() as f32, a[2].as_f64().unwrap() as f32))).unwrap_or(Vector3::new(0.0, 0.0, 0.0));
    let fixed = j.get("fixed").map(|f| f.as_bool().unwrap()).unwrap_or(false);

    Arc::new(RwLock::new(Sphere::new(
        position,
        radius,
        velocity,
        acceleration,
        fixed,
    )))
}

pub fn parse_molecule(j: &Value) -> Arc<RwLock<Molecule>> {
    let offset = j.get("offset").and_then(|o| Some(Vector3::new(o[0].as_f64().unwrap() as f32, o[1].as_f64().unwrap() as f32, o[2].as_f64().unwrap() as f32))).unwrap_or(Vector3::new(0.0, 0.0, 0.0));

    let j = if let Some(path) = j.get("path") {
        let file = File::open(path.as_str().unwrap()).expect("Could not open file");
        let reader = BufReader::new(file);
        serde_json::from_reader(reader).expect("Could not parse JSON file")
    } else {
        j.clone()
    };

    let internal_pressure = j.get("internalPressure").and_then(|ip| ip.as_f64()).map(|ip| ip as f32);
    let use_internal_pressure = j.get("internalPressure").and_then(|uip| uip.as_bool());

    let mut molecule = Molecule::new(
        Some(j["distance"].as_f64().unwrap() as f32),
        Some(j["linksEnabled"].as_bool().unwrap()),
        Some(j["strength"].as_f64().unwrap() as f32),
        internal_pressure,
        use_internal_pressure,
        );

        
    let mut spheres = vec![];

    for j_sphere in j["spheres"].as_array().unwrap() {
        let sphere = parse_sphere(j_sphere);
        let mut sphere_write = sphere.write().unwrap();
        sphere_write.add_offset(offset);
        spheres.push(sphere.clone());
        molecule.spheres.push(sphere.clone());
    }

    for j_link in j["links"].as_array().unwrap() {
        let link: Link = (
            spheres[j_link[0].as_u64().unwrap() as usize].clone(),
            spheres[j_link[1].as_u64().unwrap() as usize].clone()
        );

        molecule.links.push(link);
    }

    Arc::new(RwLock::new(molecule))
}

pub fn parse_container(j: &Value) -> Arc<RwLock<dyn ContainerTrait>> {
    let container_type = j["type"].as_str().unwrap().to_string();
    let position = Vector3::new(j["position"][0].as_f64().unwrap() as f32, j["position"][1].as_f64().unwrap() as f32, j["position"][2].as_f64().unwrap() as f32);
    let forced_inside = j.get("forcedInside").and_then(|fi| fi.as_bool()).unwrap_or(false);

    let container: Arc<RwLock<dyn ContainerTrait>> = if container_type == "cube" {
        let size = Vector3::new(
            j["size"][0].as_f64().unwrap() as f32,
            j["size"][1].as_f64().unwrap() as f32,
            j["size"][2].as_f64().unwrap() as f32,
        );
        Arc::new(RwLock::new(CubeContainer::new(position, size, forced_inside)))
    }
    else if container_type == "sphere" {
        let radius = j["radius"].as_f64().unwrap() as f32;
        Arc::new(RwLock::new(SphereContainer::new(position, Vector3::new(radius, radius, radius) * 2.0, forced_inside)))
    } 
    else {
        panic!("Unknown container type: {}", container_type);
    };

    container
}