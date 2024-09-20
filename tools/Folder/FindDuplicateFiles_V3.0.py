import tkinter as tk
from tkinter import filedialog, messagebox
import os
import shutil
import hashlib
import time


def calculate_sha256(file_path):
    """计算文件的SHA-256哈希值"""
    hash_sha256 = hashlib.sha256()
    with open(file_path, "rb") as f:
        for chunk in iter(lambda: f.read(4096), b""):
            hash_sha256.update(chunk)
    return hash_sha256.hexdigest()


def is_older(file1, file2):
    """
    判断file1是否比file2旧
    """
    return os.path.getmtime(file1) > os.path.getmtime(file2)


def process_directory(directory, status_bar):
    """处理指定目录，找出重复文件并移动较新的文件到'重复文件'文件夹"""
    file_hashes = {}
    duplicates = []
    total_files = 0
    processed_files = 0
    duplicate_dir = None

    # 遍历目录中的文件
    for root, dirs, files in os.walk(directory):
        total_files += len(files)

    for root, dirs, files in os.walk(directory):
        for file in files:
            file_path = os.path.join(root, file)
            file_hash = calculate_sha256(file_path)
            # 如果哈希值已存在，说明是重复文件
            if file_hash in file_hashes:
                # 记录重复文件
                duplicates.append((file_path, file_hashes[file_hash]))
                # 保留日期较早的文件
                if is_older(file_path, file_hashes[file_hash]):
                    file_hashes[file_hash] = file_path
                else:
                    # 将日期较新的文件移动到'重复文件'文件夹
                    if duplicate_dir is None:  # 如果duplicate_dir尚未定义，则定义它
                        duplicate_dir = os.path.join(directory, '重复文件')
                        os.makedirs(duplicate_dir, exist_ok=True)
                    duplicate_file_name = os.path.join(duplicate_dir, file)
                    shutil.move(file_path, duplicate_file_name)
                    status_bar.config(text=f"已移动重复文件: {file_path} -> {duplicate_file_name}")
                    status_bar.update_idletasks()
            else:
                # 记录文件哈希值
                file_hashes[file_hash] = file_path
            processed_files += 1
            status_bar.config(text=f"处理进度: {processed_files}/{total_files}")
            status_bar.update_idletasks()

    # 如果没有重复文件，更新状态栏
    if not duplicates:
        status_bar.config(text="无重复文件")
    else:
        status_bar.config(text="已处理完成")

    return duplicates, duplicate_dir


class DuplicateFileFinderApp:
    def __init__(self, root):
        self.root = root
        self.root.title("查找重复文件")

        # 创建一个框架来放置输入框和按钮
        self.folder_frame = tk.Frame(root)
        self.folder_frame.pack(fill=tk.X)

        # 输入框
        self.folder_path_entry = tk.Entry(self.folder_frame, width=50)
        self.folder_path_entry.pack(side=tk.LEFT, padx=5, pady=5)

        # “选择文件夹”按钮
        self.folder_select_button = tk.Button(self.folder_frame, text="选择文件夹", command=self.select_folder)
        self.folder_select_button.pack(side=tk.LEFT, padx=5, pady=5)

        # 中部：处理按钮
        self.process_button = tk.Button(root, text="处理", command=self.process_files)
        self.process_button.pack(pady=5)

        # 下部：状态栏
        self.status_bar = tk.Label(root, text="", fg="green")
        self.status_bar.pack()

    def select_folder(self):
        folder_selected = filedialog.askdirectory()
        if folder_selected:
            self.folder_path_entry.delete(0, tk.END)
            self.folder_path_entry.insert(0, folder_selected)

    def process_files(self):
        target_directory = self.folder_path_entry.get()
        if not target_directory:
            messagebox.showerror("错误", "请选择一个文件夹")
            return

        self.status_bar.config(text="处理中，请稍候...")
        self.root.update_idletasks()
        duplicates, duplicate_dir = process_directory(target_directory, self.status_bar)
        end_time = time.time()
        if duplicates:
            messagebox.showinfo("完成", f"处理完成。重复文件已移动到: {duplicate_dir}")
        else:
            messagebox.showinfo("完成", "无重复文件")
        self.status_bar.config(text=f"共花费 {end_time - self.start_time:.2f} 秒。")


if __name__ == "__main__":
    root = tk.Tk()
    app = DuplicateFileFinderApp(root)
    app.start_time = time.time()  # 在这里初始化开始时间
    root.mainloop()
