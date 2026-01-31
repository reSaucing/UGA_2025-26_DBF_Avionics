import serial
import time

# --- CONFIGURATION ---
SERIAL_PORT = 'COM3'   # Change this to your port
BAUD_RATE = 9600 
OUTPUT_FILE = 'sensorData.txt'
TIMEOUT_SECONDS = 3    # How long to wait for data before quitting
# ---------------------

def read_until_finished():
    try:
        # Initialize serial with a specific timeout
        ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=TIMEOUT_SECONDS)

        # check if connection was opened successfully
        if not ser.is_open:
            print(f"Failed to open {SERIAL_PORT}. Exiting.")
            return 
        
        # Wait for Arduino to reset/boot up after port opens
        print("Connecting... waiting for device boot.")
        time.sleep(2) 
        
        print(f"Listening. Will stop if no data received for {TIMEOUT_SECONDS} seconds.")

        # if file exists, it will be overwritten
        # using 'a' instead will append to file if it exists
        with open(OUTPUT_FILE, 'w') as file:
            while True:
                # readline() blocks for up to TIMEOUT_SECONDS.
                # If data comes, it reads it.
                # If time runs out, it returns an empty byte string b''.
                line_bytes = ser.readline()
                
                # Check if we received data or if we timed out
                if len(line_bytes) == 0:
                    print("\nTimeout reached: No data received. Stopping script.")
                    break # Exit the while loop
                
                try:
                    decoded_line = line_bytes.decode('utf-8').strip()
                    
                    # Only write if the line isn't empty whitespace
                    if decoded_line:
                        print(f"Received: {decoded_line}")
                        file.write(decoded_line + '\n')
                        file.flush()
                        
                except UnicodeDecodeError:
                    print("Decode Error (ignored)")

    except serial.SerialException as e:
        print(f"Error: {e}")
        
    finally:
        if 'ser' in locals() and ser.is_open:
            ser.close()
            print("Connection closed.")

if __name__ == "__main__":
    read_until_finished()