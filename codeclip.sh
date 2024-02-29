#!/bin/bash

# 通过脚本参数传递源文件夹和输出文件夹的名字
source_folder="$1"
output_folder="$2"

# 检查参数是否提供
if [ -z "${source_folder}" ] || [ -z "${output_folder}" ]; then
    echo "请提供源文件夹和输出文件夹的名字作为参数"
    exit 1
fi

# Clang 工具路径和参数
clang_tool="./my_clang_tool"

# 获取当前工作目录的绝对路径
current_path=$(pwd)


# 确保输出文件夹存在，如果不存在则创建
mkdir -p "${output_folder}"

# 遍历源文件夹中的所有文件
find "${source_folder}" -type f -name "*.cpp" | while read -r file; do
    if [ -f "${file}" ]; then
        # 提取文件名和扩展名
        filename=$(basename "${file}")
        extension="${filename##*.}"

        # 检查文件扩展名是否为.cpp
        if [ "${extension}" = "cpp" ]; then
            # 获取文件相对于源文件夹的相对路径
            relative_path="${file#${source_folder}/}"

            # 构建输出文件路径
            relative_output_file="${output_folder}/${relative_path}"

            # 获取绝对路径
            absolute_path="${current_path}/${relative_output_file}"

            clang_tool_args="--out-put=${absolute_path}"

            echo "Processed ${file} and saved to ${clang_tool_args}"

            # 运行 Clang 工具
            ${clang_tool} "${file}" ${clang_tool_args}
           
        else
            echo "Skipping non-C++ file: ${file}"
        fi
    fi
done