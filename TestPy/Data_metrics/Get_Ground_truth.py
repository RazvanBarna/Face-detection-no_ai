import kagglehub
from pathlib import Path
import csv
import cv2

path = kagglehub.dataset_download("iamtushara/face-detection-dataset")
dataset_path = Path(path) / "merged"

labels_validation = dataset_path / "labels" / "validation"
img_path = Path("../MyDataset/Images")

i = 0
fieldsName = ['filename', 'c_min', 'c_max', 'r_min', 'r_max']
with open('../CSVs/labels.csv', 'w', newline='') as csvfile:
    writer = csv.writer(csvfile)
    writer.writerow(fieldsName)
    for txt in labels_validation.iterdir():
        if i == 100:
            break
        img_name = Path(txt).stem

        img_file = img_path / (img_name + ".jpg")
        src = cv2.Mat
        if img_file.exists():
            src  = cv2.imread(str(img_file))
        else:
            print(f'{img_file} {i} does not exist!!')

        i+=1
        if txt.suffix.lower() != '.txt':
                continue

        with open(txt, 'r') as f:
            lines = f.readlines()
            for l in lines:
                line = l.split() #x y w h
                line.pop(0)
                x_center_norm = float(line[0])
                y_center_norm = float(line[1])
                w_norm = float(line[2])
                h_norm = float(line[3])
                h,w,c = src.shape
                x1 = int((x_center_norm * w) - (w_norm * w /2))
                x2 = int((x_center_norm * w) + (w_norm * w /2))
                y1 = int((y_center_norm * h) - (h_norm * h /2))
                y2 = int((y_center_norm * h) + (h_norm * h /2))
                writer.writerow([img_name, x1, x2, y1, y2])


