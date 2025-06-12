import os
import subprocess
import chardet

def detect_encoding(file_path):
    with open(file_path, 'rb') as f:
        raw_data = f.read()
    result = chardet.detect(raw_data)
    return result['encoding'] if result['encoding'] else 'utf-8'

def preprocess_text_with_pos(text):
    processed_chars = []
    original_positions = []
    for idx, ch in enumerate(text):
        if ch not in [' ', '\t', '\n', '\r']:
            processed_chars.append(ch)
            original_positions.append(idx)
    return ''.join(processed_chars), original_positions

def get_line_number(text, char_pos):
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
        return True, "O 兩個檔案內容相同（忽略空白、tab與換行）"

    min_len = min(len(processed1), len(processed2))
    diff_index = next((i for i in range(min_len) if processed1[i] != processed2[i]), min_len)
    line_num1 = get_line_number(text1, pos_map1[diff_index]) if diff_index < len(pos_map1) else -1

    context_range = 30
    start = max(diff_index - context_range, 0)
    end = diff_index + context_range + 1

    snippet1 = processed1[start:end]
    snippet2 = processed2[start:end]

    detail = (
        f"! 兩個檔案內容不同（忽略空白、tab與換行）\n"
        f"差異字元所在行號: {line_num1}\n"
        f"檔案1差異片段:\n{snippet1}\n{' ' * (diff_index - start)}^\n"
        f"檔案2差異片段:\n{snippet2}\n{' ' * (diff_index - start)}^\n"
    )
    return False, detail

def run_and_compare(root_dir, log_lines):
    for subdir in sorted(os.listdir(root_dir)):
        taskset_path = os.path.join(root_dir, subdir)
        if not os.path.isdir(taskset_path):
            continue

        exe_name = f"OS_{root_dir}.exe"
        exe_path = os.path.join(taskset_path, exe_name)
        output_file = os.path.join(taskset_path, "Output.txt")
        output_ta_file = os.path.join(taskset_path, "Output_TA.txt")

        header = f"[{root_dir}/{subdir}]"
        print(f"➡️ 正在執行: {header}")
        log_lines.append(f"{header}\n")

        if not os.path.exists(exe_path):
            msg = f"⚠️ 找不到執行檔：{exe_path}\n"
            print(msg.strip())
            log_lines.append(msg)
            continue

        try:
            result = subprocess.run([exe_path], cwd=taskset_path,
                                    stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
            if result.returncode != 0:
                msg = f"⚠️ 執行 {exe_path} 回傳碼：{result.returncode}（可接受）\n"
                print(msg.strip())
                log_lines.append(msg)
        except Exception as e:
            msg = f"❌ 執行錯誤: {e}\n"
            print(msg.strip())
            log_lines.append(msg)
            continue

        if not os.path.exists(output_file) or not os.path.exists(output_ta_file):
            msg = "⚠️ 缺少 Output.txt 或 Output_TA.txt，略過比對\n"
            print(msg.strip())
            log_lines.append(msg)
            continue

        success, result = compare_files(output_file, output_ta_file)
        print("✅ 比對結果：" + ("相同" if success else "不同"))
        log_lines.append(result + "\n")

def main():
    log_lines = []
    for folder in ["NPCS", "CPP","CUS","EDF"]:
        if os.path.isdir(folder):
            print(f"\n📁 處理 {folder} 資料夾")
            run_and_compare(folder, log_lines)

    with open("check.txt", "w", encoding="utf-8") as fout:
        fout.writelines(log_lines)

if __name__ == "__main__":
    main()
