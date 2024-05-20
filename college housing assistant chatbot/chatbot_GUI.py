from flask import Flask, request, render_template_string, jsonify, session
from flask_session import Session
import pandas as pd
from recommend import load_dataset, parse_preferences, filter_listings

app = Flask(__name__)
app.config["SECRET_KEY"] = "your_secret_key"
app.config["SESSION_TYPE"] = "filesystem"
Session(app)

# Define stop words for ending the chat
stop_words = ['exit', 'quit', 'stop', 'end', 'goodbye']

# loading dataset once when server online
data = load_dataset('cleaned_data.csv') 

def display_listings(filtered_data, limit=3):
    response = ""
    for index, row in filtered_data.head(limit).iterrows():
        response += f"\n- Name: {row['Apartment_Name']}"
        response += f"\n  Price: ${row['Apartment_Price']}"
        response += f"\n  Bedrooms: {row['Bedroom']} Bathrooms: {row['Bathroom']}"
        complete_unit = row['Complete Unit']
        complete_unit_text = 'Yes' if complete_unit == 1 else 'No'
        response += f"\n  Complete Unit: {complete_unit_text}"
        response += f"\n  Address: {row['Address']}"
        response += f"\n  Website: {row['Website']}"
        response += f"\n  Contact Number: {row['Contact No.']}"
        bus_stop_info = 'Yes' if row['Apartment_Bus_stop'] == 1 else 'No'
        response += f"\n  Bus Stop Nearby: {bus_stop_info}"
        if bus_stop_info == 'Yes':
            response += f"\n  Bus Numbers: {row['Apartment_Bus_no.']}"
        response += "\n"
    return response

HTML_TEMPLATE = """
<!doctype html>
<html>
<head>
    <title>Off-Campus Housing Chatbot</title>
    <style>
        body { 
            font-family: Arial, sans-serif; 
            margin: 0;
            padding: 0;
            display: flex;
            flex-direction: column;
            height: 100vh;
        }
        header {
            background-color: #007bff;
            color: white;
            padding: 10px 20px;
            text-align: center;
            font-size: 24px;
        }
        #chat-window {
            flex-grow: 1;
            width: 80%;
            margin: 20px auto;
            padding: 10px;
            border: 1px solid #ccc;
            overflow-y: scroll;
            background-color: #f9f9f9;
        }
        .chat-bubble {
            padding: 10px;
            border-radius: 20px;
            margin: 5px;
            width: fit-content;
            max-width: 70%;
            box-shadow: 0px 0px 5px rgba(0,0,0,0.2);
        }
        .user-message {
            background-color: #007bff;
            color: white;
            margin-left: auto;
        }
        .chatbot-message {
            background-color: #f1f1f1;
            color: black;
        }
        .chat-input-area {
            display: flex;
            padding: 10px;
            background-color: #fff;
        }
        #user-input {
            flex-grow: 1;
            padding: 10px;
            margin-right: 10px;
            font-size: 16px;
            border: 1px solid #ccc;
            border-radius: 5px;
        }
        button {
            padding: 10px 20px;
            font-size: 16px;
            color: white;
            background-color: #007bff;
            border: none;
            border-radius: 5px;
            cursor: pointer;
        }
        button:hover {
            background-color: #0056b3;
        }
        .chat-bubble {
            white-space: pre-wrap; /* Allows text with new lines to be properly formatted */
        }
    </style>
</head>
<body>
    <header>Off-Campus Housing Chatbot</header>
    <div id="chat-window">
        <div class="chat-bubble chatbot-message">Chatbot: Hi, how can I help you today? (Type 'exit' to stop)</div>
    </div>
    <div class="chat-input-area">
        <input type="text" id="user-input" placeholder="Type your message here..." onkeydown="if (event.key === 'Enter') { sendMessage(); }" />
        <button onclick="sendMessage()">Send</button>
    </div>

    <script>
        function sendMessage() {
            var userInput = document.getElementById('user-input').value;
            if (!userInput.trim()) return;  // Don't send empty messages
            addMessage(userInput, 'user-message');
            
            fetch('/send_message', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify({message: userInput}),
            })
            .then(response => response.json())
            .then(data => {
                addMessage(data.message, 'chatbot-message');
            });

            document.getElementById('user-input').value = ''; // Clear input field after sending
        }

        function addMessage(message, className) {
            var chatWindow = document.getElementById('chat-window');
            var bubble = document.createElement('div');
            bubble.className = 'chat-bubble ' + className;
            bubble.textContent = message;
            chatWindow.appendChild(bubble);
            chatWindow.scrollTop = chatWindow.scrollHeight;
        }
    </script>
</body>
</html>
"""


@app.route('/')
def index():
    return render_template_string(HTML_TEMPLATE)

@app.route('/send_message', methods=['POST'])
def send_message():
    message_data = request.json
    user_message = message_data['message'].strip().lower()

    if 'awaiting_response' in session and session['awaiting_response']:
        if user_message in ['no', 'n']:
            session['awaiting_response'] = False
            return jsonify(message="Thank you for using our service. Have a great day!")
        elif user_message in ['yes', 'y']:
            session['awaiting_response'] = False
            # Logic to handle refining search or showing more options

    if user_message in stop_words:
        return jsonify(message="Thank you for using our service. Have a great day!")
    
    if 'hello' in user_message or 'hi' in user_message or 'hey' in user_message:
        return jsonify(message="Hello there! Can I assist you in finding a place to stay?")

    preferences = parse_preferences(user_message)
    if not preferences:
        return jsonify(message="Could you please provide more details like the number of bedrooms, bathrooms, and your budget?")

    filtered_data = filter_listings(data, preferences)
    if filtered_data.empty:
        return jsonify(message="I couldn't find any listings that match your criteria. Would you like to adjust your search criteria? (yes/no): ")
    else:
        chatbot_response = "Here are some options based on your preferences:" + display_listings(filtered_data)
        session['awaiting_response'] = True
        return jsonify(message=chatbot_response + "\nWould you like to refine your search or see more options? (yes/no): ")

if __name__ == '__main__':
    app.run(debug=True)
