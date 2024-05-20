import pandas as pd
import spacy
from spacy.matcher import Matcher

nlp = spacy.load("en_core_web_sm")
matcher = Matcher(nlp.vocab)

def load_dataset(file_path):
    data = pd.read_csv(file_path)
    data['Apartment_Price'] = pd.to_numeric(data['Apartment_Price'].replace('[\$,]', '', regex=True), errors='coerce')
    return data

def parse_preferences(user_input):
    doc = nlp(user_input)
    preferences = {}
    decimal_pattern = {'TEXT': {'REGEX': r'\d+(\.\d+)?'}}

    # Existing patterns
    bed_bath_pattern = [{'IS_DIGIT': True}, {'LOWER': {'IN': ['bed', 'bedroom', 'bedrooms']}}, decimal_pattern, {'LOWER': {'IN': ['bath', 'bathroom', 'bathrooms']}}]
    price_pattern = [{'LOWER': {'IN': ['under', 'below', 'less', 'maximum', 'max']}}, {'IS_DIGIT': True}]
    parking_pattern = [{'LOWER': 'parking'}]
    bus_stop_pattern = [{'LOWER': 'bus'}, {'LOWER': 'stop'}]
    furnished_pattern = [{'LOWER': 'furnished'}]
    pet_friendly_pattern = [{'LOWER': 'pet'}] 
    gym_pool_pattern = [{'LOWER': {'IN': ['gym', 'pool']}}]
    utilities_pattern = [{'LOWER': {'IN': ['utilities', 'bills']}}]
    laundry_pattern = [{'LOWER': {'IN': ['washer', 'dryer', 'laundry']}}]
    #complete_unit_pattern = [{'LOWER': {'IN': ['full unit', 'complete unit']}}]
    rec_pattern = [{'LOWER': {'IN': ['recreational area', 'basketball', 'volleyball', 'court']}}]
    distance_pattern = [{'LOWER': {'IN': ['close to', 'near', 'within']}}, {'LOWER': {'IN': ['fishpond', 'msc', 'zach', 'reed', 'mays', 'rellis']}}]
    full_unit_type_pattern = [{'LOWER': {'IN': ['full', 'complete', 'whole']}}, {'LOWER': {'IN': ['unit', 'apartment', 'house']}}]
    individual_unit_type_pattern = [{'LOWER': {'IN': ['individual', 'single', 'each', 'private']}}, {'LOWER': {'IN': ['unit', 'room', 'bedroom', 'space']}}]

    # Add patterns to matcher
    matcher.add("BED_BATH_PATTERN", [bed_bath_pattern])
    matcher.add("PRICE_PATTERN", [price_pattern])
    matcher.add("PARKING_PATTERN", [parking_pattern])
    matcher.add("BUS_STOP_PATTERN", [bus_stop_pattern])
    matcher.add("FURNISHED_PATTERN", [furnished_pattern])
    matcher.add("PET_FRIENDLY_PATTERN", [pet_friendly_pattern])
    matcher.add("GYM_POOL_PATTERN", [gym_pool_pattern])
    matcher.add("UTLITIES_PATTERN", [utilities_pattern])
    matcher.add("LAUNDRY_PATTERN", [laundry_pattern])
    #matcher.add("COMPLETE_UNIT_PATTERN", [complete_unit_pattern])
    matcher.add("REC_PATTERN", [rec_pattern])
    matcher.add("FULL_UNIT_TYPE_PATTERN", [full_unit_type_pattern])
    matcher.add("INDIVIDUAL_UNIT_TYPE_PATTERN", [individual_unit_type_pattern])
    # Add more as needed
    matcher.add("DISTANCE_PATTERN", [distance_pattern])

    matches = matcher(doc)

    for match_id, start, end in matches:
        span = doc[start:end]
        match_label = nlp.vocab.strings[match_id]

        if match_label == "BED_BATH_PATTERN":
            numbers = [float(token.text) for token in span if token.like_num]
            if len(numbers) >= 2:
                preferences['bedrooms'] = numbers[0]
                preferences['bathrooms'] = numbers[1]
        elif match_label == "PRICE_PATTERN":
            numbers = [int(token.text) for token in span if token.like_num]
            if numbers:
                preferences['max_price'] = numbers[0]
        elif match_label == "PARKING_PATTERN":
            preferences['parking'] = True
        elif match_label == "BUS_STOP_PATTERN":
            preferences['bus_stop'] = True
        elif match_label == "FURNISHED_PATTERN":
            preferences['furnished'] = True
        elif match_label == "PET_FRIENDLY_PATTERN":
            preferences['pet_friendly'] = True
        elif match_label == "GYM_POOL_PATTERN":
            preferences['gym_and_pool'] = True
        elif match_label == "UTLITIES_PATTERN":
            preferences['utilities'] = True
        elif match_label == "LAUNDRY_PATTERN":
            preferences['laundry'] = True
        elif match_label == "COMPLETE_UNIT_PATTERN":
            preferences['complete_unit'] = True
        elif match_label == "REC_PATTERN":
            preferences['rec'] = True
        elif match_label == "FULL_UNIT_TYPE_PATTERN":
            preferences['funit'] = True
        elif match_label == "INDIVIDUAL_UNIT_TYPE_PATTERN":
            preferences['iunit'] = True 
        # Add more conditions as needed for other features

        # New distance matching
        elif match_label == "DISTANCE_PATTERN":
            location = span[-1].text.lower()
            if location in ['fishpond', 'msc', 'zach', 'reed', 'mays', 'rellis']:
                preferences[f'distance_to_{location}'] = 'near'

    return preferences

def filter_listings(data, preferences):
    # Start with the full dataset
    filtered_data = data

    # Filter by price
    if 'max_price' in preferences:
        filtered_data = filtered_data[filtered_data['Apartment_Price'] <= preferences['max_price']]

    # Filter by number of bedrooms
    if 'bedrooms' in preferences:
        filtered_data = filtered_data[filtered_data['Bedroom'] == preferences['bedrooms']]

    # Filter by number of bathrooms
    if 'bathrooms' in preferences:
        filtered_data = filtered_data[filtered_data['Bathroom'] == preferences['bathrooms']]

    # Filter by parking availability
    if 'parking' in preferences:
        filtered_data = filtered_data[filtered_data['Free Parking'] == 1]

    # Filter by proximity to a bus stop
    if 'bus_stop' in preferences:
        filtered_data = filtered_data[filtered_data['Apartment_Bus_stop'] == 1]

    # Filter by apartment furnished
    if 'furnished' in preferences:
        filtered_data = filtered_data[filtered_data['Furnished'] == 1]
    
    # Filter by pets allowed
    if 'pet_friendly' in preferences:
        filtered_data = filtered_data[filtered_data['Pet Friendly'] == 1]
    
    # Filter by gym and pool filter
    if 'gym_and_pool' in preferences:
        filtered_data = filtered_data[filtered_data['Gym_and_Pool'] == 1]
    
    # Filter by util included
    if 'utilities' in preferences:
        filtered_data = filtered_data[filtered_data['Utilities_included'] == 1]

    # Filter by complete unit filter
    if 'funit' in preferences:
        filtered_data = filtered_data[filtered_data['Complete Unit'] == 1]
    
    if 'iunit' in preferences:
        filtered_data = filtered_data[filtered_data['Complete Unit'] == 0]

    # Filter by recreational areas
    if 'rec' in preferences:
        filtered_data = filtered_data[filtered_data['Recreation areas'] == 1]

    # Filter by laundry
    if 'laundry' in preferences:
        filtered_data = filtered_data[filtered_data['In-Unit Laundry'] == 1]

    # Track the distance column for sorting
    distance_column_for_sorting = None

    #filter by location
    if 'distance_to_fishpond' in preferences:
        filtered_data = filtered_data[filtered_data['Fishpond'] < 0.6] #Update the threshold as needed
        distance_column_for_sorting = 'Fishpond'

    if 'distance_to_msc' in preferences:
        filtered_data = filtered_data[filtered_data['MSC'] < 0.7] #Update the threshold as needed
        distance_column_for_sorting = 'MSC'

    if 'distance_to_zach' in preferences:
        filtered_data = filtered_data[filtered_data['Zachary_Engineering'] < 0.6] #Update the threshold as needed
        distance_column_for_sorting = 'Zachary_Engineering'

    if 'distance_to_reed' in preferences:
        filtered_data = filtered_data[filtered_data['Reed Arena'] < 1] #Update the threshold as needed
        distance_column_for_sorting = 'Reed Arena'
    
    if 'distance_to_mays' in preferences:
        filtered_data = filtered_data[filtered_data['Mays Business School'] < 1] #Update the threshold as needed
        distance_column_for_sorting = 'Mays Business School'

    if 'distance_to_rellis' in preferences:
        filtered_data = filtered_data[filtered_data['Rellis'] < 7] #Update the threshold as needed
        distance_column_for_sorting = 'Rellis'

    # If a distance filter was applied, sort by that distance column
    if distance_column_for_sorting:
        filtered_data = filtered_data.sort_values(by=distance_column_for_sorting, ascending=True)
    
    #print("THis is what i want", preferences)
    return filtered_data


def display_listings(filtered_data, limit=5):
    for index, row in filtered_data.head(limit).iterrows():
        print(f"\n- Name: {row['Apartment_Name']}")
        print(f"  Price: ${row['Apartment_Price']}")
        print(f"  Bedrooms: {row['Bedroom']} Bathrooms: {row['Bathroom']}")
        complete_unit = row['Complete Unit']
        complete_unit_text = 'Yes' if complete_unit == 1 else 'No'
        print(f"  Complete Unit: {complete_unit_text}")
        print(f"  Address: {row['Address']}")
        print(f"  Website: {row['Website']}")
        print(f"  Contact Number: {row['Contact No.']}")
        bus_stop_info = 'Yes' if row['Apartment_Bus_stop'] == 1 else 'No'
        print(f"  Bus Stop Nearby: {bus_stop_info}")
        if bus_stop_info == 'Yes':
            print(f"  Bus Numbers: {row['Apartment_Bus_no.']}")
        print()

def chatbot_main(file_path):
    data = load_dataset(file_path)

    stop_words = ['exit', 'quit', 'stop', 'end', 'goodbye']
    print("Chatbot: Hi, how can I help you today? (Type 'exit' to stop)")

    while True:
        user_input = input("User: ").strip().lower()

        if user_input in stop_words:
            print("Chatbot: Thank you for using our service. Have a great day!")
            break

        if 'hello' in user_input or 'hi' in user_input or 'hey' in user_input:
            print("Chatbot: Hello there! Can I assist you in finding a place to stay?")
            continue

        preferences = parse_preferences(user_input)

        if not preferences:
            print("Chatbot: Could you please provide more details like the number of bedrooms, bathrooms, and your budget?")
            continue

        filtered_data = filter_listings(data, preferences)
        #print(filtered_data)
        if filtered_data.empty:
            print("Chatbot: I couldn't find any listings that match your criteria.")
            if input("Chatbot: Would you like to adjust your search criteria? (yes/no): ").lower() != 'yes':
                continue
        else:
            print("Chatbot: Here are some options based on your preferences:")
            display_listings(filtered_data)

            more_input = input("Chatbot: Would you like to refine your search or see more options? (yes/no): ").lower()
            if more_input not in ['yes', 'y']:
                break

if __name__ == "__main__":
    chatbot_main('cleaned_data.csv')
