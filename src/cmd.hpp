#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include "classes/simulation.hpp"

using namespace std;

class Cmd {
    
    private:
        Cmd() = delete; // No instance of this class should be created
        ~Cmd() = delete;

        // commands
        static void worldFileCommand(string file);
        static void printHelp();

    public:

        static Simulation* sim; // pointer to the simulation object
        static string worldFile;

        static void setup(Simulation* sim);
        static void parse(int argc, char* argv[]);

};

string Cmd::worldFile = "";
Simulation* Cmd::sim = nullptr;

void Cmd::printHelp() {

    int lineWidth = 40;

    cout << "Usage: ./particle_simulator [options]" << endl;
    cout << "Options:" << endl;
    cout << left << setw(lineWidth) << "  -h, --help" << "Print this help message" << endl;
    // cout << left << setw(lineWidth) << "  -v, --version" << "Print the version of the program" << endl; // TODO: Implement version later
    cout << left << setw(lineWidth) << "  -w, --world <world_file>" << "Specify the world file to load" << endl;
    // cout << left << setw(lineWidth) << "  --gc, --grid-cell-size <size>" << "Specify the size of the grid's cells" << endl; // TODO: Implement grid size later
    // cout << left << setw(lineWidth) << "  --substeps <num>" << "Specify the number of substeps" << endl; // TODO: Implement substeps later
    // cout << left << setw(lineWidth) << "  --threads <num>" << "Specify the number of threads to use" << endl; // TODO: Implement threads later
    // cout << left << setw(lineWidth) << "  --fps <num>" << "Specify the target frames per second" << endl; // TODO: Implement fps later
    // cout << left << setw(lineWidth) << "  --add-particles <num>" << "Specify the number of particles to add" << endl; // TODO: Implement add particles later
}

void Cmd::setup(Simulation* sim) {
    Cmd::sim = sim;
}

void Cmd::parse(int argc, char* argv[]) {

    for (int i = 1; i < argc; i++) {
        string arg = argv[i];
        if (arg == "-h" || arg == "--help") {
            printHelp();
            exit(0);
        } else if (arg == "-w" || arg == "--world") {
            if (i + 1 < argc) {
                worldFile = argv[i + 1];
                cout << "World file: " << worldFile << endl;
                worldFileCommand(worldFile);
                i++;
            } else {
                cerr << "Error: No world file specified" << endl;
                exit(1);
            }
        } else {
            cerr << "Error: Unknown option " << arg << endl;
            exit(1);
        }
    }

    if (worldFile == "") {
        cout << "Warning: No world file specified. Using default world file" << endl;
        worldFileCommand("../data/world_default.json");
    }
}

void Cmd::worldFileCommand(string file) {
    sim->loadWorld(file);
}
