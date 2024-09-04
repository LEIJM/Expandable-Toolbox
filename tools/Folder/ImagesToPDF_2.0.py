import os
import fitz  # pymupdf
from tqdm import tqdm  # 进度条库


def images_to_pdf(image_folder):
    # 获取指定文件夹的名称，并构造输出PDF的文件名
    folder_name = os.path.basename(os.path.normpath(image_folder))
    output_pdf = os.path.join(image_folder + "_out.pdf")

    # 获取文件夹中的所有图片文件，并按名称升序排序
    image_files = sorted(
        [f for f in os.listdir(image_folder) if f.lower().endswith(('.png', '.jpg', '.jpeg', '.bmp', '.gif', '.tiff'))])

    # 创建一个新的PDF文档
    pdf_document = fitz.open()

    # 使用 tqdm 来显示进度条
    with tqdm(total=len(image_files), desc="Processing Images", unit="image") as pbar:
        for i, image_file in enumerate(image_files):
            image_path = os.path.join(image_folder, image_file)

            # 打开图片并创建一个PDF页面
            img = fitz.Pixmap(image_path)
            rect = fitz.Rect(0, 0, img.width, img.height)
            pdf_page = pdf_document.new_page(width=img.width, height=img.height)

            # 插入图片到PDF页面
            pdf_page.insert_image(rect, pixmap=img)

            # 更新进度条
            pbar.update(1)
            pbar.set_postfix({'Progress': f'{((i + 1) / len(image_files)) * 100:.2f}%'})

    # 保存PDF文档
    pdf_document.save(output_pdf)
    pdf_document.close()
    print(f"\nPDF saved to: {output_pdf}")

if __name__ == '__main__':
    # 示例用法
    image_folder = input("请输入图片文件夹路径:\n")
    images_to_pdf(image_folder)