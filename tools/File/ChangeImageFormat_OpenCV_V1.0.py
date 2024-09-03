# Change image format-OpenCV版
# 修改图像格式
import cv2

if __name__ == '__main__':
    # 使用OpenCV读取图像
    # 注意：OpenCV以BGR格式读取图像，但保存时会正确处理格式
    image = cv2.imread(r"")

    # 保存图像到新的格式
    # 注意：通过文件扩展名来指定新格式，OpenCV不需要显式指定格式，因为它会根据文件扩展名自动推断
    cv2.imwrite(r"", image)
