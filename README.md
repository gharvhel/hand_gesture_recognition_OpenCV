# hand_gesture_recognition_OpenCV
A program which will take as its input a sequence of images of either a fist or a palm and then produce from such sequence an instruction to the computer. 
The process involves capturing at least two different images with clear backgrounds and then inputting them into the software. The software will then proceed to do a few image editing procedures to pick out the skin color from the images, which is then captured and contoured. Once we obtain a contour, we can then find defects as well as the center of mass hand, which allows the program to determine the “what and where” of the hand respectively. 
After that, the program is able to check whether or not the combination of the “what and where” for the two pictures is part of a specified grammar; if it is, an instruction is logged to the console.  
Check Visual_Interfaces_project1.pdf for mor details.
