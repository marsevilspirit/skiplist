#!/bin/bash

# 格式化所有C++源文件的脚本

echo "正在格式化C++源文件..."

# 格式化源文件
find src/ -name "*.hpp" -o -name "*.cpp" | xargs clang-format -i
find examples/ -name "*.cpp" | xargs clang-format -i
find tests/ -name "*.cpp" | xargs clang-format -i

echo "格式化完成！"

# 显示修改的文件
echo "已修改的文件："
git diff --name-only
