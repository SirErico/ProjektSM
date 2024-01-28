import cv2
import serial
import json
import keyboard
import time

hSerial = serial.Serial('COM3', 115200, timeout=1, parity=serial.PARITY_NONE)
rpm = 0  # Zmienna do przechowywania wartości suwaka

timestr = time.strftime("%Y%m%d-%H%M%S")
hFile = open(f"data_two_positioncontroller{timestr}.txt", "a")

def on_trackbar_change(value):
    global rpm
    rpm = value
    send_rpm = rpm + 1000  # Wartość, którą będziemy wysyłać przez port szeregowy
    send_data = f"{send_rpm:04}"  # Formatujemy wartość do 4 znaków
    hSerial.write(send_data.encode('utf-8'))  # Wysyłamy zaktualizowaną wartość przez port szeregowy
    print(f"Sent RPM value: {send_rpm}")

def main():
    cv2.namedWindow('Manage RPM')
    cv2.createTrackbar('RPM 1000+', 'Manage RPM', rpm, 2000, on_trackbar_change)

    while True:
        if keyboard.is_pressed("q"):
            break

        time.sleep(0.1)  # Dodajemy opóźnienie przed odczytem danych z portu szeregowego

        received_data = hSerial.readline().decode("utf-8").strip()
       # print(f"Received data: {received_data}")

        try:
            sample = json.loads(received_data)
        except ValueError:
            print("Bad JSON")
            print(f"{received_data}\r")
            continue

        received_rpm = sample.get("RPM", 0)
        received_rpm_ref = sample.get("RPM_REF", 0)
        received_duty = sample.get("Duty", 0)

        hFile.write(f"{received_data}\n")

        cv2.waitKey(10)

    cv2.destroyAllWindows()
    hFile.close()

main()
hSerial.close()