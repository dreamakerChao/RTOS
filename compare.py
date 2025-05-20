import chardet

def detect_encoding(file_path):
    """
    使用 chardet 偵測檔案編碼
    """
    with open(file_path, 'rb') as f:
        raw_data = f.read()
    result = chardet.detect(raw_data)
    return result['encoding'] if result['encoding'] else 'utf-8'

def preprocess_text_with_pos(text):
    """
    Remove spaces, tabs, and newlines, and return:
    - processed string
    - list of original indices for each character
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
    Return the line number (1-based) where the character is located in the original text
    """
    return text.count('\n', 0, char_pos) + 1

def compare_files(file1_path, file2_path):
    enc1 = detect_encoding(file1_path)
    enc2 = detect_encoding(file2_path)

    with open(file1_path, 'r', encoding=enc1, errors='replace') as f1, \
         open(file2_path, 'r', encoding=enc2, errors='replace') as f2:
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

    line_num1 = get_line_number(text1, pos_map1[diff_index]) if diff_index < len(pos_map1) else -1

    print(f"差異字元所在行號: {line_num1}")

    context_range = 30 # 顯示前後30個字元
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
