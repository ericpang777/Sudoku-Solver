import cv2
import numpy as np
import pytesseract
from PIL import Image
from tensorflow import keras


def process_image(src):
    image = cv2.imread(src)
    image = cv2.resize(image, (700, (int)(700/np.size(image, 1) * np.size(image, 0)))) # 700 is x resolution, maintain aspect ratio
    image_edit = cv2.medianBlur(image, 5)
    image_edit = cv2.cvtColor(image_edit, cv2.COLOR_BGR2GRAY)
    image_edit = cv2.adaptiveThreshold(image_edit, 255, cv2.ADAPTIVE_THRESH_GAUSSIAN_C, cv2.THRESH_BINARY, 11, 2)
    image_edit = cv2.bitwise_not(image_edit)
    image_edit = cv2.dilate(image_edit, np.ones((2, 2), np.uint8), iterations=1)
    return image, image_edit


def find_max_area_contour(image):
    contours, hierarchy = cv2.findContours(image, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    c_max = max(contours, key=cv2.contourArea)
    return c_max


def find_corners(c_max):
    c_list = c_max.tolist()

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
            bot_right = (x, y)

    return [top_left, top_right, bot_left, bot_right]


def extract_sudoku(image_edit, image):
    c_max = find_max_area_contour(image_edit)
    corners_sudoku = find_corners(c_max)
    corners_sudoku = np.float32(corners_sudoku)
    corners_image = np.float32([[0,0], [700,0], [0,700], [700,700]])
    matrix = cv2.getPerspectiveTransform(corners_sudoku, corners_image)
    image = cv2.warpPerspective(image, matrix, (700, 700))
    return image


def remove_gridlines(image):
    edges = cv2.Canny(image, 50, 80, apertureSize=3)

    lines = cv2.HoughLines(edges, 1, np.pi / 180, 200)
    for line in lines:
        for rho, theta in line:
            a = np.cos(theta)
            b = np.sin(theta)
            x0 = a * rho
            y0 = b * rho
            x1 = int(x0 + 1000*(-b))
            y1 = int(y0 + 1000*(a))
            x2 = int(x0 - 1000*(-b))
            y2 = int(y0 - 1000*(a))

            cv2.line(image, (x1, y1), (x2, y2), (255, 255, 255), 5)
    return image


def extract_numbers(image):
    model = keras.models.load_model("models/mnist_model.h5")
    image = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    image = cv2.bitwise_not(image)
    grid = []

    for i in range(9):
        for j in range(9):
            y = (int)(i * 700/9)
            x = (int)(j * 700/9)
            image_crop = image[y:y+77, x:x+77]

            image_crop = cv2.dilate(image_crop, np.ones((2, 2), np.uint8), iterations=1)
            contours, hierarchy = cv2.findContours(image_crop, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
            if len(contours) > 0:
                c_max = max(contours, key=cv2.contourArea)
                for k in range(len(contours)):
                    if contours[k].all() != c_max.all():
                        image_crop = cv2.drawContours(image_crop, contours, k, (0, 0, 0), 3)

            ret, image_crop = cv2.threshold(image_crop, 127, 255, cv2.THRESH_BINARY_INV)
            image_crop = cv2.resize(image_crop, (40, 40))
            print(cv2.countNonZero(image_crop))
            if cv2.countNonZero(image_crop) < 1550: # Arbitrary number to filter, less than 40^2 = 1600
                cv2.imshow((str)(i*9 + j), image_crop)
                # For Tensorflow model
                #image_crop = np.asarray(image_crop)
                #image_crop = np.expand_dims(image_crop, axis=0)
                #image_crop = np.expand_dims(image_crop, axis=3)
                #grid.append(model.predict(image_crop).argmax())
                image_crop = cv2.cvtColor(image_crop, cv2.COLOR_BGR2RGB)
                grid.append(pytesseract.image_to_string(image_crop, config="--psm 10"))
    return grid


def read_file(file_name):
    sudoku = []
    file = open(file_name, "r")
    for line in file:
        split = line.rstrip()[:-1].split(" ")
        sudoku.append(list(map(int, split)))
    return sudoku


image, image_edit = process_image("img_sudoku/sudoku1.jpg")
image_edit = extract_sudoku(image_edit, image)
cv2.imshow("b", image_edit)
image_edit = remove_gridlines(image_edit)
a = extract_numbers(image_edit)
print(a)
'''
b = read_file("sudoku.txt")
b = [i for list in b for i in list]
print(a)
print(b)
print(a == b)
'''
cv2.imshow("image", image)
cv2.imshow("edit", image_edit)
cv2.waitKey(0)
cv2.destroyAllWindows()