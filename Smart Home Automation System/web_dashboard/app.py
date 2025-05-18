from flask import Flask, render_template, jsonify, request
import serial
import time

app = Flask(__name__)

# Set up serial connection to Arduino
arduino = serial.Serial('COM3', 9600, timeout=1)  # Update COM port for your system
time.sleep(2)  # Allow Arduino to initialize

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/control', methods=['POST'])
def control_device():
    device = request.form.get('device')
    action = request.form.get('action')
    
    if device == 'light':
        arduino.write(b'1' if action == 'on' else b'0')
    elif device == 'fan':
        arduino.write(b'1' if action == 'on' else b'0')
    
    return jsonify({"status": "success"})

@app.route('/sensor_data')
def sensor_data():
    arduino.write(b'R')  # Request sensor data
    data = arduino.readline().decode('utf-8').strip()
    
    # Assuming the data is in the format: "Temperature: xx.x"
    if data:
        temperature = data.split(': ')[1]
        return jsonify({"temperature": temperature})
    return jsonify({"error": "No data available"})

if __name__ == '__main__':
    app.run(debug=True)

