import logging

# Configure logging
logging.basicConfig(filename='test_log.log', level=logging.INFO)

def log_test_result(test_name, status, message=""):
    if status:
        logging.info(f"Test Passed: {test_name}")
    else:
        logging.error(f"Test Failed: {test_name} - {message}")

def display_log():
    with open('test_log.log', 'r') as log_file:
        print(log_file.read())
