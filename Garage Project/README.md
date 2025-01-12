# Assignment 3: Garage Management System

## Project Overview
This project implements a **Garage Management System** designed to manage entities like cars, customers, mechanics, and repairs. The system is built using C++ with a modular approach, making it easy to maintain and extend. It includes a UML diagram for visualization of the system design.

## File Descriptions
Here is a breakdown of the files included in the project:

### Source Code Files
- **Car.cc / Car.h**: Implements and defines the `Car` class, which represents vehicles managed by the system. This includes attributes like `make`, `model`, and `year`, as well as methods to retrieve or update car details.
- **Control.cc / Control.h**: Manages the overall system workflow and acts as the main controller. This file includes methods for initializing the system, handling user input, and coordinating between various classes.
- **Customer.cc / Customer.h**: Implements and defines the `Customer` class, representing garage clients. Contains attributes such as `name` and `contact information`, and methods to retrieve and update customer data.
- **Entity.cc / Entity.h**: Defines a base class `Entity` for various entities in the system. This file provides shared attributes and methods that are inherited by other classes such as `Car` and `Customer`.
- **Garage.cc / Garage.h**: Implements and defines the `Garage` class to manage the physical garage's operations. Includes methods for storing and retrieving cars, managing mechanics, and scheduling repairs.
- **Mechanic.cc / Mechanic.h**: Represents the `Mechanic` class and their roles in the system. Includes attributes such as `name`, `specialization`, and `availability`, and methods for assigning tasks.
- **Repair.cc / Repair.h**: Defines and handles the `Repair` class, which tracks details of repairs performed in the garage. This includes attributes like `repair ID`, `car details`, and `status`, and methods to update repair progress.
- **RepairList.cc / RepairList.h**: Manages a list of repairs for streamlined operations. Provides methods to add, remove, and retrieve repairs from the list.
- **TestControl.cc / TestControl.h**: Contains unit testing or debugging logic for the system. Includes methods to simulate workflows and validate system performance.
- **Tester.cc / Tester.h**: Additional testing utilities, providing methods to test individual classes and their functionalities.
- **View.cc / View.h**: Handles user interface components for the system. Includes methods for displaying information and gathering user inputs.

### Main Entry and Build Files
- **main.cc**: The main entry point for the system. This file initializes the `Control` class and starts the program loop.
- **Makefile**: Automates the compilation of the project. Contains build rules for compiling all source files and generating the executable.

### Additional Files
- **A3UML.drawio (1).pdf**: UML diagram providing an overview of the class structure and relationships in the system. Helps visualize the interactions and design of the project.
- **README.md**: Existing README file for basic project details.

### Testing Files
- **test.cc**: Contains test cases to validate the system's functionality. Tests cover major components like `Car`, `Customer`, `Mechanic`, and `Repair` classes to ensure they behave as expected.

## Dependencies
- **Compiler**: A C++ compiler such as `g++` (version 9.3.0 or higher recommended).
- **Make**: For using the provided Makefile (version 4.2 or higher recommended).

## How to Compile and Run
1. Ensure `g++` and `make` are installed on your system.
2. Navigate to the project directory.
3. Run the following command to compile the project:
   ```bash
   make
   ```
4. Execute the compiled program:
   ```bash
   ./main
   ```

## Usage
The system allows:
- Adding and managing cars.
- Tracking customers and their repair needs.
- Managing mechanics and assigning repairs.
- Viewing and updating repair statuses.

## UML Diagram
The UML diagram (`A3UML.drawio (1).pdf`) provides a high-level overview of the system's class structure and interactions. It illustrates the relationships between classes like `Car`, `Customer`, `Garage`, `Mechanic`, and `Repair`. This diagram directly corresponds to the implemented codebase, ensuring alignment with the actual system design. Each class in the diagram matches its implementation, and the diagram serves as a blueprint for understanding the flow of operations within the system.

## Testing
Run the following command to execute the test cases:
```bash
./test
```
The test cases ensure that all major functionalities are working as expected. Specifically, the tests include:
- **Car Tests**: Validate the creation, retrieval, and update of car details.
- **Customer Tests**: Ensure customer details, such as names and contact information, can be added, retrieved, and modified correctly.
- **Mechanic Tests**: Test the functionality for assigning and managing mechanics, including availability updates.
- **Repair Tests**: Verify the addition, removal, and status updates of repairs in the system.
- **Integration Tests**: Simulate end-to-end workflows to ensure all components interact as intended.

### Expected Output
When running the tests, the system provides detailed feedback for each test case, indicating whether it passed or failed. For failed tests, error messages specify the issue, making it easier to debug. A summary of the test results is displayed at the end, showing the total number of tests run, passed, and failed.
