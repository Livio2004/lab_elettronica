import tkinter as tk
from tkinter import scrolledtext
import serial
import serial.tools.list_ports
import threading
import time

# --- CONFIGURAZIONE ---
# !!! MODIFICA QUESTA RIGA CON LA TUA PORTA SERIALE !!!
# Esempi: 'COM3' per Windows, '/dev/cu.usbmodem12345' per macOS, '/dev/ttyACM0' per Linux
SERIAL_PORT = '/dev/cu.usbmodem1102'
BAUDRATE = 9600
# --------------------

ser = None

def send_command(command):
    if ser and ser.is_open:
        try:
            log_message(f"Invio: {command}", "orange")
            # Aggiungiamo '\n' che il microcontrollore si aspetta come terminatore
            full_command = command + '\n'
            ser.write(full_command.encode('ascii'))
        except Exception as e:
            log_message(f"Errore invio: {e}", "red")
    else:
        log_message("Porta seriale non connessa.", "red")

def read_from_serial():
    """Funzione che gira in un thread separato per leggere la seriale."""
    while True:
        if ser and ser.is_open:
            try:
                if ser.in_waiting > 0:
                    line = ser.readline().decode('ascii').strip()
                    if line:
                        log_message(f"Ricevuto: {line}", "blue")
            except (serial.SerialException, TypeError):
                break # Esce dal loop se la porta viene chiusa
        time.sleep(0.1)

def log_message(message, color="black"):
    """Funzione thread-safe per aggiornare la GUI."""
    if root.winfo_exists(): # Controlla se la finestra esiste ancora
        log_text.config(state=tk.NORMAL)
        log_text.insert(tk.END, message + "\n", color)
        log_text.see(tk.END)  # Auto-scroll
        log_text.config(state=tk.DISABLED)

# --- Inizializza la porta seriale ---
try:
    ser = serial.Serial(SERIAL_PORT, BAUDRATE, timeout=1)
    print(f"Porta {SERIAL_PORT} aperta con successo.")
except serial.SerialException as e:
    print(f"Errore: Impossibile aprire la porta seriale {SERIAL_PORT}. {e}")

# --- GUI ---
root = tk.Tk()
root.title("STM32 LED Controller")
root.geometry("400x300")

main_frame = tk.Frame(root, padx=10, pady=10)
main_frame.pack(fill=tk.BOTH, expand=True)

button_frame = tk.Frame(main_frame)
button_frame.pack(pady=10)

on_button = tk.Button(button_frame, text="Accendi LED", command=lambda: send_command("LED_ON"), 
                      font=("Arial", 14), bg="#4CAF50", fg="white", width=15)
on_button.pack(side=tk.LEFT, padx=10)

off_button = tk.Button(button_frame, text="Spegni LED", command=lambda: send_command("LED_OFF"), 
                       font=("Arial", 14), bg="#f44336", fg="white", width=15)
off_button.pack(side=tk.LEFT, padx=10)

log_label = tk.Label(main_frame, text="Log Comunicazione:")
log_label.pack(anchor="w")
log_text = scrolledtext.ScrolledText(main_frame, height=8, state=tk.DISABLED)
log_text.pack(fill=tk.BOTH, expand=True, pady=5)
log_text.tag_config("red", foreground="red")
log_text.tag_config("blue", foreground="blue")
log_text.tag_config("orange", foreground="orange")

# Avvia il thread di lettura
if ser:
    thread = threading.Thread(target=read_from_serial, daemon=True)
    thread.start()

def on_closing():
    if ser and ser.is_open:
        ser.close()
    root.destroy()

root.protocol("WM_DELETE_WINDOW", on_closing)

if not ser:
    log_message(f"ERRORE: Porta {SERIAL_PORT} non trovata.", "red")

root.mainloop()