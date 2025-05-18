def test_sensor_reading():
    sensor_value = 100  # Simulated sensor reading
    assert sensor_value > 50, "Sensor value too low!"
    assert sensor_value < 200, "Sensor value too high!"
