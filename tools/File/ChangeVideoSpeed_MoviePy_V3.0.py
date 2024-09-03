# Change video speed
# 更改视频速率
import tkinter as tk
from tkinter import filedialog
from moviepy.editor import VideoFileClip
import os

def adjust_video_speed_and_save(video_path, speed):
    if not video_path:
        raise ValueError("输入的视频文件路径不能为空。")

    video_dir, video_filename = os.path.split(video_path)
    output_filename = video_filename.rsplit('.', 1)[0] + '_out.mp4'
    output_path = os.path.join(video_dir, output_filename)

    clip = VideoFileClip(video_path)
    adjusted_clip = clip.speedx(speed)
    adjusted_clip.write_videofile(output_path)
    return f"视频播放速率调整完成，输出文件位于：{output_path}"

def browse_file():
    file_path = filedialog.askopenfilename()
    if file_path:
        file_entry.delete(0, tk.END)
        file_entry.insert(0, file_path)

def process_video():
    video_path = file_entry.get()
    speed = float(speed_entry.get())
    result = adjust_video_speed_and_save(video_path, speed)
    result_label.config(text=result)

# 创建主窗口
root = tk.Tk()
root.title("视频播放速率调整")

# 创建文件路径输入框和选择文件按钮
file_frame = tk.Frame(root)
file_entry = tk.Entry(file_frame, width=50)
file_entry.pack(side=tk.LEFT, padx=5)
browse_button = tk.Button(file_frame, text="选择文件", command=browse_file)
browse_button.pack(side=tk.LEFT, padx=5)
file_frame.pack(pady=10)

# 创建播放速率标签和输入框
speed_frame = tk.Frame(root)
speed_label = tk.Label(speed_frame, text="播放速率:")
speed_label.pack(side=tk.LEFT)
speed_entry = tk.Entry(speed_frame, width=10)
speed_entry.pack(side=tk.LEFT, padx=5)
speed_frame.pack(pady=10)

# 创建处理按钮
process_button = tk.Button(root, text="处理", command=process_video)
process_button.pack(pady=10)

# 创建结果标签
result_label = tk.Label(root, text="", fg="green")
result_label.pack(pady=10)

# 启动主循环
root.mainloop()
