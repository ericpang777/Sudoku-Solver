import cv2
import numpy as np

image = cv2.imread("img_sudoku/sudoku3.jpg", 1)
image = cv2.resize(image, (700, (int)(700/np.size(image, 1) * np.size(image, 0)))) # 700 is x resolution, maintain aspect ratio
image = cv2.medianBlur(image, 5)
gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
gray = cv2.adaptiveThreshold(gray, 255, cv2.ADAPTIVE_THRESH_GAUSSIAN_C, cv2.THRESH_BINARY, 11, 2)
erode = cv2.erode(gray, np.ones((2,2), np.uint8), iterations=1)
edges = cv2.Canny(image, 100, 175)

contours, hierarchy = cv2.findContours(edges, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
max_c = max(contours, key=cv2.contourArea)
cv2.drawContours(image, max_c, -1, (0,255,0), 3)

cv2.imshow("image", image)
cv2.waitKey(0)
cv2.destroyAllWindows()