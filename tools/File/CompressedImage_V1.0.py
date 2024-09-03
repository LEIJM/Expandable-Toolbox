# Compressed image
# 压缩图像
from PIL import Image

def compress_image(input_image_path, output_image_path, quality=85):
    """
    压缩图像并保存。
    :param input_image_path: 输入图像的路径
    :param output_image_path: 输出图像的路径
    :param quality: JPEG图像质量，范围从1（最差）到95（最佳），默认85
    """
    # 打开图像
    img = Image.open(input_image_path)

    # 保存图像，JPEG格式，设置质量
    img.save(output_image_path, "JPEG", optimize=True, quality=quality)

if __name__ == "__main__":
    input_image = r""  # 替换为你的图像路径
    output_image = r""  # 替换为你希望保存压缩图像的路径
    quality = 85  # 压缩质量，范围从1（最差）到95（最佳）调整quality参数来尝试不同的压缩级别
    compress_image(input_image, output_image, quality)
