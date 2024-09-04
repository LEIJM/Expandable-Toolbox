import os
import fitz  # pymupdf
import tkinter as tk
from tkinter import filedialog
from tkinter.ttk import Progressbar, Style

def images_to_pdf(image_folder, progress_var, progress_label, progress_bar, status_label, root):
    try:
        folder_name = os.path.basename(os.path.normpath(image_folder))
        output_pdf = os.path.join(image_folder + "_out.pdf")

        image_files = sorted(
            [f for f in os.listdir(image_folder) if f.lower().endswith(('.png', '.jpg', '.jpeg', '.bmp', '.gif', '.tiff'))])

        pdf_document = fitz.open()

        progress_var.set(0)
        progress_step = 100 / len(image_files) if image_files else 0

        for i, image_file in enumerate(image_files):
            image_path = os.path.join(image_folder, image_file)
            img = fitz.Pixmap(image_path)
            rect = fitz.Rect(0, 0, img.width, img.height)
            pdf_page = pdf_document.new_page(width=img.width, height=img.height)
            pdf_page.insert_image(rect, pixmap=img)

            progress_value = progress_var.get() + progress_step
            progress_var.set(progress_value)
            progress_label.config(text=f"{progress_value:.2f}%")
            root.update_idletasks()

        pdf_document.save(output_pdf)
        pdf_document.close()

        # 隐藏进度条和百分比标签
        progress_label.pack_forget()
        progress_bar.pack_forget()

        # 在原位置显示状态信息
        status_label.config(text=f"处理完成！PDF已保存到: {output_pdf}", fg="green")
        status_label.pack(fill=tk.X, expand=True)

    except Exception as e:
        # 隐藏进度条和百分比标签，并显示错误信息
        progress_label.pack_forget()
        progress_bar.pack_forget()
        status_label.config(text=f"错误: {str(e)}", fg="red")
        status_label.pack(fill=tk.X, expand=True)

def browse_folder(entry):
    folder_selected = filedialog.askdirectory()
    if folder_selected:
        entry.delete(0, tk.END)
        entry.insert(0, folder_selected)

def start_processing(entry, progress_var, progress_label, progress_bar, status_label, root):
    image_folder = entry.get()
    if os.path.isdir(image_folder):
        status_label.pack_forget()  # 隐藏之前的状态信息
        progress_label.config(text="0.00%")
        progress_var.set(0)
        progress_label.pack(side=tk.LEFT)  # 显示百分比标签
        progress_bar.pack(side=tk.LEFT, fill=tk.X, expand=True)  # 显示进度条
        root.update_idletasks()  # 更新界面
        images_to_pdf(image_folder, progress_var, progress_label, progress_bar, status_label, root)
    else:
        status_label.config(text="错误: 请输入有效的文件夹路径", fg="red")
        status_label.pack(fill=tk.X, expand=True)

def create_gui():
    root = tk.Tk()
    root.title("图片转PDF")

    # 设置字体样式
    style = Style()
    style.configure("TProgressbar", troughcolor='white', background='green')

    # 上部：文件路径输入框和浏览按钮
    frame_top = tk.Frame(root, padx=10, pady=10)
    frame_top.pack(fill=tk.X)

    entry = tk.Entry(frame_top, width=50)
    entry.pack(side=tk.LEFT, fill=tk.X, expand=True, padx=(0, 5))

    browse_button = tk.Button(frame_top, text="选择文件夹", command=lambda: browse_folder(entry))
    browse_button.pack(side=tk.LEFT)

    # 中部：处理按钮
    frame_middle = tk.Frame(root, padx=10, pady=10)
    frame_middle.pack(fill=tk.X)

    process_button = tk.Button(frame_middle, text="处理", command=lambda: start_processing(entry, progress_var, progress_label, progress_bar, status_label, root))
    process_button.pack()

    # 下部：状态栏（初始隐藏的进度条和百分比标签）
    frame_bottom = tk.Frame(root, padx=10, pady=10)
    frame_bottom.pack(fill=tk.X)

    progress_var = tk.DoubleVar()
    progress_label = tk.Label(frame_bottom, text="0.00%", fg="green")

    progress_bar = Progressbar(frame_bottom, variable=progress_var, maximum=100, style="TProgressbar")
    # 状态信息标签（初始隐藏）
    status_label = tk.Label(frame_bottom, text="", fg="green", anchor="w")

    root.mainloop()

if __name__ == '__main__':
    create_gui()