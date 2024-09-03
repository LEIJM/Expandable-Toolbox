# Change audio format to mp3
# 将音频文件转换为MP3格式
import tkinter as tk
from tkinter import filedialog, messagebox
import os
import ffmpeg

def convert_audio(input_audio_path):
    try:
        # 获取输入音频文件的目录和文件名
        input_audio_dir, input_audio_filename = os.path.split(input_audio_path)
        # 创建输出音频文件的文件名
        output_audio_filename = input_audio_filename.rsplit('.', 1)[0] + '_out.mp3'
        # 拼接输出音频文件的完整路径
        output_audio_path = os.path.join(input_audio_dir, output_audio_filename)

        # 使用ffmpeg将音频转换为指定的格式
        (
            ffmpeg
            .input(input_audio_path)
            .output(output_audio_path, acodec='libmp3lame', abitrate='128k')
            .run(overwrite_output=True)
        )

        # 更新状态信息
        status_label.config(text="音频格式转换完成！")
    except Exception as e:
        # 更新状态信息为错误信息
        status_label.config(text=f"发生错误: {e}")

def browse_file():
    # 打开文件选择对话框
    filepath = filedialog.askopenfilename()
    if filepath:
        # 清空文本框并插入新路径
        entry.delete(0, tk.END)
        entry.insert(0, filepath)

def process_audio():
    # 获取文本框中的路径
    input_audio_path = entry.get()
    if not input_audio_path:
        messagebox.showwarning("警告", "请输入音频文件路径或选择文件。")
        return
    # 调用转换函数
    convert_audio(input_audio_path)

# 创建主窗口
root = tk.Tk()
root.title("音频格式转换器")

# 创建文本框用于输入文件路径
entry = tk.Entry(root, width=50)
entry.pack(pady=10)

# 创建浏览按钮
browse_button = tk.Button(root, text="浏览", command=browse_file)
browse_button.pack(side=tk.LEFT, padx=10, pady=10)

# 创建处理按钮
process_button = tk.Button(root, text="处理", command=process_audio)
process_button.pack(side=tk.RIGHT, padx=10, pady=10)

# 创建状态标签
status_label = tk.Label(root, text="", fg="red")
status_label.pack(side=tk.BOTTOM, fill=tk.X, padx=10, pady=10)

# 运行主循环
root.mainloop()


