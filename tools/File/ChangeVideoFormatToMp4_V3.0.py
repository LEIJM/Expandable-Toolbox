# Change video format to mp4
# 将视频格式更改为mp4
import os
from moviepy.editor import VideoFileClip
import tkinter as tk
from tkinter import filedialog, messagebox


def convert_video(input_video_path):
    # 确保输入的视频文件路径不为空
    if not input_video_path:
        raise ValueError("输入的视频文件路径不能为空。")

    # 获取输入视频文件的目录和文件名
    input_video_dir, input_video_filename = os.path.split(input_video_path)
    # 创建输出视频文件的文件名，保留原文件后缀
    output_video_filename = input_video_filename.rsplit('.', 1)[0] + '_out.mp4'
    # 拼接输出视频文件的完整路径
    output_video_path = os.path.join(input_video_dir, output_video_filename)

    # 加载视频
    video = VideoFileClip(input_video_path)
    # 写入新格式的视频文件
    # 注意：这里不需要指定编解码器，因为moviepy会根据文件扩展名自动选择
    video.write_videofile(output_video_path)
    # 释放资源
    video.reader.close()
    video.audio.reader.close_proc()

    # 更新状态信息
    status_label.config(text="视频格式转换完成！")


def browse_file():
    # 打开文件选择对话框
    filepath = filedialog.askopenfilename()
    if filepath:
        # 清空文本框并插入新路径
        file_entry.delete(0, tk.END)
        file_entry.insert(0, filepath)


def process_video():
    input_video_path = file_entry.get()
    if not input_video_path:
        messagebox.showwarning("警告", "请输入视频文件路径或选择文件。")
        return
    convert_video(input_video_path)


# 创建主窗口
root = tk.Tk()
root.title("视频格式转换器 其他->MP4")

# 创建文件路径输入框
file_entry = tk.Entry(root, width=50)
file_entry.pack(pady=10)

# 创建浏览按钮
browse_button = tk.Button(root, text="浏览", command=browse_file)
browse_button.pack(side=tk.LEFT, padx=10, pady=10)

# 创建处理按钮
process_button = tk.Button(root, text="处理", command=process_video)
process_button.pack(side=tk.RIGHT, padx=10, pady=10)

# 创建状态标签
status_label = tk.Label(root, text="", fg="green")
status_label.pack(side=tk.BOTTOM, fill=tk.X, padx=10, pady=10)

# 运行主循环
root.mainloop()
