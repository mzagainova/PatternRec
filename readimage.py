from PIL import Image

im = Image.open("Training_6.ppm")
width, height = im.size
rgb_im = im.convert('RGB')
red = []
green = []
blue = []

print(width)
print(height)

for i in range(width):
	for j in range(height):
		r, g, b = rgb_im.getpixel((i, j))
		red.append(str(r))
		green.append(str(g))
		blue.append(str(b))
count = 0
with open('Training_6.txt', 'w') as f:
	for item in red:
		f.write(item)
		f.write(" ")
		if count == 2324768:
			print(item)
		count += 1

	#f.write('\n')
	for item in blue:
		f.write(item)
		f.write(" ")
	#f.write('\n')
	for item in green:
		f.write(item)
		f.write(" ")
print(red[0], red[width * height-1], len(red))
print(green[0], green[width*height-1], len(green))
print(blue[0], blue[width*height-1], len(blue))

