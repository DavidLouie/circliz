# circliz
Takes an input image and produces an output image produced of circles that is similar to the input image. 
The image is produced by choosing a random colour from the source image, and drawing a circle of that colour on the output image. If the circle moves the output image closer to the source image, the changes are saved. Otherwise, they are reverted. After a large number of iterations of this process, the output image will resemble the source image, made out of only circles.

# Usage
$ ./circliz filename [output filename] [number of iterations] [changing radius]

Only the first argument is mandatory. 
  - output filename: the filename that the program will write the output image to.
  - number of iterations: number of iterations that will be run. Must be at least 100,000.
  - changing radius: include this argument if you want the radius of the drawn circles to change 
  
# Samples
Sample usage with static circle radius:

$ ./circliz JapanCitystreet.jpg JCS_out.png 1000000

Input image:
![JapanCitystreet](https://raw.githubusercontent.com/DavidLouie/circliz/master/JapanCitystreet.jpg)
Ouput image:
![JCS_out](https://raw.githubusercontent.com/DavidLouie/circliz/master/JCS_out.png)


Sample usage with changing circle radius:

$ ./circliz b2049.jpg b2049_out.jpg 100000 1

Input image:
![b2049](https://raw.githubusercontent.com/DavidLouie/circliz/master/b2049.jpg)
Output image:
![b2049_out](https://raw.githubusercontent.com/DavidLouie/circliz/master/b2049_out.png)

