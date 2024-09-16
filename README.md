![alt text](Sandbox/assets/screenshots/image.png)

# Procedurally animated fish
The fish in this project follows your cursor and is procedurally animated, providing a dynamic interaction.

## Build
1. Run the `build.sh` script. This script will:
    ### For linux
    - include the Deimos engine and fetch dependencies if run on Linux
    - configure and compile the project
    ### For Windows
    - all stays the same, but the Windows setuper is still in development, so you would have to manually install the following dependencies
    * Cmake 
    * Visual Studio with C++ development tools (for compilers)
    ```
    ./build.sh
    ```
 2. Run the project
    ```
    ./build/Sandbox/Sandbox
    ```