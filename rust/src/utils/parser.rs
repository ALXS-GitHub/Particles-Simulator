use nalgebra::Vector3;
use serde::Deserialize;
use serde_json::Value;
use std::fs::File;
use std::io::BufReader;
use std::sync::Arc;

// TODO implement the Molecule structure later

use crate::classes::container::{Container, ContainerTrait};
use crate::classes::containers::cube_container::CubeContainer;
// use crate::classes::containers::sphere_container::SphereContainer; // TODO implement the SphereContainer structure later
// use crate::classes::molecule::Molecule;
use crate::classes::particles::sphere::Sphere;

fn parse_sphere(j: &Value) -> Arc<Sphere> {
    let position = Vector3::new(j["position"][0].as_f64().unwrap() as f32, j["position"][1].as_f64().unwrap() as f32, j["position"][2].as_f64().unwrap() as f32);
    let radius = j["radius"].as_f64().unwrap() as f32;
    let velocity = j.get("velocity").map(|v| Vector3::new(v[0].as_f64().unwrap() as f32, v[1].as_f64().unwrap() as f32, v[2].as_f64().unwrap() as f32)).unwrap_or(Vector3::new(0.0, 0.0, 0.0));
    let acceleration = j.get("acceleration").map(|a| Vector3::new(a[0].as_f64().unwrap() as f32, a[1].as_f64().unwrap() as f32, a[2].as_f64().unwrap() as f32)).unwrap_or(Vector3::new(0.0, 0.0, 0.0));
    let fixed = j.get("fixed").map(|f| f.as_bool().unwrap()).unwrap_or(false);

    Arc::new(Sphere::new(
        position,
        radius,
        velocity,
        acceleration,
        fixed,
    ))
}

// fn parse_molecule(j: &Value) -> Arc<Molecule> {
//     let offset = j.get("offset").map(|o| Vector3::new(o[0].as_f64().unwrap() as f32, o[1].as_f64().unwrap() as f32, o[2].as_f64().unwrap() as f32)).unwrap_or(Vector3::new(0.0, 0.0, 0.0));

//     let j = if let Some(path) = j.get("path") {
//         let file = File::open(path.as_str().unwrap()).expect("Could not open file");
//         let reader = BufReader::new(file);
//         serde_json::from_reader(reader).expect("Could not parse JSON file")
//     } else {
//         j.clone()
//     };

//     let internal_pressure = j.get("internalPressure").map(|ip| ip.as_f64().unwrap() as f32).unwrap_or(0.001);
//     let use_internal_pressure = j.get("internalPressure").is_some();

//     let mut molecule = Molecule {
//         distance: j["distance"].as_f64().unwrap() as f32,
//         links_enabled: j["linksEnabled"].as_bool().unwrap(),
//         strength: j["strength"].as_f64().unwrap() as f32,
//         internal_pressure: Some(internal_pressure),
//         spheres: vec![],
//         links: vec![],
//     };

//     let mut spheres = vec![];

//     for j_sphere in j["spheres"].as_array().unwrap() {
//         let mut sphere = parse_sphere(j_sphere);
//         sphere.position += offset;
//         spheres.push(sphere.clone());
//         molecule.spheres.push(j_sphere.clone());
//     }

//     for j_link in j["links"].as_array().unwrap() {
//         molecule.links.push([j_link[0].as_u64().unwrap() as usize, j_link[1].as_u64().unwrap() as usize]);
//     }

//     Arc::new(molecule)
// }

fn parse_container(j: &Value) -> Arc<dyn ContainerTrait> {
    let container_type = j["type"].as_str().unwrap().to_string();
    let position = Vector3::new(j["position"][0].as_f64().unwrap() as f32, j["position"][1].as_f64().unwrap() as f32, j["position"][2].as_f64().unwrap() as f32);
    let forced_inside = j.get("forcedInside").map(|fi| fi.as_bool().unwrap()).unwrap_or(false);

    let container: Arc<dyn ContainerTrait> = if container_type == "cube" {
        let size = Vector3::new(
            j["size"][0].as_f64().unwrap() as f32,
            j["size"][1].as_f64().unwrap() as f32,
            j["size"][2].as_f64().unwrap() as f32,
        );
        Arc::new(CubeContainer::new(position, size, forced_inside))
    }
    // TODO : impl this later
    // else if container_type == "sphere" {
    //     let radius = j["radius"].as_f64().unwrap() as f32;
    //     Container {
    //         container_type,
    //         position,
    //         size: None,
    //         radius: Some(radius),
    //         forced_inside: Some(forced_inside),
    //     }
    // } 
    else {
        panic!("Unknown container type: {}", container_type);
    };

    container
}