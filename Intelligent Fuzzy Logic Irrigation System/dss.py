import json

from utils.weather import get_weather_data
from utils.rules import evaluate_rules
from utils.fuzzifier import fuzzy_values
from constants import MAX_WATERING_DURATION_MIN, WATERING_FACTORS

# simulated input for ground saturation from soil moisture sensor
SIMULATED_GROUND_SATURATION = 10.0  # percentage


def main():
    # get input
    ground_saturation = SIMULATED_GROUND_SATURATION
    weather_data = get_weather_data()
    weather_data["ground_saturation"] = ground_saturation

    # get membership values
    print("Weather Data:", weather_data)
    # fuzzification
    fuzzy_values_result = fuzzy_values(weather_data)

    # rule evaluation
    outputs = evaluate_rules(fuzzy_values_result)
    print("Fuzzy Values:", outputs)

    # defuzzification
    # find continuous output from fuzzy sets
    watering_levels = ["none", "short", "medium", "long"]
    watering_strengths = [outputs[level] for level in watering_levels]
    max_strength = max(watering_strengths)
    if max_strength == 0:
        watering_duration = 0
    else:
        # get the level with the highest strength
        best_level = watering_levels[watering_strengths.index(max_strength)]
        watering_duration = (
            WATERING_FACTORS[best_level] * MAX_WATERING_DURATION_MIN
        )

    # output result
    print(f"Recommended Watering Duration: {watering_duration:.2f} minutes")
    print("Detailed Outputs:", json.dumps(outputs, indent=2))
    with open("./output/output.json", "w") as f:
        json.dump(
            {
                "weather_data": weather_data,
                "fuzzy_values": fuzzy_values_result,
                "outputs": outputs,
                "recommended_watering_duration_min": watering_duration,
            },
            f,
            indent=2,
        )


if __name__ == "__main__":
    main()
