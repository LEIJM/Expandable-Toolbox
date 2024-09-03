# Change image format-PIL版
# 修改图像格式
from PIL import Image

if __name__ == '__main__':
    original_image = Image.open(r"")  # 请确保这是你的图像文件的正确路径

    # 检查图像模式是否为RGBA，如果是，则转换为RGB
    if original_image.mode == 'RGBA':
        rgb_image = original_image.convert('RGB')
    else:
        # 如果图像已经是RGB模式或其他JPEG支持的模式，则直接使用
        rgb_image = original_image

        # 现在可以安全地将图像保存为JPEG了
    rgb_image.save(r"", 'PNG')
