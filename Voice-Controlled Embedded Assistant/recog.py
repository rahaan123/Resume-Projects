import speech_recognition as sr

# Initialize recognizer
recognizer = sr.Recognizer()

# Microphone as source
with sr.Microphone() as source:
    print("Say something...")
    recognizer.adjust_for_ambient_noise(source)  # Adjust for ambient noise
    audio = recognizer.listen(source)  # Listen for audio input

# Recognize speech using Google Web Speech API
try:
    command = recognizer.recognize_google(audio)
    print("You said: " + command)
    
    # Process command (simple example)
    if "turn on the light" in command.lower():
        print("Turning on the light...")
    elif "turn off the light" in command.lower():
        print("Turning off the light...")
    else:
        print("Command not recognized.")
except sr.UnknownValueError:
    print("Sorry, I could not understand the audio.")
except sr.RequestError:
    print("Could not request results from Google Speech Recognition service.")
