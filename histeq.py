#-> read image 
#-> convert image to gray scale
#-> histogram of image 
#-> prob of each pixel intensities 
#-> (max intensity upto which we want to extend)*(cumulative probabilities of pixel intensities) 
################################################################################
#!/usr/bin/python
import cv2
import numpy as np
from matplotlib import pyplot as plt
from pylab import *

img = cv2.imread('dehazed.jpg')
img1 = cv2.resize(img, (512, 512)) 
blue,green,red = cv2.split(img1)
# red = img1[:,:,0]
# green = img1[:,:,1]
# blue = img1[:,:,2]
# print "red: ",'\n', red 
print "Code Started"
temp = cv2.cvtColor(img1, cv2.COLOR_RGB2GRAY)

####################### Histogram Equalization ####################################
def histogramEqualized(img1):
	w,h = img1.shape
	hist,bins = np.histogram(img1.ravel(),256,[0,256])
	maxm = amax(img1)
	prob=[0]*256
	histeql = np.zeros((w,h)) 
	i=0
	while(i<256):
		prob[i] = hist[i]/(w*h*1.0)
		i+=1
	cve = np.cumsum(prob) 
	for i in range(0,w):
		for j in range(0,h):
			img1[i][j] = round(255*cve[img1[i][j]])
	return img1
##################################################################################
temp = histogramEqualized(temp)
redChannel = histogramEqualized(red)
blueChannel = histogramEqualized(blue)
greenChannel = histogramEqualized(green)


#cv2.imwrite( "Histequalised.png", img1)
#print "size(prob)->", len(prob), "size(cve)->", len(cve)
#print "histeql->", histeql

#print "size(hist)->", len(hist)
#cv2.namedWindow("original")
#cv2.namedWindow("gray")
#cv2.namedWindow("histogramEqualisation")
#cv2.imshow("original",img)
# cv2.waitKey(0)
# print redChannel


# tempRed = img1[:]
# tempBlue = img1[:]
# tempGreen = img1[:]

tempComb = img1[:]

Equalized = cv2.merge((blueChannel,greenChannel,redChannel))
# tempComb[:,:,0] = redChannel[:]
# tempComb[:,:,1] = greenChannel[:]
# tempComb[:,:,2] = blueChannel[:]
# #cv2.imshow("gray",img1)
# #cv2.waitKey(0)
# tempRed[:,:,0] = redChannel[:]
# tempRed[:,:,1] = 0
# tempRed[:,:,2] = 0

# tempGreen[:,:,1] = greenChannel[:]
# tempGreen[:,:,0] = 0
# tempGreen[:,:,2] = 0

# tempBlue[:,:,2] = blueChannel[:]
# tempBlue[:,:,0] = 0
# tempBlue[:,:,1] = 0

'''redChannel=redChannel.astype(np.uint8)
cv2.imshow("Red2 histogramEqualisation",redChannel)
cv2.imshow("original",img)'''


#temp=temp.astype(np.uint8)
#cv2.imshow("gray histogramEqualisation",temp)


Equalized=Equalized.astype(np.uint8)
cv2.imshow("Combined histogramEqualisation",Equalized)

#redChannel=redChannel.astype(np.uint8)
#cv2.imshow("Red histogramEqualisation",redChannel)

#blueChannel = blueChannel.astype(np.uint8)
#cv2.imshow("Blue histogramEqualisation",blueChannel)

#greenChannel = greenChannel.astype(np.uint8)
#cv2.imshow("Green histogramEqualisation",greenChannel)

cv2.waitKey(0)
cv2.destroyAllWindows()
#print img

