# Intelligent Fuzzy Logic Irrigation System 💧

An automated decision support system (DSS) designed to optimize water usage for agricultural irrigation. This project leverages **Fuzzy Logic** to determine the ideal watering duration based on real-time weather conditions and soil moisture levels.

## 🧠 Project Overview

Traditional irrigation systems often rely on simple timers, leading to water waste or under-watering. This system mimics human reasoning by processing imprecise inputs (like "hot" temperature or "dry" soil) to make precise control decisions.

### key Features
*   **Fuzzy Logic Decision Making**: Uses fuzzification, rule evaluation, and defuzzification to calculate outputs.
*   **Weather API Integration**: Fetches real-time weather data (Temperature, Humidity, Precipitation) from Google Weather API.
*   **Decision Support System (DSS)**: Provides actionable recommendations for irrigation duration.
*   **Simulated Sensor Data**: Handles simulated inputs for ground saturation to test various scenarios.

## 🛠️ Technology Stack

*   **Language**: Python 3.x
*   **Core Concepts**: Fuzzy Logic, Artificial Intelligence
*   **External APIs**: Google Weather API

## 📂 System Architecture

### 1. `dss.py` (Main Controller)
The heart of the system. It orchestrates the flow of data:
1.  **Input**: Collects weather data and soil moisture levels.
2.  **Fuzzification**: Converts raw numerical data into fuzzy sets (e.g., Temperature 30°C -> "High").
3.  **Inference**: Applies a set of `If-Then` rules (e.g., "If Soil is Dry AND Temp is Hot THEN Water Duration is Long").
4.  **Defuzzification**: Aggregates the results to produce a crisp value (Control Signal).

### 2. `utils/fuzzifier.py`
Defines the membership functions for various input parameters:
*   **Ground Saturation** (Dry, Moist, Wet)
*   **Temperature** (Cold, Warm, Hot)
*   **Humidity** (Low, Medium, High)

### 3. `utils/rules.py`
Contains the knowledge base of the system. It defines the logic rules that govern the decision-making process.

## 🚀 How to Run

1.  **Prerequisites**:
    *   Python 3.8+ installed.
    *   Install dependencies (if any listed in `requirements.txt`).

2.  **Execution**:
    ```bash
    python dss.py
    ```

3.  **Output**:
    The system will print the gathered weather data, the intermediate fuzzy values, and the final **Recommended Watering Duration** in minutes. Detailed logs are saved to `output/output.json`.
