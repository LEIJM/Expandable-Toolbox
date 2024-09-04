import os
import fitz  # PyMuPDF 库
from tqdm import tqdm  # 用于显示进度条

def convert_images_to_pdf(image_dir):
    # 创建一个新的 PDF 文档
    pdf_doc = fitz.open()

    # 获取该目录下的所有图片文件,并按名称排序
    image_files = sorted([f for f in os.listdir(image_dir) if f.endswith((".png", ".jpg", ".jpeg"))])
    total_files = len(image_files)

    # 创建进度条对象
    progress_bar = tqdm(total=total_files, desc="Processing images", unit="file", ncols=80)

    # 遍历所有图片文件,并将其添加到 PDF 文档中
    for image_file in image_files:
        image_path = os.path.join(image_dir, image_file)
        page = pdf_doc.new_page()
        page.insert_image(page.rect, filename=image_path)
        progress_bar.update(1)  # 更新进度条

    # 获取输出文件的路径和名称
    output_dir = os.path.dirname(image_dir)
    output_filename = os.path.basename(image_dir) + "_out.pdf"
    output_path = os.path.join(output_dir, output_filename)

    # 保存 PDF 文档
    pdf_doc.save(output_path)
    progress_bar.close()  # 关闭进度条
    print(f"PDF 文档已生成: {output_path}")


if __name__ == "__main__":
    # 指定图片文件所在的路径
    image_dir = input("请输入图片文件夹路径:\n")
    # 调用函数进行转换
    convert_images_to_pdf(image_dir)