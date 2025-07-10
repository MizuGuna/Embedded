from logging import exception

import gspread
from google.oauth2.service_account import Credentials
import serial
import time
import json

scopes = [
    "https://www.googleapis.com/auth/spreadsheets"
]
creds = Credentials.from_service_account_file("credentials.json", scopes=scopes)
client = gspread.authorize(creds)
sheet_id = "1UYoE7HpYE_xK2-5zKpek_TazQJzAFXdvs_ojRwo6ctg"
sheet = client.open_by_key(sheet_id).sheet1

if sheet.row_count == 0 or not sheet.row_values(1):
    sheet.append_row(["Timestamp", "Temperature", "pH", "TDS", "ORP"])

ser = serial.Serial("COM6", 9600)
time.sleep(2)

print("Listening to LoRa data...")


while True:
    try:
        line = ser.readline().decode("utf-8").strip()
        if not line or not line.startswith("{"):
            continue

        data = json.loads(line)

        message_str = data["message"]

        parts = message_str.split("|")

        result = {}

        for part in parts:
            if ':' in part:
                key, value = part.split(':', 1)
                key = key.strip().lower()
                value = value.strip()
                result[key] = value

        timestamp = time.strftime("%Y-%m-%d %H:%M:%S")
        print(f"[{timestamp}] Received: ", data)

        sheet.append_row([
            timestamp,
            result.get("temp", ""),
            result.get("ph", ""),
            result.get("tds" ""),
            result.get("orp", "")
        ])

    except json.JSONDecodeError:
        print("Malformed JSON, skipping:", line)
    except Exception as e:
        print("Error: ", e)