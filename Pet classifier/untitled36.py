# -*- coding: utf-8 -*-
"""
Created on Tue Apr 27 03:18:15 2021

@author: letha
"""
import opencv-python as cv2
import matlplotlib.pyplot as plt
import numpy as np
img = cv2.imread(r'C:\Users\letha\OneDrive\Desktop\491\project\datasets\shiro\og')
plt.imshow(img, cmap = 'gray', interpolation = 'bicubic')
plt.xticks([]), plt.yticks([])  # to hide tick values on X and Y axis
plt.show()