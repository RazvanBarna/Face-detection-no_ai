import cv2
from pathlib import Path
import kagglehub
import csv

def haar_data(i:int):
    path = kagglehub.dataset_download("iamtushara/face-detection-dataset")
    dataset_path = Path(path) / "merged"

    images_train = dataset_path / "images" / "train"
    images_validation = dataset_path / "images" / "validation"
    labels_train = dataset_path / "labels" / "train"

    cascade_path = cv2.data.haarcascades + "haarcascade_frontalface_default.xml"
    face_cascade = cv2.CascadeClassifier(cascade_path)
    fieldsName = ['filename', 'c_min', 'c_max', 'r_min', 'r_max']

    with open('../CSVs/haar_coords.csv', 'w', newline='') as csvfile:
        writer = csv.writer(csvfile)
        writer.writerow(fieldsName)

        idx = 0
        for img_path in images_validation.iterdir():
            if idx == i:
                break

            if img_path.suffix.lower() not in {".jpg", ".jpeg", ".png", ".bmp"}:
                continue

            src = cv2.imread(str(img_path))

            idx += 1
            print(img_path.name)

            gray = cv2.cvtColor(src, cv2.COLOR_BGR2GRAY)
            faces = face_cascade.detectMultiScale(
                gray,
                scaleFactor=1.1,
                minNeighbors=5,
                minSize=(30, 30)
            )

            for (x, y, w, h) in faces:
                cv2.rectangle(src, (x, y), (x + w, y + h), (0, 255, 0), 2)
                row = [img_path.name, x, x + w, y, y + h]
                writer.writerow(row)

            cv2.imwrite(f"../MyDataset/Haar_images/{img_path.name}", src)
