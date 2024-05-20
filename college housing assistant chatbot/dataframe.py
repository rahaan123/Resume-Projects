import csv
import numpy as np
import pandas as pd


df = pd.read_csv("dataset.csv")
df_to_train = pd.read_csv("dataset.csv")
# print(df_to_train.info())
# print()


def split_floor_plan(floor_plan):
    if floor_plan == '0':
        return 0, 0
    else:
        bedroom, bathroom = floor_plan.split(',')
        return int(bedroom), float(bathroom)
    
df_to_train['Bedroom'], df_to_train['Bathroom'] = zip(*df_to_train['Floor Plan'].apply(split_floor_plan))
df_to_train.drop(['Floor Plan'], axis=1, inplace=True)
df_to_train['Apartment_Bus_no.'] = df_to_train['Apartment_Bus_no.'].fillna(0)
# Define a dictionary for replacement values
replacement_dict = {'Y': 1, 'N': 0}

# Replace values in multiple columns using the replace() method
columns_to_replace = ['Complete Unit', 'Furnished', 'Utilities_included', 
                      'Pet Friendly', 'Gym_and_Pool', 'In-Unit Laundry', 
                      'Recreation areas', 'Free Parking', 'Apartment_Bus_stop']
df_to_train[columns_to_replace] = df_to_train[columns_to_replace].replace(replacement_dict)
#print(df_to_train.info())


selected = ["Apartment_Name", "Website", "Contact No.", "Address"]
apartments = df[selected]
apartments = apartments.drop_duplicates(subset='Apartment_Name', keep='first')

apartments.to_csv("apartments.csv", index =False)
df_to_train.to_csv("cleaned_data.csv", index=False)
