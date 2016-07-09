#!/usr/bin/python
import random
import glob
import os
from PIL import Image

# Sea background is transparent
color_map = [
    (0,0,0), # 0 black
    (29,214,160), # 1 midgreen
    (162,252,226), # 2 light green
    (24,162,105), # 3 dark green
    (208,139,231), # 4 mid purple
    (190,110,216), # 5 dark purple
    (241,197,255), # 6 light purple
    (52,88,223), # 7 dark blue
    (133,158,252), # 8 blue
]

class ImageProcessor:

    def get_images(self):
        images = []
        images_files = glob.glob('raws/*.png')
        for img in images_files:
            images.append([Image.open(img), img])
        return images

    def make_images_array(self, images):
        all_images = []
        for img in images:
            processed = self.img_to_matrix(img[0])
            file_name = os.path.basename(img[1])[:-4]
            img_data = [file_name, processed]
            all_images.append(img_data)
        return all_images

    def img_to_matrix(self, image):
        image = image.convert("RGB")

        pixel_data = image.load()
        image_array = [image.size[1]*image.size[0],image.size[0]]
        for y in xrange(image.size[1]):
            for x in xrange(image.size[0]):
                try:
                    index = color_map.index(pixel_data[x, y])
                except:
                    index = 0
                image_array.append(index)
        return image_array

    def save_img_data(self, data):
        h = open ('file_parts/header.txt', 'r')
        f = open('processed/img_data.txt', 'w')

        file_content = ''
        for line in h:
            file_content += line

        for img in data:
            file_content += '\nint '+img[0]+'[] = \n'

            img_data = ''
            img_data ='    {'+str(img[1])[1:-1].replace(" ", "")+'},\n'
            file_content += img_data[:-2]
            file_content += ';'
        f.write(file_content[:-2]+';')

imageProcessor = ImageProcessor()

images = imageProcessor.get_images()
img_data =  imageProcessor.make_images_array(images)
imageProcessor.save_img_data(img_data)
