import os
import shutil
import hashlib
import time
from concurrent.futures import ProcessPoolExecutor


def calculate_sha256(file_path):
    """计算文件的SHA-256哈希值"""
    hash_sha256 = hashlib.sha256()
    with open(file_path, "rb") as f:
        for chunk in iter(lambda: f.read(4096), b""):
            hash_sha256.update(chunk)
    return hash_sha256.hexdigest()


def is_older(file1, file2):
    """判断file1是否比file2旧"""
    return os.path.getmtime(file1) < os.path.getmtime(file2)


def process_directory(directory):
    """处理指定目录，找出重复文件并移动较新的文件到'重复文件'文件夹"""
    file_hashes = {}
    duplicates = []
  
    # 遍历目录中的文件  
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
                    duplicate_dir = os.path.join(directory, '重复文件')
                    os.makedirs(duplicate_dir, exist_ok=True)
                    duplicate_file_name = os.path.join(duplicate_dir, file)
                    shutil.move(file_path, duplicate_file_name)
                    print(f"已移动重复文件: {file_path} -> {duplicate_file_name}")
            else:
                # 记录文件哈希值
                file_hashes[file_hash] = file_path
  
    # 如果没有重复文件，打印信息
    if not duplicates:
        print("无重复文件")
    else:
        print("已处理完成")


def main(directory):
    # 使用多进程执行目录处理
    with ProcessPoolExecutor() as executor:
        executor.submit(process_directory, directory)


if __name__ == "__main__":
    # 指定要检查的目录
    target_directory = r""
    start_time = time.time()  
    main(target_directory)
    end_time = time.time()
    print(f"共花费 {end_time - start_time:.2f} 秒。")
