import cv2
import numpy as np


def process_image(src):
    image = cv2.imread(src)
    image = cv2.resize(image, (700, (int)(700/np.size(image, 1) * np.size(image, 0)))) # 700 is x resolution, maintain aspect ratio
    image_edit = cv2.medianBlur(image, 5)
    image_edit = cv2.cvtColor(image_edit, cv2.COLOR_BGR2GRAY)
    image_edit = cv2.adaptiveThreshold(image_edit, 255, cv2.ADAPTIVE_THRESH_GAUSSIAN_C, cv2.THRESH_BINARY, 11, 2)
    image_edit = cv2.bitwise_not(image_edit)
    image_edit = cv2.dilate(image_edit, np.ones((2, 2), np.uint8), iterations=1)
    return (image, image_edit)


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


def extract_sudoku(image, corners):
    corners_sudoku = np.float32(corners)
    corners_image = np.float32([[0,0], [700,0], [0,700], [700,700]])
    matrix = cv2.getPerspectiveTransform(corners_sudoku, corners_image)
    return cv2.warpPerspective(image, matrix, (700, 700))


def extract_numbers(image):
    grid = []
    for i in range(81):
        y = (int)(i//9 * 700/9)
        x = (int)(i%9 * 700/9)
        grid.append(image[y:y+77, x:x+77])
    return grid


image, image_edit = process_image("img_sudoku/sudoku3.jpg")
c_max = find_max_area_contour(image_edit)
corners = find_corners(c_max)
image_edit = extract_sudoku(image, corners)

cv2.drawContours(image, c_max, -1, (0,255,0), 3)
cv2.imshow("image", image)
cv2.imshow("edit", image_edit)
cv2.waitKey(0)
cv2.destroyAllWindows()