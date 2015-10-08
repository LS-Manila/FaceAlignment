# Face Alignment

Face recognition algorithms are highly dependent on the positioning of the face images. One of the features that are often used in face extraction are the eyes. Proper positioning of the eyes in a face image are required in order for the program to recognize a person. Face images are *aligned*, *transformed*, *cropped*, and *resized* with accordance to the eye coordinates. 

Given an unaligned face image, the eye alignment markers are set to the image, and after being marked, the image is rotated in order for proper eye alignment. When the alignment is done, the image is cropped in order to get a closer look on the features that are needed for extraction. 
