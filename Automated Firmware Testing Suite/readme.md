# first do
pip install -r requirements.txt

# To run the repo:
python test_runner.py

# View the test log:
from log_manager import display_log
display_log()

# Generate the Report:
### After running the tests, generate the report with:
from report_generator import generate_report
generate_report()
