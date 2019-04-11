from PIL import Image

im = Image.open("ref6.ppm")
width, height = im.size
rgb_im = im.convert('RGB')
red = []
green = []
blue = []


for i in range(width):
	for j in range(height):
		r, g, b = rgb_im.getpixel((i, j))
		red.append(str(r))
		green.append(str(g))
		blue.append(str(b))
count = 0
with open('ref6.txt', 'w') as f:
	for item in red:
		f.write(item)
		f.write(" ")
		
			
		count += 1


	for item in blue:
		f.write(item)
		f.write(" ")

	for item in green:
		f.write(item)
		f.write(" ")
