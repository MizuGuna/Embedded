"""
LoRa Data Logger to Google Sheets

This script receives JSON data from a LoRa receiver via serial port,
parses sensor data (temperature, pH, TDS, ORP), and logs it to Google Sheets
in batches for efficient API usage.

Data flow:
1. LoRa receiver (Arduino) sends JSON: {"packet_size": 32, "message": "Temp:25.5|pH:7.2|TDS:350|ORP:200", "rssi": -80, "snr": 8.5}
2. Python extracts the "message" field and parses the pipe-delimited sensor data (case-insensitive)
3. Parsed data is batched and sent to Google Sheets
"""

import gspread
from google.oauth2.service_account import Credentials
import serial
import time
import json

# === Configuration Parameters ===
BUFFER = []                           # In-memory buffer to store data before pushing to sheets
LAST_PUSH_TIME = time.time()          # Track last time data was pushed to sheets
PUSH_INTERVAL_SECONDS = 60            # Push to sheets every 60 seconds (even if buffer isn't full)
MAX_BUFFER = 20                       # Push to sheets when buffer reaches this size
SERIAL_PORT = "COM10"                 # Serial port where LoRa receiver is connected
BAUD_RATE = 9600                      # Serial communication baud rate (must match Arduino)
SPREADSHEET_ID = "1UYoE7HpYE_xK2-5zKpek_TazQJzAFXdvs_ojRwo6ctg"  # Google Sheets ID

# === Initialize Serial Communication ===
ser = serial.Serial(SERIAL_PORT, BAUD_RATE)
time.sleep(2)  # Allow serial connection to stabilize

print("Listening to LoRa data...")

# === Push data to Google Sheets ===
def push_to_sheet(data_batch):
    """
Push a batch of sensor data to Google Sheets

Args:
data_batch: List of lists containing [timestamp, temp, ph, tds, orp] data
    """
    # Set up Google Sheets API authentication
    scopes = ["https://www.googleapis.com/auth/spreadsheets"]
    creds = Credentials.from_service_account_file("credentials.json", scopes=scopes)
    client = gspread.authorize(creds)
    sheet = client.open_by_key(SPREADSHEET_ID).sheet1

    # Initialize sheet with headers if empty
    if sheet.row_count == 0 or not sheet.row_values(1):
        sheet.append_row(["Timestamp", "Temperature", "pH", "TDS", "ORP"])

    # Batch append all buffered data (more efficient than individual appends)
    sheet.append_rows(data_batch)
    print(f"Pushed {len(data_batch)} rows to Google Sheets.")

    # Clean up old data to prevent sheet from growing indefinitely
    maintain_sheet_limit(sheet, 120)

# === Delete old data from Google Sheets ===
def maintain_sheet_limit(sheet, max_rows=5000, batch=100):
    """
Keep sheet size manageable by deleting oldest rows when limit is exceeded

Args:
sheet: Google Sheets worksheet object
max_rows: Maximum number of rows to keep in sheet
batch: Number of rows to delete at once (for performance)
    """
    rows = sheet.get_all_values()
    extra = len(rows) - max_rows
    to_delete = min(extra, batch)

    if extra > 0:
        # Delete from row 2 (keep header row) - oldest data first
        for _ in range(to_delete):
            sheet.delete_rows(2)
        print(f"Deleted {to_delete} old rows from Sheets.")

# === Main Data Processing Loop ===
while True:
    try:
        # Read JSON data from serial port (sent by LoRa receiver)
        line = ser.readline().decode("utf-8").strip()

        # Skip empty lines or non-JSON data
        if not line or not line.startswith("{"):
            continue

        # Parse JSON data from LoRa receiver (contains packet_size, message, rssi, snr)
        data = json.loads(line)
        message_str = data.get("message", "")  # Extract the sensor data from "message" field

        # Parse sensor data from message string (flexible format: "Temp:25.5|pH:7.2|TDS:350|ORP:200")
        # Case-insensitive parsing - keys are normalized to lowercase
        parts = message_str.split("|")
        result = {}

        # Extract key-value pairs from each part (handles any case)
        for part in parts:
            if ':' in part:
                key, value = part.split(':', 1)
                result[key.strip().lower()] = value.strip()  # Convert keys to lowercase

        # Generate timestamp for this reading
        timestamp = time.strftime("%Y-%m-%d %H:%M:%S")
        print(f"[{timestamp}] Received:", data)

        # Add parsed data to buffer for batch processing
        BUFFER.append([
            timestamp,
            result.get("temp", ""),    # Temperature value or empty string
            result.get("ph", ""),      # pH value or empty string
            result.get("tds", ""),     # TDS (Total Dissolved Solids) or empty string
            result.get("orp", "")      # ORP (Oxidation Reduction Potential) or empty string
        ])

        # Push data to Google Sheets if buffer is full OR time interval exceeded
        # This balances between API efficiency and data freshness
        if len(BUFFER) >= MAX_BUFFER or (time.time() - LAST_PUSH_TIME) > PUSH_INTERVAL_SECONDS:
            push_to_sheet(BUFFER)
            BUFFER.clear()
            LAST_PUSH_TIME = time.time()

    except json.JSONDecodeError:
        # Handle malformed JSON from serial port gracefully
        print("Malformed JSON, skipping:", line)
    except Exception as e:
        # Log any other errors but continue running
        print("Error:", e)
    except KeyboardInterrupt:
        # Clean shutdown when user stops the script
        print("Script Stopped by user, Exiting...")
        ser.close()
        break