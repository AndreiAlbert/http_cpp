# C++ HTTP Server

A lightweight HTTP server implemented in C++17, inspired by Apache, with support for `GET` requests, directory listing, file serving, and custom MIME type handling. This project is designed for Linux and uses `make` for building.

## Features

- **GET Requests**: Supports basic `GET` requests for serving files.
- **Directory Listing**: Displays an HTML page listing files and directories if a directory is requested.
- **File Serving with MIME Types**: Dynamically serves files with the correct MIME types.
- **Configuration File**: Easily configurable port and root directory.
- **Logging**: Logs server activities (requests, errors) a log file

## Requirements

- **C++17** or later
- **Linux** environment
- **Make** for building

## Getting Started

### 1. Clone the Repository

```bash
git clone https://github.com/yourusername/your-repository-name.git
cd your-repository-name
```
### 2. Configuration

Create a configuration file (`config.ini`) in the root directory of your project with the following structure:

port = 8080        # Specify the port the server will listen on
root = ~/my_web_root   # Root directory for serving files

- **`port`**: The port number the server will listen on.
- **`root`**: The root directory of the server. This can be an absolute path or start with `~` to represent the user’s home directory.

### 3. Build the Project

To compile the project, use the following command:

```bash
make
```

### 4. Run the server
```bash
./bin/main
```
