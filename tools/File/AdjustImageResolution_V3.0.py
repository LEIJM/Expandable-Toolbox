# Adjust image resolution
# 调整图像分辨率
import os
from PIL import Image
import tkinter as tk
from tkinter import filedialog, messagebox

# 调整图像分辨率
def change_image(input_image_path, size):
    input_dir, input_filename = os.path.split(input_image_path)
    output_filename = os.path.splitext(input_filename)[0] + "_out" + os.path.splitext(input_filename)[1]
    output_image_path = os.path.join(input_dir, output_filename)

    with Image.open(input_image_path) as image:
        resized_image = image.resize(size, Image.LANCZOS)
        resized_image.save(output_image_path)


# GUI界面函数
def on_process_button_click():
    global image_path_entry, width_entry, height_entry
    image_path = image_path_entry.get()
    width = int(width_entry.get())
    height = int(height_entry.get())
    new_size = (width, height)

    if not image_path:
        messagebox.showwarning("警告", "请输入或选择一个图片路径")
        return
    if not os.path.isfile(image_path):
        messagebox.showwarning("警告", "指定的路径不是一个有效的图片文件")
        return

    change_image(image_path, new_size)
    messagebox.showinfo("完成", "图片已成功调整尺寸")

def on_browse_button_click():
    global image_path_entry
    image_path_entry.delete(0, tk.END)  # 清空文本框内容
    selected_file = filedialog.askopenfilename()  # 打开文件选择器，选择文件而不是目录
    if selected_file:  # 如果选择了文件，则将其路径填入文本框
        image_path_entry.insert(tk.END, selected_file)

# 创建主窗口
root = tk.Tk()
root.title("图片尺寸调整工具")

# 创建输入框和按钮
frame = tk.Frame(root)
frame.pack(pady=10)

image_path_entry = tk.Entry(frame, width=50)
image_path_entry.pack(side=tk.LEFT, padx=10)

browse_button = tk.Button(frame, text="浏览", command=on_browse_button_click)
browse_button.pack(side=tk.LEFT)

# 创建中间部分输入宽度和高度
frame_middle = tk.Frame(root)
frame_middle.pack(pady=10)

width_label = tk.Label(frame_middle, text="宽:")
width_label.pack(side=tk.LEFT)

width_entry = tk.Entry(frame_middle, width=10)
width_entry.pack(side=tk.LEFT)

height_label = tk.Label(frame_middle, text="高:")
height_label.pack(side=tk.LEFT)

height_entry = tk.Entry(frame_middle, width=10)
height_entry.pack(side=tk.LEFT)

# 创建底部处理按钮
process_button = tk.Button(root, text="处理", command=on_process_button_click)
process_button.pack(pady=10)

# 运行主循环
root.mainloop()
