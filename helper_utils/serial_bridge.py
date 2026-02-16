#!/usr/bin/env python3
"""
Serial-to-HTTP Bridge for GPIO Digital Twin
-------------------------------------------
Reads JSON telemetry from Serial (AVR) and forwards it to the 
HTTP GPIO Simulator (Unity Digital Twin).

Requirements:
    pip install pyserial
"""

import serial
import json
import requests
import sys
import threading
from datetime import datetime

# Configuration
SERIAL_PORT = 'COM3'
BAUD_RATE = 57600
SIMULATOR_URL = "http://127.0.0.1:8080/api/gpio"

def main():
    print("=" * 60)
    print("Serial-to-Digital Twin Bridge")
    print("=" * 60)
    print(f"Serial Port: {SERIAL_PORT}")
    print(f"Baud Rate:   {BAUD_RATE}")
    print(f"Simulator:   {SIMULATOR_URL}")
    print("=" * 60)

    try:
        # Initialize Serial
        ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=0.1)
        print(f"\n>>> Linked to {SERIAL_PORT} successfully! <<<")
        print("Waiting for telemetry events...\n")
        
        # Use a Session for connection pooling (much faster!)
        session = requests.Session()
        
        while True:
            if ser.in_waiting > 0:
                line = ser.readline().decode('utf-8', errors='ignore').strip()
                if not line:
                    continue
                
                try:
                    # Example format: {"t":"WRITE","p":"LED1","v":1}
                    data = json.loads(line)
                    
                    # Extract fields
                    cmd_type = data.get("t")
                    pin_name = data.get("p")
                    value = data.get("v")
                    
                    if cmd_type in ["WRITE", "GPIO"] and pin_name:
                        timestamp = datetime.now().strftime('%H:%M:%S')
                        
                        # Forward to Simulator ASYNC using threading
                        def send_to_simulator(p_name, p_val, ts):
                            url = f"{SIMULATOR_URL}/{p_name}"
                            payload = {"value": p_val}
                            try:
                                # Increased timeout to 1.0s to avoid flakes under heavy polling load
                                resp = session.post(url, json=payload, timeout=1.0)
                                if resp.status_code == 200:
                                    print(f"[{ts}] Forwarded: {p_name} -> {p_val}")
                                else:
                                    print(f"[{ts}] [ERROR] Simulator returned {resp.status_code}")
                            except requests.exceptions.RequestException as e:
                                print(f"[{ts}] [ERROR] Failed to reach Simulator: {e}")

                        # Fire and forget thread
                        threading.Thread(target=send_to_simulator, args=(pin_name, value, timestamp), daemon=True).start()
                            
                except json.JSONDecodeError:
                    # Ignore non-JSON lines (boot messages, logs, etc)
                    if not line.startswith('{'):
                         print(f"DEBUG: {line}")
                    continue
                    
    except serial.SerialException as e:
        print(f"\n[FATAL ERROR] Could not open {SERIAL_PORT}: {e}")
        print("\nPossible solutions:")
        print(f"1. Check if the AVR is plugged in.")
        print(f"2. Ensure no other apps (PuTTY, Arduino IDE) are using {SERIAL_PORT}.")
        print(f"3. Change the COM port in this script if needed.")
        sys.exit(1)
    except KeyboardInterrupt:
        print("\nBridge stopped by user.")
    finally:
        if 'ser' in locals() and ser.is_open:
            ser.close()

if __name__ == "__main__":
    main()
