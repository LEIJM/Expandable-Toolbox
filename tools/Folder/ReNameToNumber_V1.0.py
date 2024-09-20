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
    total_files = len(files)

    # 使用多线程计算哈希值或获取时间/大小/名称
    with concurrent.futures.ThreadPoolExecutor() as executor:

        futures = {}
        for f in files:
            file_path = os.path.join(directory, f)
            if sort_by == 'hash':
                future = executor.submit(calculate_sha256, file_path)
            elif sort_by == 'ctime':
                future = executor.submit(get_creation_time, file_path)
            elif sort_by == 'mtime':
                future = executor.submit(get_last_modified_time, file_path)
            elif sort_by == 'size':
                future = executor.submit(get_file_size, file_path)
            elif sort_by == 'name':
                future = (os.path.splitext(f)[0], f)
            else:
                raise ValueError("排序规则无效，请使用“hash”、“ctime”、“mtime”、“size”或“name”。")

            futures[future] = f

    # 等待所有任务完成
    files_with_sorting = [(future.result(), futures[future]) for future in concurrent.futures.as_completed(futures)]

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


# 使用函数
if __name__ == "__main__":
    directory_path = r""  # 替换为你的文件夹路径
    sort_by = 'size'  # 可选 'hash', 'ctime', 'mtime', 'size', 'name'
    reverse_order = False  # 设置为True表示从大到小排序，False表示从小到大排序
    # 先执行预处理步骤
    preprocess_filenames(directory_path)
    # 再执行重命名步骤
    rename_files(directory_path, sort_by, reverse_order)
