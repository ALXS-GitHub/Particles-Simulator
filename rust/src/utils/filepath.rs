pub fn get_path(file_name: &str) -> String {
    if cfg!(debug_assertions) {
        format!("../{}", file_name)
    } else {
        format!("{}", file_name)
    }
}

// todo change according to what is said here : https://users.rust-lang.org/t/conditional-compilation-for-debug-release/1098/6