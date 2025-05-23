from keras.models import load_model  # TensorFlow is required for Keras to work
import cv2  # Install opencv-python
import numpy as np
import socket

HOST = '192.168.160.210'
PORT = 8080


class Messenger:
    def __init__(self, host: str, port: int) -> None:
        self.host = socket.socket()
        self.host.connect((host, port))

    def send_message(self, message: str) -> None:
        self.host.send(f"{message}\n".encode())

    def close(self) -> None:
        self.host.close()


messenger = Messenger(HOST, PORT)

# Disable scientific notation for clarity
np.set_printoptions(suppress=True)

# Load the model
model = load_model("keras_Model.h5", compile=False)

# Load the labels
class_names = open("labels.txt", "r").readlines()

# CAMERA can be 0 or 1 based on default camera of your computer
camera = cv2.VideoCapture('http://'+HOST+':81/stream')

while True:
    # Grab the web camera's image.
    ret, image = camera.read()

    # Resize the raw image into (224-height,224-width) pixels
    image = cv2.resize(image, (224, 224), interpolation=cv2.INTER_AREA)

    # Show the image in a window
    cv2.imshow("Webcam Image", image)

    # Make the image a numpy array and reshape it to the models input shape.
    image = np.asarray(image, dtype=np.float32).reshape(1, 224, 224, 3)

    # Normalize the image array
    image = (image / 127.5) - 1

    # Predicts the model
    prediction = model.predict(image)
    index = np.argmax(prediction)
    class_name = class_names[index]
    confidence_score = prediction[0][index]

    # Print prediction and confidence score
    print("Class:", class_name[2:], end="")
    print("Confidence Score:", str(np.round(confidence_score * 100))[:-2], "%")

    # Define the acceptable signs
    signs = ['10', '0', '-10', '15', '-15', 'STOP']

    # Send the sign as a string to the ESP32 camera
    if confidence_score > 0.9 and class_name[2:-1] in signs:
        messenger.send_message(class_name[2:-1])

    # Listen to the keyboard for presses.
    keyboard_input = cv2.waitKey(1)

    # 27 is the ASCII for the esc key on your keyboard.
    if keyboard_input == 27:
        break

camera.release()
cv2.destroyAllWindows()
