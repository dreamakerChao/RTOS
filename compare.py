def preprocess_text_with_pos(text):
    """
    移除空白、tab、換行，並回傳：
    - 處理後字串
    - 每個字元在原始字串的位置index清單
    """
    processed_chars = []
    original_positions = []
    for idx, ch in enumerate(text):
        if ch not in [' ', '\t', '\n', '\r']:
            processed_chars.append(ch)
            original_positions.append(idx)
    return ''.join(processed_chars), original_positions

def get_line_number(text, char_pos):
    """
    根據字元在原始字串位置，計算所在行數 (從1開始)
    """
    return text.count('\n', 0, char_pos) + 1

def compare_files(file1_path, file2_path):
    with open(file1_path, 'r', encoding='utf-8') as f1, \
         open(file2_path, 'r', encoding='utf-8') as f2:
        text1 = f1.read()
        text2 = f2.read()

    processed1, pos_map1 = preprocess_text_with_pos(text1)
    processed2, pos_map2 = preprocess_text_with_pos(text2)

    if processed1 == processed2:
        print("O 兩個檔案內容相同(忽略空白、tab與換行)")
        return

    print("! 兩個檔案內容不同(忽略空白、tab與換行)")

    min_len = min(len(processed1), len(processed2))
    diff_index = None
    for i in range(min_len):
        if processed1[i] != processed2[i]:
            diff_index = i
            break
    if diff_index is None:
        diff_index = min_len

    # 印出差異字元所在原始檔案的行號
    line_num1 = get_line_number(text1, pos_map1[diff_index]) if diff_index < len(pos_map1) else -1
    line_num2 = get_line_number(text2, pos_map2[diff_index]) if diff_index < len(pos_map2) else -1

    print(f"檔案1差異字元所在行號: {line_num1}")
    print(f"檔案2差異字元所在行號: {line_num2}")

    # 印出差異附近前後10個字，並用^標示字元差異位置
    context_range = 10
    start = max(diff_index - context_range, 0)
    end = diff_index + context_range + 1

    snippet1 = processed1[start:end]
    snippet2 = processed2[start:end]

    print("檔案1差異片段:")
    print(snippet1)
    print(" " * (diff_index - start) + "^")

    print("檔案2差異片段:")
    print(snippet2)
    print(" " * (diff_index - start) + "^")

# 範例使用
compare_files('Output.txt', 'Output_TA.txt')
