# Compressed image
# 压缩图像
import tkinter as tk
from tkinter import filedialog, messagebox
from PIL import Image
import os


def compress_image(input_image_path, quality):
    try:
        # 获取输入图像文件的目录和文件名
        input_image_dir, input_image_filename = os.path.split(input_image_path)
        # 获取文件后缀
        file_extension = input_image_filename.rsplit('.', 1)[1].lower()
        # 仅支持处理JPG和JPEG文件
        if file_extension not in ['jpg', 'jpeg']:
            raise ValueError("暂时仅支持处理JPG和JPEG文件。")
        # 创建输出图像文件的文件名
        output_image_filename = input_image_filename.rsplit('.', 1)[0] + '_out.' + file_extension
        # 拼接输出图像文件的完整路径
        output_image_path = os.path.join(input_image_dir, output_image_filename)

        # 打开图像
        img = Image.open(input_image_path)
        # 保存图像，保留原格式，设置质量
        img.save(output_image_path, optimize=True, quality=quality)
        return True
    except Exception as e:
        messagebox.showerror("错误", f"处理图像时发生错误: {e}")
        return False


def browse_file():
    # 打开文件选择对话框
    filepath = filedialog.askopenfilename()
    if filepath:
        # 清空文本框并插入新路径
        file_entry.delete(0, tk.END)
        file_entry.insert(0, filepath)


def process_image():
    input_image_path = file_entry.get()
    quality = quality_entry.get()

    if not input_image_path:
        messagebox.showwarning("警告", "请输入图像文件路径或选择文件。")
        return
    if not quality.isdigit() or not 1 <= int(quality) <= 95:
        messagebox.showwarning("警告", "请输入有效的图像质量（1-95）。")
        return

    # 转换质量为整数
    quality = int(quality)
    # 调用压缩函数
    if compress_image(input_image_path, quality):
        status_label.config(text="处理完成")


# 创建主窗口
root = tk.Tk()
root.title("图像压缩工具")

# 创建文件路径输入框
file_entry = tk.Entry(root, width=50)
file_entry.pack(pady=10)

# 创建浏览按钮
browse_button = tk.Button(root, text="浏览", command=browse_file)
browse_button.pack(side=tk.LEFT, padx=10)

# 创建输出图像质量输入框
quality_label = tk.Label(root, text="输出图像质量（1-95）")
quality_entry = tk.Entry(root, width=10)
quality_label.pack(side=tk.LEFT, padx=10)
quality_entry.pack(side=tk.LEFT, padx=10)

# 创建处理按钮
process_button = tk.Button(root, text="处理", command=process_image)
process_button.pack(side=tk.RIGHT, padx=10)

# 创建状态标签
status_label = tk.Label(root, text="", fg="green")
status_label.pack(side=tk.BOTTOM, fill=tk.X, padx=10, pady=10)

# 在左下角添加注释
root.after(100, lambda: status_label.config(text="暂时仅支持处理JPG和JPEG文件"))

# 运行主循环
root.mainloop()
