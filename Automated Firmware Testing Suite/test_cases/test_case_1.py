import requests

def test_firmware_update():
    response = requests.get("http://device_ip/firmware_version")
    assert response.status_code == 200, "Firmware update failed!"
    assert response.json()['version'] == "1.2.3", "Version mismatch!"
