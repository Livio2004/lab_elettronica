import tkinter as tk
from tkinter import messagebox
import serial
import serial.tools.list_ports

# Configura qui la porta e il baudrate
SERIAL_PORT = '/dev/cu.usbmodem102'     # Cambia in base al tuo sistema
BAUDRATE = 9600
TIMEOUT = 1               # Timeout per la ricezione in secondi

def send_and_receive():
    char = entry.get()
    
    # Cancella il messaggio precedente
    output_label.config(text="")
    root.update_idletasks()  # <-- forza il refresh immediato della GUI

    if len(char) != 1:
        messagebox.showerror("Errore", "Inserisci un solo carattere ASCII.")
        return

    try:
        ser.reset_input_buffer()
        ser.write(char.encode('ascii'))

        received = ser.read(1)
        if received:
            output_label.config(text=f"Ricevuto: '{received.decode('ascii')}'")
        else:
            output_label.config(text="Errore ricezione")
    except Exception as e:
        messagebox.showerror("Errore", f"Errore durante l'invio/ricezione: {e}")

# Inizializza la porta seriale
try:
    ser = serial.Serial(SERIAL_PORT, BAUDRATE, timeout=TIMEOUT)
except serial.SerialException as e:
    ser = None
    print(f"Errore apertura porta seriale: {e}")

# Crea la GUI
root = tk.Tk()
root.title("USART ASCII Tool")

tk.Label(root, text="Inserisci un carattere ASCII:").pack(pady=10)

entry = tk.Entry(root, width=5, font=("Arial", 18), justify='center')
entry.pack(pady=5)

send_button = tk.Button(root, text="Invia", command=send_and_receive)
send_button.pack(pady=10)

output_label = tk.Label(root, text="", font=("Arial", 14), fg="blue", width=25, anchor="center")
output_label.pack(pady=10)

root.mainloop()

# Chiudi la seriale alla fine
if ser and ser.is_open:
    ser.close()
 