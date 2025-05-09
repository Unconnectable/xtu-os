#!/bin/bash

# 替换映射：中文全角 → 英文半角
declare -A replace_map=(
    ['（']='('
    ['）']=')'
    ['，']=','
    ['。']='.'
    ['；']=';'
    ['：']=':'
)

# 初始化统计信息（基于 replace_map 动态生成）
declare -A count_stats
for key in "${!replace_map[@]}"; do
    value="${replace_map[$key]}"
    count_stats["$key→$value"]=0
done

# 支持的文本文件扩展名列表
textFileExtensions=(
    .txt .md
    .ps1 .sh .py .pl .rb
    .js .ts .json .yml .yaml
    .html .htm .css .php
    .java .c .h .cpp .hpp
    .cs .go .rs .swift .kt
)

# 构建 find 匹配表达式：匹配指定扩展名的文件
find_expr="\\( "
first=1
for ext in "${textFileExtensions[@]}"; do
    if [[ $first -eq 1 ]]; then
        find_expr+=" -name \"*$ext\""
        first=0
    else
        find_expr+=" -o -name \"*$ext\""
    fi
done
find_expr+=" \\)"

# 获取当前脚本的绝对路径
current_script_path="$(cd "$(dirname "$0")" && pwd)/$(basename "$0")"

# 查找所有符合扩展名的文件
while IFS= read -r -d '' file; do
    # 获取文件的绝对路径
    absolute_file="$(cd "$(dirname "$file")" && pwd)/$(basename "$file")"

    # 如果是当前脚本本身，则跳过
    if [[ "$absolute_file" == "$current_script_path" ]]; then
        echo "跳过自身脚本: $file"
        continue
    fi

    echo "处理文件: $file"

    temp_file=$(mktemp)
    cp "$file" "$temp_file"

    # 逐个替换字符并统计次数
    for key in "${!replace_map[@]}"; do
        value="${replace_map[$key]}"
        stat_key="$key→$value"

        old_count=$(grep -o "$key" "$temp_file" | wc -l 2>/dev/null)
        sed -i "s/$key/$value/g" "$temp_file"
        new_count=$(grep -o "$key" "$temp_file" | wc -l 2>/dev/null)
        replaced_count=$((old_count - new_count))
        count_stats["$stat_key"]=$((count_stats["$stat_key"] + replaced_count))
    done

    # 替换原文件
    mv "$temp_file" "$file"
done < <(eval find . -type f $find_expr -print0)

# 输出统计信息
echo -e "\n替换统计:"
for key in "${!count_stats[@]}"; do
    echo "$key: ${count_stats[$key]} 次"
done

echo -e "\n所有文件处理完成!"