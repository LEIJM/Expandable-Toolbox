# Compressed image
# 压缩图像
from PIL import Image
import os

def compress_image(input_image_path, quality=85):
    """
    压缩图像并保存。
    :param input_image_path: 输入图像的路径
    :param quality: JPEG图像质量，范围从1（最差）到95（最佳），默认85
    """
    # 确保输入的图像文件路径不为空
    if not input_image_path:
        raise ValueError("输入的图像文件路径不能为空。")

    # 获取输入图像文件的目录和文件名
    input_image_dir, input_image_filename = os.path.split(input_image_path)
    # 创建输出图像文件的文件名
    output_image_filename = input_image_filename.rsplit('.', 1)[0] + '_out.jpg'
    # 拼接输出图像文件的完整路径
    output_image_path = os.path.join(input_image_dir, output_image_filename)

    # 打开图像
    img = Image.open(input_image_path)

    # 保存图像，JPEG格式，设置质量
    img.save(output_image_path, "JPEG", optimize=True, quality=quality)

if __name__ == "__main__":
    input_image = input("请输入图像文件的路径：")  # 用户输入图像路径
    quality = int(input("请输入压缩质量（1-95）："))  # 用户输入压缩质量

    # 检查quality是否在有效范围内
    if not 1 <= quality <= 95:
        print("压缩质量必须在1到95之间。")
    else:
        compress_image(input_image, quality)
        print("图像压缩完成！")

