import tkinter as tk
from tkinter import scrolledtext, messagebox
import serial
import struct
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import threading
import time

# --- CONFIGURAZIONE ---
# !!! MODIFICA QUESTA RIGA CON LA TUA PORTA SERIALE !!!
SERIAL_PORT = '/dev/cu.usbmodem1102'
BAUDRATE = 9600
NUM_SAMPLES = 100
BYTES_PER_SAMPLE = 4  # int32_t sono 4 byte
BYTES_TO_READ = NUM_SAMPLES * BYTES_PER_SAMPLE # 100 * 4 = 400 byte
# --------------------

ser = None
received_data = []

def send_command_and_plot(command):
    """Invia un comando e gestisce la ricezione e il plotting."""
    if not (ser and ser.is_open):
        messagebox.showerror("Errore", "Porta seriale non connessa.")
        return

    log_message(f"Invio comando: '{command}'...", "orange")
    
    try:
        # Pulisci il buffer di ricezione prima di inviare
        ser.reset_input_buffer()
        # Invia il comando seguito da '\n'
        ser.write((command + '\n').encode('ascii'))

        # Aspetta la risposta in base al comando
        if command == 'e':
            # Aspettiamo un blocco di dati binari
            log_message(f"In attesa di {BYTES_TO_READ} byte di dati...", "gray")
            raw_data = ser.read(BYTES_TO_READ)

            if len(raw_data) != BYTES_TO_READ:
                error_msg = f"Timeout o dati incompleti: ricevuti {len(raw_data)}/{BYTES_TO_READ} byte."
                log_message(error_msg, "red")
                messagebox.showerror("Errore di Ricezione", error_msg)
                return

            # Interpreta i byte grezzi
            # '<': Little-endian (standard per STM32)
            # 'i': signed integer a 4 byte (int32_t in C)
            # 'i' * NUM_SAMPLES: 100 di questi interi
            valori = list(struct.unpack('<' + 'i' * NUM_SAMPLES, raw_data))
            
            log_message(f"Ricevuti {len(valori)} campioni binari.", "blue")
            
            global received_data
            received_data = valori
            update_plot()

        else: # Per comandi come 'hello' che rispondono con testo
            line = ser.readline().decode('ascii', errors='ignore').strip()
            if line:
                log_message(f"Risposta: '{line}'", "blue")
            else:
                log_message("Timeout: nessuna risposta testuale ricevuta.", "red")

    except Exception as e:
        messagebox.showerror("Errore", f"Si è verificato un errore: {e}")

def update_plot():
    """Aggiorna il grafico."""
    ax.clear()
    if received_data:
        ax.plot(received_data, marker='o', linestyle='-', markersize=3)
    ax.set_title("Dati Ricevuti dal Microcontrollore")
    ax.set_xlabel("Indice Campione")
    ax.set_ylabel("Valore (int32)")
    ax.grid(True)
    ax.axhline(0, color='black', linewidth=0.5)
    canvas.draw()

def clear_data():
    """Pulisce i dati e il grafico."""
    global received_data
    received_data = []
    log_message("Dati e grafico puliti.", "green", clear=True)
    update_plot()

# --- GUI ---
# (La parte della GUI è identica a quella che abbiamo già corretto,
#  con i LabelFrame e il layout pack corretto. La includo per completezza)
root = tk.Tk()
root.title("STM32 Data Plotter (Binary Mode)")
root.geometry("800x650")

control_frame = tk.LabelFrame(root, text="Controlli", padx=10, pady=10)
control_frame.pack(padx=10, pady=5, fill=tk.X, side=tk.TOP) 

plot_button = tk.Button(control_frame, text="Richiedi Dati ('e')", command=lambda: send_command_and_plot('e'), bg="#4CAF50", fg="white")
plot_button.pack(side=tk.LEFT, padx=5)
hello_button = tk.Button(control_frame, text="Invia 'hello'", command=lambda: send_command_and_plot('hello'))
hello_button.pack(side=tk.LEFT, padx=5)
clear_button = tk.Button(control_frame, text="Pulisci", command=clear_data)
clear_button.pack(side=tk.LEFT, padx=5)

status_bar = tk.Label(root, text="Inizializzazione...", bd=1, relief=tk.SUNKEN, anchor=tk.W)
status_bar.pack(side=tk.BOTTOM, fill=tk.X)

log_frame = tk.LabelFrame(root, text="Log Comunicazione", padx=10, pady=10)
log_frame.pack(padx=10, pady=5, fill=tk.X, side=tk.BOTTOM)

log_text = scrolledtext.ScrolledText(log_frame, height=6, state=tk.DISABLED, font=("Courier New", 9))
log_text.pack(fill=tk.BOTH, expand=True)
log_text.tag_config("red", foreground="red")
log_text.tag_config("blue", foreground="blue")
log_text.tag_config("orange", foreground="orange")
log_text.tag_config("green", foreground="green")

plot_frame = tk.LabelFrame(root, text="Grafico", padx=10, pady=10)
plot_frame.pack(padx=10, pady=5, fill=tk.BOTH, expand=True, side=tk.TOP)

fig, ax = plt.subplots()
canvas = FigureCanvasTkAgg(fig, master=plot_frame)
canvas.get_tk_widget().pack(fill=tk.BOTH, expand=True)
update_plot()

def log_message(message, color="black", clear=False):
    if root.winfo_exists():
        log_text.config(state=tk.NORMAL)
        if clear:
            log_text.delete(1.0, tk.END)
        log_text.insert(tk.END, message + "\n", color)
        log_text.see(tk.END)
        log_text.config(state=tk.DISABLED)

# --- Inizializzazione e chiusura ---
try:
    ser = serial.Serial(SERIAL_PORT, BAUDRATE, timeout=2) # Timeout di 2 secondi
    status_bar.config(text=f"Connesso a {SERIAL_PORT} @ {BAUDRATE} bps", fg="dark green")
except serial.SerialException as e:
    status_bar.config(text=f"ERRORE: Impossibile aprire {SERIAL_PORT}. {e}", fg="red")
    ser = None

def on_closing():
    if ser and ser.is_open:
        ser.close()
    root.destroy()

root.protocol("WM_DELETE_WINDOW", on_closing)
root.mainloop()