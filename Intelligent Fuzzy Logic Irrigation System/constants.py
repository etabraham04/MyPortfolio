"""Constants for the dynamic sprinkler system."""

# User Inputs
# LATITUDE = 45.3858581
# LONGITUDE = -75.6950041

LATITUDE = 37.322998  # cupertino latitude
LONGITUDE = -122.032181  # cupertino longitude

SOIL_TARGET_OPTIONS = ["ornamentals", "vegetables"]
SOIL_TARGET = SOIL_TARGET_OPTIONS[0]  # Choose "ornamental" or "vegetables"

# Watering duration constants
MAX_WATERING_DURATION_MIN = 60  # maximum watering duration in minutes
WATERING_FACTORS = {
    "none": 0.0,
    "short": 0.35,
    "medium": 0.65,
    "long": 1.0,
}

# Location
LOCATION = (LATITUDE, LONGITUDE)

# Weather API constants
WEATHER_BASE_URL = "https://weather.googleapis.com/v1/forecast/days:lookup"
