import os
from ffmpeg import input, output


def convert_video_ffmpeg(input_video_path):
    # 确保输入的视频文件路径不为空
    if not input_video_path:
        raise ValueError("输入的视频文件路径不能为空。")

    # 获取输入视频文件的目录和文件名
    input_video_dir, input_video_filename = os.path.split(input_video_path)
    # 创建输出视频文件的文件名，保留原文件后缀
    output_video_filename = input_video_filename.rsplit('.', 1)[0] + '_out.mp4'
    # 拼接输出视频文件的完整路径
    output_video_path = os.path.join(input_video_dir, output_video_filename)

    # 使用ffmpeg-python库转换视频格式
    try:
        (
            input(input_video_path)
            .output(output_video_path)
            .run(overwrite_output=True)
        )
        print(f"视频已成功转换为MP4格式，并保存为 {output_video_path}")
    except Exception as e:
        print(f"视频转换失败: {e}")


if __name__ == '__main__':
    # 输入视频文件路径
    input_video = r"E:\test\1.flv"
    convert_video_ffmpeg(input_video)
