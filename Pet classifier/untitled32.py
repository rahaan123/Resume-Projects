# -*- coding: utf-8 -*-
"""
Created on Sat Mar 27 00:43:04 2021

@author: kajim
"""


import matplotlib.pyplot as plt
import numpy as np
from tensorflow.keras.preprocessing.image import ImageDataGenerator
#%matplotlib inline



gen = ImageDataGenerator(rotation_range=0.01, width_shift_range=0.02, height_shift_range=0.03, shear_range=0.01, zoom_range=0.01, 
    channel_shift_range=5., horizontal_flip=False)

chosen_img = r'C:\Users\letha\OneDrive\Desktop\491\project\datasets\shiro\og\13.jpg'
image_path = r'C:\Users\letha\OneDrive\Desktop\491\project\datasets\shiro\var'

image = np.expand_dims(plt.imread(chosen_img),0)

count = 0
for batch in gen.flow(image, batch_size=1,save_to_dir= image_path, save_format='jpg'):
    count += 1
    if count > 10:
        break
