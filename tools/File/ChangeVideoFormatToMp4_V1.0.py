# Change video format to mp4
# 将视频格式更改为mp4
import os
from moviepy.editor import VideoFileClip


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


if __name__ == '__main__':
    # 输入视频文件路径
    input_video = r"E:\test\1.flv"
    convert_video(input_video)
    print("视频格式转换完成！")

