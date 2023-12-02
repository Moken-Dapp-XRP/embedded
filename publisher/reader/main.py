import sensor
import image
import time
from machine import UART
from fpioa_manager import fm
from board import board_info
from Maix import GPIO


class LEDManager:
    LED_PINS = [GPIO.GPIO0, GPIO.GPIOHS0, GPIO.GPIO2]

    def __init__(self):
        for pin in self.LED_PINS:
            fm.register(board_info.LED_R, fm.fpioa.GPIO0, force=True)

        self.leds = [GPIO(pin, GPIO.OUT) for pin in self.LED_PINS]
        self.turn_off()

    def turn_on(self):
        for led in self.leds:
            led.value(1)

    def turn_off(self):
        for led in self.leds:
            led.value(0)

    def cleanup(self):
        for pin in self.LED_PINS:
            fm.unregister(pin)


class UARTManager:
    def __init__(self):
        fm.register(10, fm.fpioa.UART1_TX, force=True)
        fm.register(15, fm.fpioa.UART1_RX, force=True)
        self.uart_A = UART(UART.UART1, 115200, 8, 1, 0, timeout=1000, read_buf_len=4096)

    def write_payload(self, payload):
        self.uart_A.write(payload)

    def cleanup(self):
        self.uart_A.deinit()


class CameraManager:
    def __init__(self):
        sensor.reset()
        sensor.set_pixformat(sensor.RGB565)
        sensor.set_framesize(sensor.QVGA)
        sensor.set_vflip(1)
        sensor.run(1)
        sensor.skip_frames(30)

    def capture_image(self):
        return sensor.snapshot()


class QRCodeProcessor:
    @staticmethod
    def find_qrcodes(img):
        return img.find_qrcodes()


class Application:
    def __init__(self):
        self.led_manager = LEDManager()
        self.uart_manager = UARTManager()
        self.camera_manager = CameraManager()
        self.qr_processor = QRCodeProcessor()

    def run(self):
        while True:
            clock.tick()

            img = self.camera_manager.capture_image()
            qr_codes = self.qr_processor.find_qrcodes(img)

            if len(qr_codes) > 0:
                payload = qr_codes[0].payload()
                print(payload)
                self.uart_manager.write_payload(payload)
                self.led_manager.turn_off()
            else:
                self.led_manager.turn_on()

    def cleanup(self):
        self.led_manager.cleanup()
        self.uart_manager.cleanup()


app = Application()

try:
    app.run()
finally:
    app.cleanup()
