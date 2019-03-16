from PIL import Image
import numpy as np
#im = Image.open("check.txt")
#width, height = im.size
#rgb_im = im.convert('RGB')
red = []
green = []
blue = []

file = open("check.txt", "r")
count = 0
rgbArray = np.zeros((1856, 1392, 3), 'uint8')
for item in file:
	if count < 2583552:
		red.append(item)
	if count > 2583552 and count < 2583552 *2 + 1:
		green.append(item)
	if count > 2583552 * 2 + 1:
		blue.append(item)
	count +=1
rgbArray[..., 0] = red
rgbArray[..., 1] = blue
rgbArray[..., 2] = green

img = Image.fromarray(rgbArray)
img.save('check.jpeg')
