import tkinter as tk
from tkinter import filedialog, ttk
import os
import fitz  # PyMuPDF 库
import threading

class ImageToPdfGUI:
    def __init__(self, master):
        self.master = master
        master.title("Image to PDF Converter")
        master.geometry("500x120")
        master.resizable(False, False)

        # 上部分：文件路径输入
        self.path_frame = tk.Frame(master)
        self.path_frame.pack(pady=10, padx=10, fill=tk.X)

        self.path_entry = tk.Entry(self.path_frame)
        self.path_entry.pack(side=tk.LEFT, expand=True, fill=tk.X)

        self.browse_button = tk.Button(self.path_frame, text="浏览", command=self.browse_folder, width=8)
        self.browse_button.pack(side=tk.LEFT, padx=(5, 0))

        # 处理按钮
        self.process_button = tk.Button(master, text="处理", command=self.start_processing, width=10)
        self.process_button.pack(pady=10)

        # 状态栏
        self.status_frame = tk.Frame(master, height=40)  # 设置高度为40，以容纳两行文本
        self.status_frame.pack(fill=tk.X, pady=(0, 10), padx=10)
        self.status_frame.pack_forget()

        self.status_bar = tk.Label(self.status_frame, text="", bd=0, relief=tk.FLAT, anchor=tk.W, bg='SystemButtonFace', wraplength=480)
        self.status_bar.pack(side=tk.LEFT, fill=tk.X, expand=True)

        self.progress_bar = ttk.Progressbar(self.status_frame, orient="horizontal", length=100, mode="determinate")
        self.progress_bar.pack(side=tk.LEFT, padx=(5, 0))

    def browse_folder(self):
        folder_path = filedialog.askdirectory()
        self.path_entry.delete(0, tk.END)
        self.path_entry.insert(0, folder_path)

    def start_processing(self):
        image_dir = self.path_entry.get()
        if not image_dir:
            self.show_status("请选择文件夹", "red")
            return

        self.process_button.config(state=tk.DISABLED)
        self.show_status("处理中...", "green")
        self.progress_bar["value"] = 0
        self.status_frame.pack(fill=tk.X, pady=(0, 10), padx=10)

        # 在新线程中运行处理函数
        thread = threading.Thread(target=self.convert_images_to_pdf, args=(image_dir,))
        thread.start()

    def convert_images_to_pdf(self, image_dir):
        # 创建一个新的 PDF 文档
        pdf_doc = fitz.open()

        # 获取该目录下的所有图片文件,并按名称排序
        image_files = sorted([f for f in os.listdir(image_dir) if f.endswith((".png", ".jpg", ".jpeg"))])
        total_files = len(image_files)

        # 遍历所有图片文件,并将其添加到 PDF 文档中
        for i, image_file in enumerate(image_files):
            image_path = os.path.join(image_dir, image_file)
            page = pdf_doc.new_page()
            page.insert_image(page.rect, filename=image_path)
            self.update_progress((i + 1) / total_files * 100, f"处理中: {i + 1}/{total_files}")

            # 获取输出文件的路径和名称
        output_dir = os.path.dirname(image_dir)
        output_filename = os.path.basename(image_dir) + "_out.pdf"
        output_path = os.path.join(output_dir, output_filename)

        # 保存 PDF 文档
        pdf_doc.save(output_path)
        self.update_progress(100, f"PDF 文档已生成: {output_path}")
        self.process_button.config(state=tk.NORMAL)

    def show_status(self, message, color="green"):
        self.status_bar.config(text=message, fg=color)
        if not self.status_frame.winfo_viewable():
            self.status_frame.pack(fill=tk.X, pady=(0, 10), padx=10)
        self.master.update_idletasks()

    def update_progress(self, value, message):
        self.master.after(0, self.progress_bar.config, {"value": value})
        self.master.after(0, self.status_bar.config, {"text": message})

def main():
    root = tk.Tk()
    gui = ImageToPdfGUI(root)
    root.mainloop()

if __name__ == "__main__":
    main()