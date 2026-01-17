import json
import sys
import os
from utils.rules import evaluate_rules
from utils.fuzzifier import fuzzy_values
import constants
from constants import MAX_WATERING_DURATION_MIN, WATERING_FACTORS
from pathlib import Path


# simulated input for ground saturation from soil moisture sensor
SIMULATED_GROUND_SATURATION = 10.0  # percentage

# Add parent directory to path to allow imports
sys.path.insert(0, str(Path(__file__).parent.parent))
# Load test scenarios
TEST_FILE = Path(__file__).parent / "tests" / "fuzzy_test_suite.json"

scenario_names = [
    "Global Stop: Cold Day Temp",
    "Global Stop: High Rain Probability",
    "Ornamentals Rule 6: Very Dry, Low Rain Chance",
    "Vegetables Rule 2: Dry, Hot Day, No Rain",
    "Global Stop: Partial Cold Day (7 C)",
    "Warm vs Hot Boundary (33 C Day)",
]


def clear_screen():
    os.system("cls" if os.name == "nt" else "clear")


def print_header(text, width=80):
    print("=" * width)
    print(f" {text.upper()}".center(width))
    print("=" * width)


def print_section(text):
    print(f"\n--- {text} ---")


def print_dict_table(data, title="Data"):
    print(f"\n{title}:")
    print("-" * 40)
    for key, value in data.items():
        key_str = key.replace("_", " ").title()
        print(f"{key_str:<25} : {value}")
    print("-" * 40)


def print_fuzzy_values(data):
    print("\nFuzzy Membership Values:")
    print("-" * 60)
    for category, memberships in data.items():
        cat_str = category.replace("_", " ").title()
        print(f"[{cat_str}]")
        for term, value in memberships.items():
            bar_len = int(value * 20)
            bar = "█" * bar_len
            print(f"  {term:<15} : {value:.2f} |{bar:<20}|")
        print()
    print("-" * 60)


def print_output_chart(outputs):
    print("\nRule Evaluation (Watering Factor):")
    print("-" * 60)
    for level, value in outputs.items():
        bar_len = int(value * 30)
        bar = "█" * bar_len
        print(f"  {level.title():<10} : {value:.2f} |{bar:<30}|")
    print("-" * 60)


def wait_for_user(prompt="Press Enter to continue..."):
    print(f"\n>> {prompt}")
    input()


def main():
    # Load in the test data:
    with open(TEST_FILE, "r") as f:
        scenarios = json.load(f)

    # Check for the specified scenarios
    specified = []
    for name in scenario_names:
        match = next((s for s in scenarios if s["scenario"] == name), None)
        if match:
            specified.append(match)

    output_list = []
    total_scenarios = len(specified)

    clear_screen()
    print_header("Smart Sprinkler System - Logic Demo")
    print(f"\nLoaded {total_scenarios} scenarios for demonstration.")
    wait_for_user("Press Enter to start the demo")

    for i, x in enumerate(specified, 1):
        # Set the soil target based on the scenario name
        if "Vegetables" in x["scenario"]:
            constants.SOIL_TARGET = "vegetables"
        else:
            constants.SOIL_TARGET = "ornamentals"

        clear_screen()
        print_header(f"Scenario {i}/{total_scenarios}")
        print(f"\nTitle: {x['scenario']}")
        print(f"Plant Type: {constants.SOIL_TARGET.title()}")

        # Collect the weather data from that scenario.
        weather_data = x["inputs"]
        print_dict_table(weather_data, "Input Conditions")

        wait_for_user("Press Enter to run Fuzzification...")

        # fuzzification
        fuzzy_values_result = fuzzy_values(weather_data)
        print_fuzzy_values(fuzzy_values_result)

        wait_for_user("Press Enter to evaluate Rules...")

        # rule evaluation
        outputs = evaluate_rules(fuzzy_values_result)
        print_output_chart(outputs)

        # defuzzification
        watering_levels = ["none", "short", "medium", "long"]
        watering_strengths = [outputs[level] for level in watering_levels]
        max_strength = max(watering_strengths)

        if max_strength == 0:
            watering_duration = 0
            best_level = "None"
        else:
            best_level = watering_levels[
                watering_strengths.index(max_strength)
            ]
            watering_duration = (
                WATERING_FACTORS[best_level] * MAX_WATERING_DURATION_MIN
            )

        print_section("Final Decision")
        print(f"Dominant Category : {best_level.upper()}")
        print(f"Watering Duration : {watering_duration:.2f} minutes")

        current_output = {
            "weather_data": weather_data,
            "fuzzy_values": fuzzy_values_result,
            "outputs": outputs,
            "recommended_watering_duration_min": watering_duration,
        }
        output_list.append(current_output)

        if i < total_scenarios:
            wait_for_user("Press Enter for next scenario...")
        else:
            wait_for_user("Press Enter to finish...")

    # Out of the loop
    with open("./output/output.json", "w") as f:
        json.dump(
            output_list,
            f,
            indent=2,
        )

    clear_screen()
    print_header("Demo Completed")
    print("\nResults saved to ./output/output.json")


if __name__ == "__main__":
    main()
