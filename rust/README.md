to launch the rust version :

```bash
cargo run
# or
cargo run --profile bench
```

to make the release version :

```bash
cargo build --release
# then launch the binary in the target/release directory
cd target/release
./ParticlesSimulator.exe
```