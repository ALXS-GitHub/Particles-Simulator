use clap::{Arg, Command, ArgAction};
use std::sync::{Arc, Mutex};
use std::process;
use crate::classes::simulation::Simulation;

pub struct Cmd<'a> {
    pub sim: &'a mut Simulation,
    pub world_file: String,
}

impl<'a> Cmd<'a> {

    // Setup the simulation object
    fn setup(&mut self, sim: &'a mut Simulation) {
        self.sim = sim;
    }

    // Print help message
    fn print_help(&self) {
        println!("Usage: ./particle_simulator [options]");
        println!("Options:");
        println!("  -h, --help                Print this help message");
        println!("  -w, --world <world_file>  Specify the world file to load");
    }

    // Parse command-line arguments
    pub fn parse(&mut self, args: Vec<String>) {
        let matches = Command::new("particle_simulator")
            .version("1.0")
            .author("Author Name <author@example.com>")
            .about("Particle Simulator")
            .disable_help_flag(true)
            .disable_version_flag(true)
            .arg(Arg::new("help")
                .short('h')
                .long("help")
                .num_args(0..=1)
                .help("Print this help message"))
            .arg(Arg::new("try")
                .short('t')
                .long("try")
                .num_args(0..=1)
                .default_missing_value("val")
                .help("Print this try message"))
            .arg(Arg::new("world")
                .short('w')
                .long("world")
                .value_name("WORLD_FILE")
                .num_args(1..=1)
                .help("Specify the world file to load"))
            .get_matches_from(args);
            

        if matches.contains_id("help") {
            self.print_help();
            process::exit(0);
        }

        if matches.contains_id("world") {
            let file = matches.get_one::<String>("world").unwrap();
            self.world_file = file.to_string();
            println!("World file: {}", self.world_file);
            self.world_file_command(self.world_file.clone());
        } else {
            println!("Warning: No world file specified. Using default world file");
            self.world_file_command(String::from("../data/world_default.json"));
        }

    }

    // Handle the world file command
    pub fn world_file_command(&mut self, file: String) {
        self.sim.load_world(file);

    }
}
