# Program Author:

-   Name: Ethan Abraham
-   Student Number: 101324864

## **Dynamic Participant Flyer Simulation**  
A C Programming Assignment  

### **Overview**  
This project implements a dynamic simulation of participants and a flyer in a 2D world. The program leverages modular C design with distinct source files for various functionalities like movement, hero control, and escape logic. It's a part of an academic assignment focusing on multi-file C programming and modular design principles.

### **Features**  
- **Dynamic World Simulation**: Tracks and updates the state of participants and flyers.  
- **Modular Design**: Each functionality is separated into reusable and readable modules.  
- **Optimized for Performance**: Built with efficiency in handling real-time updates.  
- **Error Handling**: Designed to manage edge cases gracefully.  

### **File Structure**  
- **`connect.c`**: Handles the connection and interactions between modules.  
- **`defs.h`**: Defines shared constants, macros, and data structures.  
- **`escape.c`**: Implements escape logic for participants in the simulation.  
- **`flyer.c`**: Manages the behavior and state of the flyer.  
- **`hero.c`**: Logic for the "hero" character's interaction within the simulation.  
- **`hollow.c`**: Implements hollow zone logic, possibly a part of environmental constraints.  
- **`main.c`**: Entry point of the program; initializes and runs the simulation loop.  
- **`participant.c`**: Handles participant movement, states, and updates.  
- **`view.c`**: Manages the visual output of the simulation.   

### **Acknowledgments**  
This project was developed as part of a programming course at Carleton University.

# Compilation and launch Instructions:

## Linking and Compilation:

-   To link and compile, enter `make` into a terminal open in the source file directory.

## Launch:

-   To launch the **server** version of the compiled code, enter `./a5` into the same terminal in the same directory.
-   To launch the **client** version of the compiled code, enter `./a5 {ip-address of server}` into another terminal in the same directory.
    -   The IP address if run locally, is 127.0.0.1
