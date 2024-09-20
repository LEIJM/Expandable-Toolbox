import tkinter as tk
from tkinter import filedialog, messagebox
import os
import hashlib
import concurrent.futures
from datetime import datetime


def preprocess_filenames(directory):
    """
    遍历指定的文件夹中的所有文件名，查看是否存在文件名全是数字且该数字不大于该文件夹的文件总数，
    若存在，则在原文件名后，文件后缀前添加下划线“_”来重命名。
    """
    files = [f for f in os.listdir(directory) if os.path.isfile(os.path.join(directory, f))]
    num_files = len(files)

    for f in files:
        base, ext = os.path.splitext(f)
        if base.isdigit() and int(base) <= num_files:
            new_name = f"{base}_"
            new_name += ext
            os.rename(os.path.join(directory, f), os.path.join(directory, new_name))


def calculate_sha256(file_path):
    """
    计算文件的SHA-256哈希值。
    """
    sha256_hash = hashlib.sha256()
    with open(file_path, "rb") as f:
        for byte_block in iter(lambda: f.read(4096), b""):
            sha256_hash.update(byte_block)
    return sha256_hash.hexdigest()


def get_creation_time(file_path):
    """
    获取文件的创建时间。
    """
    creation_time = os.path.getctime(file_path)
    return datetime.fromtimestamp(creation_time)


def get_last_modified_time(file_path):
    """
    获取文件最后一次修改的时间。
    """
    last_modified_time = os.path.getmtime(file_path)
    return datetime.fromtimestamp(last_modified_time)


def get_file_size(file_path):
    """
    获取文件的大小。
    """
    return os.path.getsize(file_path)


def rename_files(directory, sort_by='hash', reverse=False):
    """
    按哈希值、创建时间、最后一次修改时间、文件大小或文件名从小到大或从大到小对指定文件夹内的文件进行重命名，使用多线程计算哈希值或获取时间/大小/名称。
    """
    # 获取文件夹内的文件列表
    files = [f for f in os.listdir(directory) if os.path.isfile(os.path.join(directory, f))]

    # 使用多线程计算哈希值或获取时间/大小/名称
    with concurrent.futures.ThreadPoolExecutor() as executor:
        futures = {}
        files_with_sorting = []
        for f in files:
            file_path = os.path.join(directory, f)
            if sort_by == 'name':
                # 按文件名排序，不需要使用线程池
                sorting_key = os.path.splitext(f)[0]
                files_with_sorting.append((sorting_key, f))
            else:
                # 其他排序规则，使用线程池
                if sort_by == 'hash':
                    future = executor.submit(calculate_sha256, file_path)
                elif sort_by == 'ctime':
                    future = executor.submit(get_creation_time, file_path)
                elif sort_by == 'mtime':
                    future = executor.submit(get_last_modified_time, file_path)
                elif sort_by == 'size':
                    future = executor.submit(get_file_size, file_path)
                else:
                    raise ValueError("排序规则无效，请使用“hash”、“ctime”、“mtime”、“size”或“name”。")

                futures[future] = f

        # 等待所有任务完成
        for future in concurrent.futures.as_completed(futures):
            try:
                result = future.result()
                files_with_sorting.append((result, futures[future]))
            except Exception as e:
                print(f"Error processing {futures[future]}: {e}")

    # 对文件进行排序
    files_with_sorting.sort(key=lambda x: x[0], reverse=reverse)

    # 计算序号的最小位数
    num_files = len(files_with_sorting)
    num_digits = len(str(num_files)) if num_files > 0 else 1

    # 重命名文件
    for idx, (sorting_key, original_filename) in enumerate(files_with_sorting, start=1):
        base, ext = os.path.splitext(original_filename)
        new_filename = f"{idx:0{num_digits}d}{ext}"
        new_file_path = os.path.join(directory, new_filename)

        # 重命名文件
        os.rename(os.path.join(directory, original_filename), new_file_path)


# GUI界面
class RenameApp(tk.Tk):
    def __init__(self):
        super().__init__()
        self.title("文件重命名工具")
        self.geometry("400x200")

        # 创建上中下底四部分
        self.upper_frame = tk.Frame(self)
        self.upper_frame.pack(fill=tk.X)

        self.middle_frame = tk.Frame(self)
        self.middle_frame.pack(fill=tk.X)

        self.lower_frame = tk.Frame(self)
        self.lower_frame.pack(fill=tk.X)

        self.status_bar = tk.Label(self, text="", bd=1, relief=tk.SUNKEN, anchor=tk.W)
        self.status_bar.pack(side=tk.BOTTOM, fill=tk.X)

        # 上部：文件夹路径输入和选择按钮
        self.path_entry = tk.Entry(self.upper_frame, width=40)
        self.path_entry.pack(side=tk.LEFT, padx=10, pady=10)

        self.browse_button = tk.Button(self.upper_frame, text="选择文件夹", command=self.browse_directory)
        self.browse_button.pack(side=tk.RIGHT, padx=10, pady=10)

        # 中部左侧：下拉选项，选择排序规则
        self.sort_by_var = tk.StringVar()
        self.sort_by_var.set("请选择排序规则")
        self.sort_by_options = ["哈希值", "创建时间", "修改时间", "文件大小", "原文件名"]
        self.sort_by_menu = tk.OptionMenu(self.middle_frame, self.sort_by_var, *self.sort_by_options)
        self.sort_by_menu.pack(side=tk.LEFT, padx=10, pady=10)

        # 中部右侧：倒序排序选项
        self.reverse_var = tk.BooleanVar()
        self.reverse_check = tk.Checkbutton(self.middle_frame, text="倒序排序", variable=self.reverse_var)
        self.reverse_check.pack(side=tk.RIGHT, padx=10, pady=10)

        # 下部：处理按钮
        self.process_button = tk.Button(self.lower_frame, text="处理", command=self.process_files)
        self.process_button.pack(pady=10)

    def browse_directory(self):
        directory = filedialog.askdirectory()
        if directory:
            self.path_entry.delete(0, tk.END)
            self.path_entry.insert(0, directory)

    def process_files(self):
        directory = self.path_entry.get()
        if not directory or not os.path.isdir(directory):
            messagebox.showerror("错误", "请选择一个有效的文件夹路径")
            return

        sort_by = self.sort_by_options.index(self.sort_by_var.get())
        reverse = self.reverse_var.get()
        sort_by_values = ["hash", "ctime", "mtime", "size", "name"]
        sort_by_param = sort_by_values[sort_by]

        # 显示状态栏
        self.status_bar.config(text="处理中...", fg="green")
        self.status_bar.pack()

        # 这里调用之前定义的函数，为了简化，我们假设这些函数已经实现
        preprocess_filenames(directory)
        rename_files(directory, sort_by=sort_by_param, reverse=reverse)

        # 隐藏状态栏
        self.status_bar.pack_forget()
        messagebox.showinfo("完成", "重命名完成")


if __name__ == "__main__":
    app = RenameApp()
    app.mainloop()
