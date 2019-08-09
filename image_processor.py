import cv2
import numpy as np

image = cv2.imread("dataset/sudoku1.png", 1)
gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
gray = cv2.adaptiveThreshold(gray, 255, cv2.ADAPTIVE_THRESH_GAUSSIAN_C, cv2.THRESH_BINARY, 11, 2)
edges = cv2.Canny(image, 100, 200)

lines = cv2.HoughLines(edges, 1, np.pi/180, 200)
for line in lines:
    for rho, theta in line:
        a = np.cos(theta)
        b = np.sin(theta)
        x0 = a * rho
        y0 = b * rho
        x1 = int(x0 + 1000*(-b))
        y1 = int(y0 + 1000*a)
        x2 = int(x0 - 1000*(-b))
        y2 = int(y0 - 1000*a)
        cv2.line(image, (x1,y1), (x2,y2), (0,0,255), 2)

#cv2.imshow("edges", edges)
cv2.imshow("image", image)
cv2.waitKey(0)
cv2.destroyAllWindows()