# Change video speed
# 更改视频速率
from moviepy.editor import VideoFileClip
import os

def adjust_video_speed_and_save(video_path, speed):
    # 确保输入的视频文件路径不为空
    if not video_path:
        raise ValueError("输入的视频文件路径不能为空。")

    # 获取输入视频文件的目录和文件名
    video_dir, video_filename = os.path.split(video_path)
    # 创建输出视频文件的文件名
    output_filename = video_filename.rsplit('.', 1)[0] + '_out.mp4'
    # 拼接输出视频文件的完整路径
    output_path = os.path.join(video_dir, output_filename)

    # 加载视频
    clip = VideoFileClip(video_path)

    # 调整播放速率
    # 注意：speed < 1 会减慢速度，speed > 1 会加快速度
    adjusted_clip = clip.speedx(speed)

    # 写入新文件
    adjusted_clip.write_videofile(output_path)

if __name__ == '__main__':
    # 视频文件的路径
    video_path = r"E:\茶啊二中1-1.ts"
    # 播放速率（例如 1.5 或 0.5）
    speed = 1.5
    adjust_video_speed_and_save(video_path, speed)
    print("视频播放速率调整完成！")
