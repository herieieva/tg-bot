# To do telegram bot

## Getting started
If you're building locally, make sure all necessary tools are installed. To do this, run the script:

```bash
sudo ./ubuntu_env.sh
```

## Setting up
Before working with the project, it is necessary to install the configuration:
```bash
conan config install conan_config/
```

## Building
The project is built using the following script:
```bash
conan build . --output-folder=. --build=missing -pr=default
```
pr - this is conan configuration. 
For debugging builds, use "default".
For release builds, use "build"

## Local debug

### Clion
If you're developing in the CLion environment, to configure it, you need to go to File -> Settings -> Build, Execution, Deployment -> CMake and choose the profile you need (conan-release, conan-debug).
