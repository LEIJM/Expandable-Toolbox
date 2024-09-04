import fitz  # PyMuPDF
import os


def pdf_to_images(pdf_path, zoom_x=2.0, zoom_y=2.0):
    # 获取PDF文件所在目录和文件名
    pdf_dir, pdf_filename = os.path.split(pdf_path)

    # 获取PDF文件名（去除后缀）
    pdf_name = os.path.splitext(pdf_filename)[0]

    # 创建输出文件夹路径
    output_folder = os.path.join(pdf_dir, f"{pdf_name}_out")

    # 如果输出文件夹不存在，创建它
    if not os.path.exists(output_folder):
        os.makedirs(output_folder)

    # 打开PDF文件
    doc = fitz.open(pdf_path)

    # 遍历每一页
    for page_num in range(len(doc)):
        # 获取页面对象
        page = doc.load_page(page_num)

        # 设置缩放系数
        mat = fitz.Matrix(zoom_x, zoom_y)

        # 将页面转换为图像
        pix = page.get_pixmap(matrix=mat)

        # 保存图像为PNG文件
        output_path = os.path.join(output_folder, f"page_{page_num + 1}.png")
        pix.save(output_path)

        print(f"Saved page {page_num + 1} as {output_path}")

    # 关闭文档
    doc.close()


if __name__ == "__main__":
    # PDF文件路径
    pdf_path = r""

    # 运行PDF转图片函数
    pdf_to_images(pdf_path)