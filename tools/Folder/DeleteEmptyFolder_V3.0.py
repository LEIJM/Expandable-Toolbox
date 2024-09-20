# Delete Empty Folder
# 删除空文件夹
import os
import tkinter as tk
from tkinter import filedialog, messagebox


def is_empty_dir(path):
    try:
        if not os.path.isdir(path):
            return False
        return not os.listdir(path)
    except OSError:
        return False


def remove_empty_dirs(path):
    if is_empty_dir(path):
        try:
            os.rmdir(path)
            return True
        except OSError:
            return False
    else:
        for item in os.listdir(path):
            item_path = os.path.join(path, item)
            if os.path.isdir(item_path):
                if remove_empty_dirs(item_path):
                    return True
    return False


def remove_empty_dirs_in_directory(directory):
    subdirectories = [os.path.join(directory, item) for item in os.listdir(directory) if os.path.isdir(os.path.join(directory, item))]
    any_empty_dir_removed = False
    for subdir in subdirectories:
        if remove_empty_dirs(subdir):
            any_empty_dir_removed = True
    return any_empty_dir_removed


def on_browse_button_click():
    directory_entry.delete(0, tk.END)  # 清空文本框内容
    selected_directory = filedialog.askdirectory()  # 打开文件选择器
    if selected_directory:  # 如果选择了目录，则将其路径填入文本框
        directory_entry.insert(tk.END, selected_directory)


def on_process_button_click():
    target_directory = directory_entry.get()
    if not target_directory:
        messagebox.showwarning("警告", "请输入或选择一个目录")
        return
    if remove_empty_dirs_in_directory(target_directory):
        status_label.config(text="处理完成，已删除空文件夹。")
    else:
        status_label.config(text="处理完成，无空文件夹。")


# 创建主窗口
root = tk.Tk()
root.title("空文件夹删除工具")

# 创建输入框和按钮
frame = tk.Frame(root)
frame.pack(pady=10)

directory_entry = tk.Entry(frame, width=50)
directory_entry.pack(side=tk.LEFT, padx=10)

browse_button = tk.Button(frame, text="浏览", command=on_browse_button_click)
browse_button.pack(side=tk.LEFT)

process_button = tk.Button(root, text="处理", command=on_process_button_click)
process_button.pack(pady=10)

# 创建状态标签
status_label = tk.Label(root, text="", fg="green")
status_label.pack(side=tk.BOTTOM, fill=tk.X, anchor=tk.SE, pady=10)

# 运行主循环
root.mainloop()
