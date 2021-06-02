# -*- coding: utf-8 -*-
"""
Created on Sat Mar 27 01:39:54 2021

@author: kajim
"""
import os
# changing the directory to where the image files are located
os.chdir(r"C:\Users\letha\OneDrive\Desktop\491\project\datasets\shiro\var")

for index, file in enumerate(os.listdir()):
    os.rename(file, str(index + 14) + ".jpg")