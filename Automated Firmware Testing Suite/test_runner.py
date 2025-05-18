import pytest
import sys

def run_tests():
    # Run all test cases and store the results in a log file
    result = pytest.main(["--maxfail=5", "--disable-warnings", "--tb=short"])
    if result == 0:
        print("All tests passed!")
    else:
        print(f"Tests failed with exit code {result}")
    
if __name__ == "__main__":
    run_tests()
