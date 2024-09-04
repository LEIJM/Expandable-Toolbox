import fitz  # PyMuPDF
import os
from docx import Document


def pdf_to_word(pdf_path):
    # 获取PDF文件所在目录和文件名
    pdf_dir, pdf_filename = os.path.split(pdf_path)

    # 获取PDF文件名（去除后缀）
    pdf_name = os.path.splitext(pdf_filename)[0]

    # 创建输出Word文件路径
    output_path = os.path.join(pdf_dir, f"{pdf_name}_out.docx")

    # 打开PDF文件
    doc = fitz.open(pdf_path)

    # 创建一个新的Word文档
    word_doc = Document()

    # 遍历每一页
    for page_num in range(len(doc)):
        # 获取页面对象
        page = doc.load_page(page_num)

        # 提取页面文本
        text = page.get_text("text")

        # 将文本添加到Word文档中
        if text.strip():  # 仅当页面有文本时才添加
            word_doc.add_paragraph(text)
            word_doc.add_page_break()  # 在每页后添加分页符

        print(f"Processed page {page_num + 1} of {len(doc)}")

    # 保存Word文档
    word_doc.save(output_path)
    print(f"Saved Word document as {output_path}")

    # 关闭PDF文档
    doc.close()


if __name__ == "__main__":
    # PDF文件路径
    pdf_path = r""

    # 运行PDF转Word函数
    pdf_to_word(pdf_path)