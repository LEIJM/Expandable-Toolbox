# Adjust image resolution
# 调整图像分辨率
import os
from PIL import Image


def change_image(input_image_path, size):
    # 获取输入图像的目录和文件名
    input_dir, input_filename = os.path.split(input_image_path)
    # 创建输出文件名
    output_filename = os.path.splitext(input_filename)[0] + "_out" + os.path.splitext(input_filename)[1]
    output_image_path = os.path.join(input_dir, output_filename)

    with Image.open(input_image_path) as image:
        resized_image = image.resize(size, Image.LANCZOS)
        resized_image.save(output_image_path)


if __name__ == "__main__":
    # 从控制台获取图片的路径
    image_path = input("请输入要处理的图片路径: \n")
    new_size = (600, 1200)  # 设置新的分辨率
    change_image(image_path, new_size)
    print("图片尺寸调整完成")
