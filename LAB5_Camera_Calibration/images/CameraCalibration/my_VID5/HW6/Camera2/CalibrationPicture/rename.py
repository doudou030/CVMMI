import os

# 設定資料夾路徑
folder_path = './'

# 確保資料夾存在
if not os.path.exists(folder_path):
    print("指定的資料夾不存在。")
    exit()

# 獲取資料夾中的所有檔案
file_list = os.listdir(folder_path)

# 過濾出圖片檔案
image_files = [f for f in file_list if f.endswith(('.jpg', '.jpeg', '.png', '.gif'))]

# 開始編號
index = 0

# 逐一重新命名圖片檔案
for filename in image_files:
    # 新的檔案名稱
    new_filename = 'image_{:02d}.jpg'.format(index)
    
    # 舊的檔案路徑
    old_filepath = os.path.join(folder_path, filename)
    
    # 新的檔案路徑
    new_filepath = os.path.join(folder_path, new_filename)
    
    # 重新命名檔案
    os.rename(old_filepath, new_filepath)
    
    # 更新編號
    index += 1

print("所有圖片重新命名完成。")
