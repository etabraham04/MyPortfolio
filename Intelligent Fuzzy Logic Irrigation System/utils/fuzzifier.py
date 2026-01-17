from utils.memberships import (
    ground_saturation_mf,
    day_temperature_mf,
    night_temperature_mf,
    humidity_mf,
    precipitation_probability_mf,
)


def fuzzy_values(inputs):

    return {
        "ground_saturation": ground_saturation_mf(inputs["ground_saturation"]),
        "day_temp": day_temperature_mf(inputs["day_temp"]),
        "night_temp": night_temperature_mf(inputs["night_temp"]),
        "humidity": humidity_mf(inputs["humidity"]),
        "PoP": precipitation_probability_mf(inputs["PoP"]),
    }
