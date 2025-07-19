import gspread
from google.oauth2.service_account import Credentials
import serial
import time
import json

# === Config ===
BUFFER = []
LAST_PUSH_TIME = time.time()
PUSH_INTERVAL_SECONDS = 60
MAX_BUFFER = 20
SERIAL_PORT = "COM10"
BAUD_RATE = 9600
SPREADSHEET_ID = "1UYoE7HpYE_xK2-5zKpek_TazQJzAFXdvs_ojRwo6ctg"

# === Initialize Serial ===
ser = serial.Serial(SERIAL_PORT, BAUD_RATE)
time.sleep(2)

print("Listening to LoRa data...")

# === Push data to Google Sheets ===
def push_to_sheet(data_batch):
    scopes = ["https://www.googleapis.com/auth/spreadsheets"]
    creds = Credentials.from_service_account_file("credentials.json", scopes=scopes)
    client = gspread.authorize(creds)
    sheet = client.open_by_key(SPREADSHEET_ID).sheet1

    # Add header row if empty
    if sheet.row_count == 0 or not sheet.row_values(1):
        sheet.append_row(["Timestamp", "Temperature", "pH", "TDS", "ORP"])

    sheet.append_rows(data_batch)
    print(f"Pushed {len(data_batch)} rows to Google Sheets.")

    maintain_sheet_limit(sheet, 120)

# === Delete old data from Google sheets ===
def maintain_sheet_limit(sheet, max_rows=5000, batch=100):
    rows = sheet.get_all_values()
    extra = len(rows) - max_rows
    to_delete = min(extra, batch)
    if extra > 0:
        for _ in range(to_delete):
            sheet.delete_rows(2)
        print(f"Deleted {to_delete} old rows from Sheets.")

# === Main Loop ===
while True:
    try:
        line = ser.readline().decode("utf-8").strip()
        if not line or not line.startswith("{"):
            continue

        data = json.loads(line)
        message_str = data.get("message", "")
        parts = message_str.split("|")

        result = {}
        for part in parts:
            if ':' in part:
                key, value = part.split(':', 1)
                result[key.strip().lower()] = value.strip()

        timestamp = time.strftime("%Y-%m-%d %H:%M:%S")
        print(f"[{timestamp}] Received:", data)

        # Append parsed row to buffer
        BUFFER.append([
            timestamp,
            result.get("temp", ""),
            result.get("ph", ""),
            result.get("tds", ""),
            result.get("orp", "")
        ])

        # Push to sheet if needed
        if len(BUFFER) >= MAX_BUFFER or (time.time() - LAST_PUSH_TIME) > PUSH_INTERVAL_SECONDS:
            push_to_sheet(BUFFER)
            BUFFER.clear()
            LAST_PUSH_TIME = time.time()

    except json.JSONDecodeError:
        print("Malformed JSON, skipping:", line)
    except Exception as e:
        print("Error:", e)
    except KeyboardInterrupt:
        print("Script Stopped by user, Exiting...")
        ser.close()
