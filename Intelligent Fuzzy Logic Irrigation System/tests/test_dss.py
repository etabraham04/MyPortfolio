import json
import pytest
import numpy as np
from pathlib import Path
import sys

# Add parent directory to path to allow imports
sys.path.insert(0, str(Path(__file__).parent.parent))

from utils.fuzzifier import fuzzy_values
from utils.rules import evaluate_rules


# Load test scenarios
TEST_FILE = Path(__file__).parent / "fuzzy_test_suite.json"


def load_test_scenarios():
    """Load test scenarios from JSON file."""
    with open(TEST_FILE, "r") as f:
        return json.load(f)


@pytest.fixture
def test_scenarios():
    """Pytest fixture to load test scenarios."""
    return load_test_scenarios()


def determine_output_category(outputs):
    """Determine the dominant output category from fuzzy outputs."""
    # Find the output with maximum membership value
    max_key = max(outputs, key=outputs.get)
    max_value = outputs[max_key]

    # If the maximum value is very low (< 0.1), consider it as "None"
    if max_value < 0.1:
        return "None"

    # Return the category with highest membership
    return max_key.capitalize()


@pytest.mark.parametrize(
    "scenario", load_test_scenarios(), ids=lambda s: s["scenario"]
)
def test_fuzzy_system(scenario):
    """Test the fuzzy logic system with various scenarios."""
    name = scenario["scenario"]
    inputs = scenario["inputs"]
    expected = scenario["expected_output"]

    # Run fuzzification
    fuzz_values = fuzzy_values(inputs)

    # Test for ornamentals
    from constants import SOIL_TARGET_OPTIONS
    import constants

    # Test ornamentals
    constants.SOIL_TARGET = SOIL_TARGET_OPTIONS[0]  # "ornamentals"
    ornamental_outputs = evaluate_rules(fuzz_values)
    ornamental_result = determine_output_category(ornamental_outputs)

    # Test vegetables
    constants.SOIL_TARGET = SOIL_TARGET_OPTIONS[1]  # "vegetables"
    vegetable_outputs = evaluate_rules(fuzz_values)
    vegetable_result = determine_output_category(vegetable_outputs)

    # Print detailed results for debugging
    print(f"\n=== {name} ===")
    print(f"Inputs: {inputs}")
    print("\nFuzzy Values:")
    for k, v in fuzz_values.items():
        print(f"  {k}: {v}")
    print(f"\nOrnamentals Output: {ornamental_outputs}")
    print(f"  -> {ornamental_result} (expected: {expected['ornamentals']})")
    print(f"\nVegetables Output: {vegetable_outputs}")
    print(f"  -> {vegetable_result} (expected: {expected['vegetables']})")

    # Assertions
    assert ornamental_result == expected["ornamentals"], (
        f"Ornamentals failed for '{name}': "
        f"got {ornamental_result}, expected {expected['ornamentals']}"
    )

    assert vegetable_result == expected["vegetables"], (
        f"Vegetables failed for '{name}': "
        f"got {vegetable_result}, expected {expected['vegetables']}"
    )


def test_fuzzy_values_output_types(test_scenarios):
    """Test that fuzzy_values returns correct data structure."""
    inputs = test_scenarios[0]["inputs"]
    result = fuzzy_values(inputs)

    # Check that result is a dictionary
    assert isinstance(result, dict)

    # Check that all expected keys are present
    expected_keys = [
        "ground_saturation",
        "day_temp",
        "night_temp",
        "humidity",
        "PoP",
    ]
    assert set(result.keys()) == set(expected_keys)

    # Check that each value is a dictionary with correct membership keys
    for key in expected_keys:
        assert isinstance(result[key], dict)
        values = result[key].values()
        assert all(isinstance(v, (float, np.floating)) for v in values)
        assert all(0 <= v <= 1 for v in result[key].values())


def test_evaluate_rules_output_types(test_scenarios):
    """Test that evaluate_rules returns correct data structure."""
    inputs = test_scenarios[0]["inputs"]
    fuzz_values = fuzzy_values(inputs)
    result = evaluate_rules(fuzz_values)

    # Check that result is a dictionary
    assert isinstance(result, dict)

    # Check that all expected output keys are present
    expected_keys = ["none", "short", "medium", "long"]
    assert set(result.keys()) == set(expected_keys)

    # Check that each value is a numeric value between 0 and 1
    for key in expected_keys:
        assert isinstance(result[key], (int, float, np.floating))
        assert 0 <= result[key] <= 1


if __name__ == "__main__":
    # Run tests with verbose output
    pytest.main([__file__, "-vvv"])
