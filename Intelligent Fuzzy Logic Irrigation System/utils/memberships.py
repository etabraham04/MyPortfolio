"""A module defining fuzzy membership functions"""

import numpy as np


# create membership functions
def triangular_mf(x: float, a: float, b: float, c: float) -> float:
    """Triangular membership function."""
    x = np.asarray(x, dtype=float)

    # Left side
    if b != a:
        left_slope = (x - a) / (b - a)
    else:
        # Vertical edge at a == b: membership is 1 for x >= a
        left_slope = (x >= a).astype(float)

    # Right side
    if c != b:
        right_slope = (c - x) / (c - b)
    else:
        # Vertical edge at b == c: membership is 1 for x <= c
        right_slope = (x <= c).astype(float)

    # Combine and clamp to [0, 1]
    return np.clip(np.minimum(left_slope, right_slope), 0.0, 1.0)


def trapezoidal_mf(x: float, a: float, b: float, c: float, d: float) -> float:
    """Trapezoidal membership function."""
    x = np.asarray(x, dtype=float)

    # LEFT
    if b != a:
        left = (x - a) / (b - a)
    else:
        left = (x >= a).astype(float)

    # RIGHT
    if d != c:
        right = (d - x) / (d - c)
    else:
        right = (x <= d).astype(float)

    # Flat top (between b and c)
    flat = np.ones_like(x)

    return np.clip(np.minimum(np.minimum(left, right), flat), 0, 1)


def ground_saturation_mf(saturation: float) -> dict:
    """Membership functions for ground saturation levels."""
    very_dry = trapezoidal_mf(saturation, 0, 0, 10, 20)
    dry = triangular_mf(saturation, 10, 30, 50)
    moist = triangular_mf(saturation, 30, 50, 70)
    wet = trapezoidal_mf(saturation, 50, 70, 100, 100)

    return {"very_dry": very_dry, "dry": dry, "moist": moist, "wet": wet}


def day_temperature_mf(temperature: float) -> dict:
    """Membership functions for daytime temperature levels."""
    cold = trapezoidal_mf(temperature, 0, 0, 5, 10)
    cool = triangular_mf(temperature, 5, 12, 18)
    warm = trapezoidal_mf(temperature, 16, 18, 32, 34)
    hot = trapezoidal_mf(temperature, 32, 34, 50, 50)

    return {"cold": cold, "cool": cool, "warm": warm, "hot": hot}


def night_temperature_mf(temperature: float) -> dict:
    """Membership functions for nighttime temperature levels."""
    cold = trapezoidal_mf(temperature, -1, -1, -1, 2)
    cool = triangular_mf(temperature, 0, 10, 16)
    warm = trapezoidal_mf(temperature, 14, 16, 19, 21)
    hot = trapezoidal_mf(temperature, 19, 21, 21, 21)
    return {"cold": cold, "cool": cool, "warm": warm, "hot": hot}


def humidity_mf(humidity: float) -> dict:
    """Membership functions for humidity levels."""
    dry = trapezoidal_mf(humidity, 0, 0, 30, 40)
    normal = trapezoidal_mf(humidity, 30, 40, 60, 80)
    humid = trapezoidal_mf(humidity, 60, 80, 100, 100)
    return {"dry": dry, "normal": normal, "humid": humid}


def precipitation_probability_mf(probability: float) -> dict:
    """Membership functions for precipitation probability levels."""
    no_rain = triangular_mf(probability, 0, 0, 10)
    low_chance = trapezoidal_mf(probability, 0, 15, 30, 45)
    high_chance = trapezoidal_mf(probability, 35, 50, 100, 100)
    return {
        "no_rain": no_rain,
        "low_chance": low_chance,
        "high_chance": high_chance,
    }


def output_mf(watering_duration: float) -> dict:
    """Membership functions for watering duration levels."""
    none = triangular_mf(watering_duration, 0, 0, 10)
    short = triangular_mf(watering_duration, 10, 30, 50)
    medium = triangular_mf(watering_duration, 40, 60, 80)
    long = triangular_mf(watering_duration, 70, 90, 100)
    return {
        "none": none,
        "short": short,
        "medium": medium,
        "long": long,
    }
