# Smart Home Automation System

## Overview

This project implements a **Smart Home Automation System** using **C++** for hardware interfacing (e.g., Arduino or Raspberry Pi) and **Python** for backend processing and real-time data visualization through a **Flask** web dashboard.

The system allows users to control home devices (lights and fans) remotely and monitor sensor data (e.g., temperature) via a web-based interface.

---

## Features

* **C++** for interfacing with hardware (Arduino or Raspberry Pi).
* **Python** for backend processing and communication with Arduino.
* **Flask** for serving the web dashboard and API for controlling devices.
* **Real-time sensor data** visualization (e.g., temperature).
* **Control home devices** (lights and fans) through a web interface.

---

## Requirements

### Hardware

* **Arduino board** (e.g., Arduino Uno).
* **Relay modules** to control lights and fans.
* **Temperature sensor** (e.g., TMP36 or DHT11).
* **Raspberry Pi** (optional) for running the Flask server (if not using Arduino).
* **Jumper wires** and a **breadboard** for wiring.

### Software

* **Arduino IDE** (for uploading C++ code to Arduino).
* **Python 3.x** (for running Flask server).
* **Flask** (for the web server and dashboard).
* **PySerial** (for serial communication with Arduino).

---

## Installation

### 1. **Set up the Arduino**

1. Download and install the **Arduino IDE** if you haven't already.
2. Open the `control_lights.cpp`, `control_fans.cpp`, and `sensor_reading.cpp` files in the Arduino IDE.
3. Upload the files to your Arduino board.

### 2. **Set up Python Environment**

1. Clone or download this repository.

   ```bash
   git clone https://github.com/yourusername/smart-home-automation.git
   cd smart-home-automation
   ```

2. Create a virtual environment (optional but recommended):

   ```bash
   python -m venv venv
   source venv/bin/activate  # On Windows use `venv\Scripts\activate`
   ```

3. Install required Python dependencies:

   ```bash
   pip install -r requirements.txt
   ```

---

## Configuration

1. **Update the Serial Port for Arduino**:

   * Open the `app.py` file and find the line where the serial connection is established:

   ```python
   arduino = serial.Serial('COM3', 9600, timeout=1)  # Update COM port
   ```

   * Replace `'COM3'` with the appropriate port for your system (e.g., `/dev/ttyACM0` for Linux or `/dev/ttyUSB0` for Raspberry Pi).

2. **Set up your hardware**:

   * Connect the **Relay modules** to the **Arduino** to control the lights and fans.
   * Connect the **Temperature sensor** to the Arduino and ensure it is working properly.

---

## Running the System

### 1. **Start the Flask Backend**

After setting up everything, you can run the Flask server to start the web dashboard.

```bash
python app.py
```

This will start the Flask server on `http://127.0.0.1:5000`. You can open this URL in your browser to access the web dashboard.

### 2. **Control Devices from the Web Interface**

* On the dashboard, you will see buttons to control the **light** and **fan**:

  * **Turn Light On**
  * **Turn Light Off**
  * **Turn Fan On**
  * **Turn Fan Off**

### 3. **View Sensor Data**

* The web dashboard will show real-time temperature data retrieved from the **Arduino** every 5 seconds.

---

## Folder Structure

```
smart_home_automation_system/
│
├── hardware_interface/
│   ├── control_lights.cpp      # C++ code to control lights
│   ├── control_fans.cpp        # C++ code to control fans
│   └── sensor_reading.cpp      # C++ code to read sensor data
│
├── web_dashboard/
│   ├── app.py                  # Flask backend for web server
│   ├── templates/
│   │   └── index.html          # HTML template for the web dashboard
│   └── static/
│       └── style.css           # Styles for the web dashboard
│
├── requirements.txt            # Python dependencies
├── config.py                   # Configuration file (if needed)
└── README.md                   # Project instructions
```

---

## Usage

* **Web Interface**: Access the system on your local machine via the Flask server:

  * Visit `http://127.0.0.1:5000` in your web browser.

* **Control Devices**: Use the web interface to turn on/off lights and fans remotely.

* **Monitor Sensor Data**: The system will display real-time temperature readings fetched from the sensor.

---

## Troubleshooting

1. **Arduino not responding**: Ensure that the serial port in `app.py` is correctly set to the port where your Arduino is connected. Check your Arduino IDE for the correct COM port.
2. **Flask server not running**: Ensure that the required Python dependencies are installed and that the `app.py` script is run within the correct environment.

---

## Contributing

Feel free to fork the repository, make improvements, and submit pull requests. Contributions are always welcome!

---

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
