"""Weather data retrieval and processing module"""

import os
import json
import requests
from dotenv import load_dotenv
from constants import LOCATION, WEATHER_BASE_URL as BASE_URL

load_dotenv()
API_KEY = os.getenv("WEATHER_API_KEY")

if not API_KEY or not BASE_URL:
    raise ValueError("API key or Base URL not found in environment variables.")


def get_weather_data():
    """Fetch weather data from the API."""
    latitude, longitude = LOCATION
    url = f"{BASE_URL}?key={API_KEY}&location.latitude={latitude}&location.longitude={longitude}"

    response = requests.get(url)
    if response.status_code != 200:
        raise Exception(f"Error fetching weather data: {response.status_code}")

    return process_weather_data(response.json().get("forecastDays"))


def process_weather_data(weatherjson):
    """Process and extract relevant weather information."""

    if not weatherjson:
        raise ValueError("Weather forecast data is empty.")

    # Extract today's forecast
    today_forecast = weatherjson[0]
    daytime_forecast = today_forecast.get("daytimeForecast", {})
    nighttime_forecast = today_forecast.get("nighttimeForecast", {})
    max_temp_data = today_forecast.get("maxTemperature", {})
    min_temp_data = today_forecast.get("minTemperature", {})

    # Extract temperature data
    day_temp = max_temp_data.get("degrees")
    night_temp = min_temp_data.get("degrees")
    unit = max_temp_data.get("unit")

    if day_temp is None or night_temp is None:
        raise ValueError("Temperature data not found in API response.")

    # convert to celsius if in fahrenheit
    if unit == "FAHRENHEIT":
        day_temp = (day_temp - 32) * 5.0 / 9.0
        night_temp = (night_temp - 32) * 5.0 / 9.0

    # Extract humidity
    day_humidity = daytime_forecast.get("relativeHumidity", 0)
    night_humidity = nighttime_forecast.get("relativeHumidity", 0)
    humidity = (day_humidity + night_humidity) / 2

    # Extract probability of precipitation (PoP)
    day_pop = (
        daytime_forecast.get("precipitation", {})
        .get("probability", {})
        .get("percent", 0)
    )
    night_pop = (
        nighttime_forecast.get("precipitation", {})
        .get("probability", {})
        .get("percent", 0)
    )
    PoP = (day_pop + night_pop) / 2

    processed_weather_data = {
        "ground_saturation": None,
        "day_temp": day_temp,
        "night_temp": night_temp,
        "humidity": humidity,
        "PoP": PoP,
    }

    return processed_weather_data


if __name__ == "__main__":
    weather_data = get_weather_data()
    print(json.dumps(weather_data, indent=2))
