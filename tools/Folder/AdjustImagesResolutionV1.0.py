# Adjust images resolution
# 批量调整图像分辨率
import os
from PIL import Image


def change_image(directory_path, size, cover=0):
    # 遍历目录中的所有文件
    for filename in os.listdir(directory_path):
        # 只处理图片文件
        if filename.lower().endswith(('.png', '.jpg', '.jpeg', '.bmp', '.gif')):
            input_image_path = os.path.join(directory_path, filename)
            output_image_path = input_image_path

            # 如果cover为0，则创建新的文件名
            if not cover:
                output_filename = os.path.splitext(filename)[0] + "_out" + os.path.splitext(filename)[1]
                output_image_path = os.path.join(directory_path, output_filename)

            with Image.open(input_image_path) as image:
                resized_image = image.resize(size, Image.LANCZOS)
                resized_image.save(output_image_path)


if __name__ == "__main__":
    directory_path = r""  # 图片目录路径
    new_size = (600, 1200)  # 设置新的分辨率
    cover = 1  # 设置为1覆盖源文件，设置为0则不覆盖
    change_image(directory_path, new_size, cover)
    print("图片尺寸调整完成")
