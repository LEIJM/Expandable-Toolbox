# Delete Empty Folder
# 删除空文件夹
import os

def is_empty_dir(path):
    """检查目录是否为空"""
    try:
        if not os.path.isdir(path):
            return False
        return not os.listdir(path)
    except OSError:
        return False

def remove_empty_dirs(path):
    """
    递归删除指定路径下的所有空文件夹
    返回True表示删除了空文件夹，返回False表示没有删除任何空文件夹
    """
    if is_empty_dir(path):
        try:
            os.rmdir(path)
            print(f"已删除空目录: {path}")
            return True
        except OSError as e:
            print(f"删除时出错的目录: {path}: {e}")
            return False
    else:
        # 递归检查子目录
        for item in os.listdir(path):
            item_path = os.path.join(path, item)
            if os.path.isdir(item_path):
                if remove_empty_dirs(item_path):
                    return True
    return False
def remove_empty_dirs_in_directory(directory):
    """在指定目录下使用单线程删除所有空文件夹"""
    # 获取目录下的所有子目录
    subdirectories = [os.path.join(directory, item) for item in os.listdir(directory) if os.path.isdir(os.path.join(directory, item))]

    # 使用单线程处理子目录
    any_empty_dir_removed = False
    for subdir in subdirectories:
        if remove_empty_dirs(subdir):
            any_empty_dir_removed = True

    if not any_empty_dir_removed:
        print("无空文件夹")

if __name__ == "__main__":
    # 从控制台获取要处理的目录
    target_directory = input("请输入要处理的目录路径: \n")
    # 删除空文件夹
    remove_empty_dirs_in_directory(target_directory)
    print("处理完成")
