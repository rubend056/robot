## IEEE Robot Computer Vision

This is the code for a robot IEEE is doing in FIU for the SouthwestCon19 competition. This code will run on the Odroid board
The official and, for the most part outdated code, is hosted [Here](https://github.com/ieeefiu/robot2019)


### Required libraries:
* OpenCV
* Boost
* FANN

### The strategy:

#### Divide image into desired colors

#### For each desired color mask:
* INPUT: Each pixel with lightness value over the color mask
* OUTPUT: posx, posy, width, height

### To train the network
1. Filenames will have all desired output, files will be images .jpg etc...

2. Filename is type_pox_psy_width_height

3. 89*50 input images that are 4450 input neurons with values from 0-1
4. 2 hidden layers

5. Outputs all ranging from 0-1: 
  * posx
  * posy
  * width
  * height

6. Nothing found then posx = 0, posy = 0, width = 0, height = 0; Cutoff will be if width and height are less than 0.02, therefore if neural network outputs under cutoff then nothing was found

7. Training will only use one object per image as that's the only way to make the training algorithm work

8. The data file to be outputted for neural network training will be something like this:

#### .data file structure
  num_train_data num_input num_output  
  inputdata seperated by space  
  outputdata seperated by space  
  .  
  .  
  .  
  inputdata seperated by space  
  outputdata seperated by space  

* We will train two neural networks, one for recognizing spheres, another for recognizing cubes

### For multiple objects
* We will feed the extracted color mask, value/lightness only image to both neural networks, if anything is found, then set the pixels of found object will be zeroed and the process will be repeated until nothing more is found

## There are 4 main programs
### Train
Contains everything required to import image database and train a neural network
### Execute
Will take a trained network and apply it on camera images, this is what will run in the robot
### Camera
Will take images and store them in the database
* Number in top left is image count
* R: Toggles the record mode
* Spacebar: Takes a picture
### Edit
Will be able to classify the images in the database
* Up, Down, Left, Right: Move within files
* W, A, S, D: Move the box
* T, F, G, H: Resize the box
* R: Reset box to the middle
* Q: Set box psition and size all to zero, and set changed to ON. Do this the box doesn't appear on the image
* E: Toggle the changed state, if changed state is ON, the file will be saved when another file is loaded. Please only save when the box is in the right place.
* Shift, Ctrl: Increase and decrease respectively, changing the box move amount
