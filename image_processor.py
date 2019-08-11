import cv2
import numpy as np

image = cv2.imread("img_sudoku/sudoku3.jpg", 1)
image = cv2.resize(image, (700, (int)(700/np.size(image, 1) * np.size(image, 0)))) # 700 is x resolution, maintain aspect ratio
image_edit = cv2.medianBlur(image, 5)
image_edit = cv2.cvtColor(image_edit, cv2.COLOR_BGR2GRAY)
image_edit = cv2.adaptiveThreshold(image_edit, 255, cv2.ADAPTIVE_THRESH_GAUSSIAN_C, cv2.THRESH_BINARY, 11, 2)
image_edit = cv2.bitwise_not(image_edit)
image_edit = cv2.dilate(image_edit, np.ones((2, 2), np.uint8), iterations=1)

contours, hierarchy = cv2.findContours(image_edit, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
max_c = max(contours, key=cv2.contourArea)
c_list = max_c.tolist()

# choose the opposite 4 corners
top_left = (700, 700)
top_right = (0, 700)
bot_left = (700, 0)
bot_right = (0, 0)

for point in c_list:
    x = point[0][0] # point is stored as [[x, y]]
    y = point[0][1]
    if x + y < top_left[0] + top_left[1]:
        top_left = (x, y)
    if x - y > top_right[0] - top_right[1]:
        top_right = (x, y)
    if x - y < bot_left[0] - bot_left[1]:
        bot_left = (x, y)
    if x + y > bot_right[0] + bot_right[1]:
        bot_right = (x ,y)

cv2.drawContours(image, max_c, -1, (0,255,0), 3)
cv2.imshow("image", image)
cv2.waitKey(0)
cv2.destroyAllWindows()