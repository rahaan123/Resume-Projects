import time
import datetime

def generate_report():
    start_time = datetime.datetime.now()
    
    # Simulate running tests and measuring performance
    time.sleep(2)  # Placeholder for actual test running time
    
    end_time = datetime.datetime.now()
    duration = end_time - start_time
    
    # Generate a summary report
    report = f"""
    Firmware Testing Report - {end_time.strftime('%Y-%m-%d %H:%M:%S')}
    -----------------------------------------------------------
    Total Tests Run: 3
    Passed: 3
    Failed: 0
    Total Duration: {duration}
    """
    
    # Save the report to a file
    with open("test_report.txt", "w") as report_file:
        report_file.write(report)
    
    print("Test Report Generated Successfully!")
