import kagglehub
from pathlib import Path
import subprocess

def get_data(i : int):
    path = kagglehub.dataset_download("iamtushara/face-detection-dataset")
    dataset_path = Path(path) / "merged"

    images_train = dataset_path / "images" / "train"
    images_validation = dataset_path / "images" / "validation"
    labels_train = dataset_path / "labels" / "train"

    idx = 0
    exe_path = "../exec/OpenCVApplication.exe"
    for img in images_validation.iterdir():
        if idx == i:
            break
        if img.suffix.lower() in {".jpg", ".jpeg", ".png", ".bmp"}:
            idx = idx+1
            print(img.name)
            subprocess.run([exe_path,str(img)])