import serial
import time
import sys

# INSTRUCTIONS:
# 1. Enter 'm' to enter management mode on the teensy
# 2. Enter 'l' to list the files
# 3. Enter 'e' to enter extraction mode
# 4. Enter the name of the file you want to extract
#
# ***If you do the commands in any other order it won't work***

# --- CONFIGURATION ---
SERIAL_PORT = 'COM3'   # Change this to your port
BAUD_RATE = 9600 
OUTPUT_FILE = 'sensorData.csv'
# Set a very short timeout so we can check for user input frequently
TIMEOUT_SECONDS = 0.1  
# ---------------------

def run_serial_session():
    # indicate whether lines are currently being read from file
    # this will avoid printing the instructions etc. into the CSV
    readingFile = False

    try:
        # Initialize serial
        ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=TIMEOUT_SECONDS)

        if not ser.is_open:
            print(f"Failed to open {SERIAL_PORT}. Exiting.")
            return 
        
        print("Connecting... waiting for device boot.")
        time.sleep(2) 

        # This with statement represents the logic flow for the user interacting with the teensy.
        # It assumes the user enters 'm' to enter management mode, enters 'l' to list the files,
        # enters 'e' to export a file, and then enters the file name. Entering different commands
        # or entering commands in a different order is not currently supported.

        with open(OUTPUT_FILE, 'a') as file:
            while True:
                # This loop prints the initial instructions
                line_bytes = ser.readline()
                if line_bytes:
                    try:
                        decoded_line = line_bytes.decode('utf-8').strip()
                        print(f"\r[Device]: {decoded_line}") # \r keeps the line clean

                        if (decoded_line == "Otherwise, 20Hz Logging starts automatically..."):
                            break
                    except UnicodeDecodeError:
                        pass
            
            # user will input 'm' to enter management mode
            ser.write((input(">> ") + '\n').encode('utf-8'))

            while True:
                # This loop prints the management mode command list
                line_bytes = ser.readline()
                if line_bytes:
                    try:
                        decoded_line = line_bytes.decode('utf-8').strip()
                        print(f"\r[Device]: {decoded_line}") # \r keeps the line clean

                        if (decoded_line == "------------------------------------"):
                            break
                    except UnicodeDecodeError:
                        pass
            
            # user will enter 'l' to list files
            ser.write((input(">> ") + '\n').encode('utf-8'))

            while True:
                # This loop lists the files
                line_bytes = ser.readline()
                if line_bytes:
                    try:
                        decoded_line = line_bytes.decode('utf-8').strip()
                        print(f"\r[Device]: {decoded_line}") # \r keeps the line clean

                        if (decoded_line == "------------------------------------"):
                            break
                    except UnicodeDecodeError:
                        pass

            # user enters 'e' to export a file
            ser.write((input(">> ") + '\n').encode('utf-8'))

            while True:
                # This loop displays the message "Enter exact filename (e.g. DATA005.CSV):"
                line_bytes = ser.readline()
                if line_bytes:
                    try:
                        decoded_line = line_bytes.decode('utf-8').strip()
                        print(f"\r[Device]: {decoded_line}") # \r keeps the line clean

                        if (decoded_line == "Enter exact filename (e.g. DATA005.CSV):"):
                            break
                    except UnicodeDecodeError:
                        pass


            # user enters filename
            ser.write((input(">> ") + '\n').encode('utf-8'))

            readingCSVcontents = False
            while True:
                # This loop prints the file contents to the output file on the user computer
                line_bytes = ser.readline()
                if line_bytes:
                    try:
                        decoded_line = line_bytes.decode('utf-8').strip()
                        print(f"\r[Device]: {decoded_line}") # \r keeps the line clean

                        if (decoded_line == "--- END OF FILE ---"):
                            readingCSVcontents = False
                            print("Done reading file contents")
                            break
                        if (decoded_line and readingCSVcontents):
                            file.write(decoded_line + '\n')
                            file.flush()
                        if (decoded_line == "--- START OF FILE ---"):
                            readingCSVcontents = True
                            print("Writing contents to file...")
                        
                        
                        
                    except UnicodeDecodeError:
                        pass
    except serial.SerialException as e:
        print(f"Error: {e}")
    except KeyboardInterrupt:
        print("\nScript stopped by user.")
    finally:
        if 'ser' in locals() and ser.is_open:
            ser.close()
            print("Connection closed.")

if __name__ == "__main__":
    run_serial_session()