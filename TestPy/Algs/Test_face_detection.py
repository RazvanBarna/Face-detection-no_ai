import kagglehub
from pathlib import Path
import subprocess

path = kagglehub.dataset_download("iamtushara/face-detection-dataset")
dataset_path = Path(path) / "merged"

images_train = dataset_path / "images" / "train"
images_validation = dataset_path / "images" / "validation"
labels_train = dataset_path / "labels" / "train"

exe_path = "../exec/OpenCVApplication.exe"

i = 0
for img in images_validation.iterdir():
    if i == 100:
        break
    if img.suffix.lower() in {".jpg", ".jpeg", ".png", ".bmp"}:
        i = i+1
        print(img.name)
        subprocess.run([exe_path,str(img)])