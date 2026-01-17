import numpy as np
import constants


# Global stop rules
def global_stop_rules(fuzzy_values: dict):
    """Global stop rules based on fuzzy values."""
    rules = []

    # Cold Stop:
    # IF Day Temp is Cold OR Night Temp is Cold THEN Output is None
    rules.append(
        np.maximum(
            fuzzy_values["day_temp"].get("cold", 0),
            fuzzy_values["night_temp"].get("cold", 0),
        )
    )

    # Rain Stop:
    # IF Rain Probability is High Chance THEN Watering Factor is None
    rules.append(fuzzy_values["PoP"].get("high_chance", 0))

    # Saturated Soil Stop:
    # IF Ground Moisture is Wet THEN Watering Factor is None
    rules.append(fuzzy_values["ground_saturation"].get("wet", 0))

    return np.max(rules)


# Ornamental Rules
def ornamental_rules(fuzzy_values: dict):
    outputs = {"none": 0, "short": 0, "medium": 0, "long": 0}

    # Rule 1:
    # IF Ground Moisture is Moist THEN Watering Factor is None
    outputs["none"] = np.maximum(
        outputs["none"], fuzzy_values["ground_saturation"].get("moist", 0)
    )

    # Rule 2:
    # IF Ground Moisture is Very Dry
    # AND (Day Temp is Hot OR Night Temp is Hot)
    # AND Rain Probability is No Rain
    # THEN Watering Factor is Long
    outputs["long"] = np.maximum(
        outputs["long"],
        np.minimum(
            np.minimum(
                fuzzy_values["ground_saturation"].get("very_dry", 0),
                np.maximum(
                    fuzzy_values["day_temp"].get("hot", 0),
                    fuzzy_values["night_temp"].get("hot", 0),
                ),
            ),
            fuzzy_values["PoP"].get("no_rain", 0),
        ),
    )

    # Rule 3:
    # IF Ground Moisture is Very Dry
    # AND (Day Temp is Warm OR Day Temp is Cool)
    # AND Rain Probability is No Rain
    # THEN Watering Factor is Medium
    outputs["medium"] = np.maximum(
        outputs["medium"],
        np.minimum(
            np.minimum(
                fuzzy_values["ground_saturation"].get("very_dry", 0),
                np.maximum(
                    fuzzy_values["day_temp"].get("warm", 0),
                    fuzzy_values["day_temp"].get("cool", 0),
                ),
            ),
            fuzzy_values["PoP"].get("no_rain", 0),
        ),
    )

    # Rule 4:
    # IF Ground Moisture is Dry
    # AND (Day Temp is Hot OR Day Temp is Warm)
    # AND Weather Humidity is Dry
    # THEN Watering Factor is Short
    outputs["short"] = np.maximum(
        outputs["short"],
        np.minimum(
            np.minimum(
                fuzzy_values["ground_saturation"].get("dry", 0),
                np.maximum(
                    fuzzy_values["day_temp"].get("hot", 0),
                    fuzzy_values["day_temp"].get("warm", 0),
                ),
            ),
            fuzzy_values["humidity"].get("dry", 0),
        ),
    )

    # Rule 5:
    # IF Ground Moisture is Dry
    # AND Day Temp is Cool
    # THEN Watering Factor is None
    outputs["none"] = np.maximum(
        outputs["none"],
        np.minimum(
            fuzzy_values["ground_saturation"].get("dry", 0),
            fuzzy_values["day_temp"].get("cool", 0),
        ),
    )

    # Rule 6:
    # IF Ground_Moisture is Very_Dry
    # AND Rain_Probability is Low_Chance
    # THEN Watering_Factor is Short
    outputs["short"] = np.maximum(
        outputs["short"],
        np.minimum(
            fuzzy_values["ground_saturation"].get("very_dry", 0),
            fuzzy_values["PoP"].get("low_chance", 0),
        ),
    )

    return outputs


# Vegetable Rules
def vegetable_rules(fuzzy_values: dict):
    outputs = {"none": 0, "short": 0, "medium": 0, "long": 0}

    # Rule 1:
    # IF Ground Moisture is Very Dry
    # AND Rain Probability is No Rain
    # THEN Watering Factor is Long
    outputs["long"] = np.maximum(
        outputs["long"],
        np.minimum(
            fuzzy_values["ground_saturation"].get("very_dry", 0),
            fuzzy_values["PoP"].get("no_rain", 0),
        ),
    )

    # Rule 2:
    # IF Ground Moisture is Dry
    # AND (Day Temp is Hot OR Night Temp is Hot)
    # AND Rain Probability is No Rain
    # THEN Watering Factor is Long
    outputs["long"] = np.maximum(
        outputs["long"],
        np.minimum(
            np.minimum(
                fuzzy_values["ground_saturation"].get("dry", 0),
                np.maximum(
                    fuzzy_values["day_temp"].get("hot", 0),
                    fuzzy_values["night_temp"].get("hot", 0),
                ),
            ),
            fuzzy_values["PoP"].get("no_rain", 0),
        ),
    )

    # Rule 3:
    # IF Ground Moisture is Dry
    # AND (Day Temp is Warm OR Day Temp is Cool)
    # AND Rain Probability is No Rain
    # THEN Watering Factor is Medium
    outputs["medium"] = np.maximum(
        outputs["medium"],
        np.minimum(
            np.minimum(
                fuzzy_values["ground_saturation"].get("dry", 0),
                np.maximum(
                    fuzzy_values["day_temp"].get("warm", 0),
                    fuzzy_values["day_temp"].get("cool", 0),
                ),
            ),
            fuzzy_values["PoP"].get("no_rain", 0),
        ),
    )

    # Rule 4:
    # IF Ground Moisture is Moist
    # AND (Day Temp is Hot OR Day Temp is Warm)
    # AND Weather Humidity is Dry
    # THEN Watering Factor is Short
    outputs["short"] = np.maximum(
        outputs["short"],
        np.minimum(
            np.minimum(
                fuzzy_values["ground_saturation"].get("moist", 0),
                np.maximum(
                    fuzzy_values["day_temp"].get("hot", 0),
                    fuzzy_values["day_temp"].get("warm", 0),
                ),
            ),
            fuzzy_values["humidity"].get("dry", 0),
        ),
    )

    # Rule 5:
    # IF Ground Moisture is Moist
    # AND Day Temp is Cool
    # THEN Watering Factor is None
    outputs["none"] = np.maximum(
        outputs["none"],
        np.minimum(
            fuzzy_values["ground_saturation"].get("moist", 0),
            fuzzy_values["day_temp"].get("cool", 0),
        ),
    )

    # Rule 6:
    # IF Ground Moisture is Dry
    # AND Rain Probability is Low Chance
    # THEN Watering Factor is Short
    outputs["short"] = np.maximum(
        outputs["short"],
        np.minimum(
            fuzzy_values["ground_saturation"].get("dry", 0),
            fuzzy_values["PoP"].get("low_chance", 0),
        ),
    )

    return outputs


def evaluate_rules(fuzzy_values: dict):
    """Evaluate rules based on plant type and fuzzy values."""
    # Check global stop rules first
    stop_level = global_stop_rules(fuzzy_values)
    if stop_level > 0:
        return {"none": stop_level, "short": 0, "medium": 0, "long": 0}

    # Evaluate plant-specific rules
    if constants.SOIL_TARGET == "ornamentals":
        return ornamental_rules(fuzzy_values)
    elif constants.SOIL_TARGET == "vegetables":
        return vegetable_rules(fuzzy_values)
    else:
        raise ValueError(f"Unknown plant type: {constants.SOIL_TARGET}")
