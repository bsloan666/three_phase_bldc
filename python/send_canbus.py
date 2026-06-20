import can
import time

def send_hello():
    # 1. CONFIGURE YOUR INTERFACE
    # Replace 'slcan' and 'COM3' with your specific adapter settings.
    # Common examples:
    #   Candlelight/mks-can: interface='gs_usb', channel=0
    #   Seeed Studio / SLCAN: interface='slcan', channel='COM3' (or '/dev/ttyUSB0')
    #   Peak PCAN:          interface='pcan', channel='PCAN_USBBUS1'
    try:
        bus = can.interface.Bus(
            interface='slcan', 
            channel='/dev/cu.usbmodem207C395C59421', 
            bitrate=1000000  # 1 Mbps to match your ESP32 configuration
        )
        print("Successfully connected to CAN adapter.")
    except Exception as e:
        print(f"Error connecting to hardware: {e}")
        print("Please check your 'interface' and 'channel' parameters.")
        return

    # 2. CONSTRUCT THE "HELLO WORLD" PAYLOAD
    # CAN data can hold a maximum of 8 bytes.
    # ASCII for "HELLO WO"
    hello_payload = [0x48, 0x45, 0x4C, 0x4C, 0x4F, 0x20, 0x57, 0x4F] 

    # 3. CREATE THE MESSAGE
    # id=5 matches the default myNodeID in the ESP32 script
    msg = can.Message(
        arbitration_id=5,
        data=hello_payload,
        is_extended_id=False  # Standard 11-bit ID
    )

    # 4. TRANSMIT
    try:
        bus.send(msg)
        print(f"Message sent successfully to ID {msg.arbitration_id}!")
        print("Check your ESP32 Serial Monitor for: 0x48 0x45 0x4C 0x4C 0x4F 0x20 0x57 0x4F")
    except can.CanError:
        print("Message failed to transmit down the wire.")
    finally:
        bus.shutdown()

if __name__ == "__main__":
    send_hello()

