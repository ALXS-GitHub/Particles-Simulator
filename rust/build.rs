use std::env;
use std::fs;
use std::path::PathBuf;

fn main() {

    // Check the build profile
    let profile = env::var("PROFILE").unwrap();
    if profile != "release" {
        return;
    }

    // Get the output directory from the environment variable
    let out_dir = env::var("OUT_DIR").unwrap();
    let out_path = PathBuf::from(out_dir);

    let assets_names = vec![String::from("data"), String::from("models"), String::from("shaders")];

    for name in assets_names {
        // Define the source and destination paths
        let src_asset_path = PathBuf::from(format!("../{}", name));
        let dest_asset_path = out_path.join(format!("../../../{}", name)); // three levels up due to how OUT_DIR is set

        // Create the destination directory if it doesn't exist
        fs::create_dir_all(&dest_asset_path).unwrap();

        // Copy the assets
        for entry in fs::read_dir(src_asset_path).unwrap() {
            let entry = entry.unwrap();
            let path = entry.path();
            if path.is_file() {
                fs::copy(&path, dest_asset_path.join(path.file_name().unwrap())).unwrap();
            }
        }
    }

}