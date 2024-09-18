# Credential Stealer

## Description

This program is a simple file stealer written in C++. It checks for the presence of the **AnyDesk** process and, if found, sends a specific configuration file (`service.conf`) to a predefined server via an HTTP POST request.

## Features

- Checks if the **AnyDesk** process is running.
- Verifies the existence of a specified file.
- Sends the file contents to a specified server.

## Prerequisites

- Windows operating system
- Visual Studio or any C++ compiler supporting Windows API

## Compilation

To compile the program, use a C++ compiler that supports the Windows API. For example, using Visual Studio:

1. Create a new project.
2. Copy the provided code into the main source file.
3. Ensure the project links against the `winhttp` library.
4. Build the project.

## Usage

1. Ensure the **AnyDesk** process is running on the target system.
2. Update the `serverUrl` in the `SendFile` function to the desired server address.
3. Execute the compiled program.

```bash
./Stealer.exe
```
## Security Warning
This code is intended for educational purposes only. Unauthorized access to computer systems and data is illegal and unethical. Ensure you have permission before running any software that accesses files or processes on a system.

## License
This project is licensed under the MIT License - see the LICENSE file for details.
