# Change audio format to mp3
# 将音频文件转换为MP3格式
import os
import ffmpeg

def convert_audio(input_audio_path):
    # 确保输入的音频文件路径不为空
    if not input_audio_path:
        raise ValueError("输入的音频文件路径不能为空。")

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
        .output(output_audio_path, acodec='libmp3lame', abitrate='128k')  # 使用libmp3lame编解码器，并设置比特率为128kbps
        .run()
    )

    print("音频格式转换完成！")

if __name__ == '__main__':
    # 输入音频文件路径
    input_audio = input("请输入音频文件的路径：")
    convert_audio(input_audio)

