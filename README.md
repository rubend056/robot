This is the code for a robot IEEE is doing in FIU for the SouthwestCon19 competition. This code will run on the Odroid board


Required libraries:
	OpenCV
	Boost
	FANN

The strategy:

1- Divide image into desired colors

2-For each desired color mask:
-INPUT: Each pixel with lightness value over the color mask
-OUTPUT: Position with a width and height value

To train the network
- Filenames will have all desired output, files will be images .jpg etc...

- Filename is type_pox_psy_width_height

- 89*50 input images that are 44500 input neurons with values from 0-1

- 2 hidden layers

- Outputs all ranging from 0-1: 

posx

posy

width

height


Nothing found then posx = 0, posy = 0, width = 0, height = 0; Cutoff will be if width and height are < 0.02, therefore if neural network outputs under cutoff then nothing was found

We will train two neural networks, one for recognizing spheres, another for recognizing cubes

We will feed the extracted color mask, value/lightness only image to both neural networks, if anything is found, then set the pixels of found object will be zeroed and the process will be repeated until nothing more is found

Training will only use one object per image as that's the only way to make the training algorithm work

The data file to be outputted for neural network training will be something like this

num_train_data num_input num_output

inputdata seperated by space

outputdata seperated by space

.
.
.

inputdata seperated by space

outputdata seperated by space
