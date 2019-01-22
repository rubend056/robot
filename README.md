## IEEE Robot Computer Vision

This is the code for a robot IEEE is doing in FIU for the SouthwestCon19 competition. This code will run on the Odroid board
The official and, for the most part outdated code, is hosted [Here](https://github.com/ieeefiu/robot2019)


### Required libraries:
* OpenCV

### The strategy:

#### Divide image into desired colors

#### For each desired color mask:
* INPUT: Each pixel with lightness value over the color mask
* OUTPUT: posx, posy, width, height

### To run program
1. Plug in external camera, camera must contain a USB connection and contain "Auto Exposure" as a supported feature.

2. Simply run the script file bAndRun to build your project

3. Then run execute or square to run program

#### .data file structure 
  inputdata seperated by space  
  outputdata seperated by space  
  .  
  .  
  .  
  inputdata seperated by space  
  outputdata seperated by space  

### For multiple objects
* We will feed the extracted color mask, value/lightness only image to both neural networks, if anything is found, then set the pixels of found object will be zeroed and the process will be repeated until nothing more is found

## There are 2 main programs

### Execute
Will display positions of spherical objects, this is what will run in the robot
### Square
Currently development is still in progress

