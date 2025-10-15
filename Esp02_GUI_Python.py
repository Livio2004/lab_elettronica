import tkinter as tk
from tkinter import scrolledtext, messagebox
import serial
import serial.tools.list_ports
import threading
import time
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg

# --- CONFIGURAZIONE ---
# Porta seriale come da tua richiesta
SERIAL_PORT = '/dev/cu.usbmodem1102' 
BAUDRATE = 9600
NUM_SAMPLES_TOTAL = 100
# --------------------

# --- Variabili Globali ---
ser = None
received_data = [] # Lista per salvare i numeri ricevuti
is_auto_requesting = False # Flag per la modalità automatica

# --- Funzioni di Comunicazione ---

def send_request_command(command='s'):
    """Invia un comando al microcontrollore."""
    if ser and ser.is_open:
        try:
            log_message(f"Invio comando: '{command}'...", "orange")
            ser.write((command + '\n').encode('ascii'))
        except Exception as e:
            log_message(f"Errore invio: {e}", "red")
    else:
        log_message("Porta seriale non connessa.", "red")

def read_from_serial():
    """Funzione che gira in un thread separato per leggere la seriale."""
    while True:
        if not (ser and ser.is_open):
            break # Esce dal loop se la porta è chiusa
        try:
            if ser.in_waiting > 0:
                line = ser.readline().decode('ascii', errors='ignore').strip()
                if line:
                    # Schedula l'aggiornamento della GUI nel thread principale
                    root.after(0, process_received_line, line)
        except (serial.SerialException, TypeError, OSError):
            root.after(0, status_bar.config, {'text': "Connessione persa.", 'fg': 'red'})
            break
        time.sleep(0.05)

# --- Funzioni di Logica dell'Applicazione ---

def start_auto_request():
    """Avvia la richiesta automatica di tutti i campioni."""
    global is_auto_requesting
    if is_auto_requesting:
        return
    
    is_auto_requesting = True
    clear_data()
    
    thread = threading.Thread(target=auto_request_thread, daemon=True)
    thread.start()

def auto_request_thread():
    """Thread che gestisce la richiesta automatica."""
    global is_auto_requesting
    # Richiede N campioni + 1 per ricevere il messaggio "Fine trasmissione"
    for _ in range(NUM_SAMPLES_TOTAL + 1):
        if not is_auto_requesting:
            log_message("Richiesta automatica interrotta.", "orange")
            break
        send_request_command()
        time.sleep(0.05)
    is_auto_requesting = False
    
def stop_auto_request():
    """Ferma la richiesta automatica."""
    global is_auto_requesting
    is_auto_requesting = False

def process_received_line(line):
    """Processa la linea ricevuta e aggiorna la GUI."""
    log_message(f"Ricevuto: '{line}'", "blue")
    try:
        value = int(line)
        received_data.append(value)
        update_plot()
    except ValueError:
        pass # Messaggio di stato già loggato.

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
    """Pulisce i dati, il grafico e i log."""
    global received_data
    received_data = []
    log_message("Dati e grafico puliti.", "green", clear=True)
    update_plot()

# --- GUI ---
root = tk.Tk()
root.title("STM32 Data Plotter (ASCII Mode)")
root.geometry("800x650")

# --- Frame di controllo (in alto) ---
control_frame = tk.LabelFrame(root, text="Controlli", padx=10, pady=10)
control_frame.pack(padx=10, pady=5, fill=tk.X, side=tk.TOP) 

request_button = tk.Button(control_frame, text="Richiedi 1 Campione", command=send_request_command)
request_button.pack(side=tk.LEFT, padx=5)

# CORREZIONE: usava 'control_gframe' invece di 'control_frame'
auto_button = tk.Button(control_frame, text="Richiedi Tutti", command=start_auto_request, bg="#4CAF50", fg="white")
auto_button.pack(side=tk.LEFT, padx=5)

stop_button = tk.Button(control_frame, text="Stop", command=stop_auto_request, bg="#f44336", fg="white")
stop_button.pack(side=tk.LEFT, padx=5)
clear_button = tk.Button(control_frame, text="Pulisci", command=clear_data)
clear_button.pack(side=tk.LEFT, padx=5)

# --- Barra di Stato (in basso) ---
status_bar = tk.Label(root, text="Inizializzazione...", bd=1, relief=tk.SUNKEN, anchor=tk.W)
status_bar.pack(side=tk.BOTTOM, fill=tk.X)

# --- Frame per il Log (sotto la barra di stato ma prima del grafico) ---
log_frame = tk.LabelFrame(root, text="Log Comunicazione", padx=10, pady=10)
log_frame.pack(padx=10, pady=5, fill=tk.X, side=tk.BOTTOM)

log_text = scrolledtext.ScrolledText(log_frame, height=6, state=tk.DISABLED, font=("Courier New", 9))
log_text.pack(fill=tk.BOTH, expand=True)
log_text.tag_config("red", foreground="red")
log_text.tag_config("blue", foreground="blue")
log_text.tag_config("orange", foreground="orange")
log_text.tag_config("green", foreground="green")

# --- Frame per il Grafico (riempie lo spazio rimanente) ---
plot_frame = tk.LabelFrame(root, text="Grafico", padx=10, pady=10)
plot_frame.pack(padx=10, pady=5, fill=tk.BOTH, expand=True, side=tk.TOP)

fig, ax = plt.subplots()
canvas = FigureCanvasTkAgg(fig, master=plot_frame)
canvas.get_tk_widget().pack(fill=tk.BOTH, expand=True)
update_plot()

# --- Funzione per il Log (Thread-safe) ---
def log_message(message, color="black", clear=False):
    def update_gui():
        log_text.config(state=tk.NORMAL)
        if clear:
            log_text.delete(1.0, tk.END)
        log_text.insert(tk.END, message + "\n", color)
        log_text.see(tk.END)
        log_text.config(state=tk.DISABLED)
    if root.winfo_exists():
        root.after(0, update_gui)

# --- Inizializzazione e chiusura ---
try:
    ser = serial.Serial(SERIAL_PORT, BAUDRATE, timeout=1)
    status_bar.config(text=f"Connesso a {SERIAL_PORT} @ {BAUDRATE} bps", fg="dark green")
    read_thread = threading.Thread(target=read_from_serial, daemon=True)
    read_thread.start()
except serial.SerialException as e:
    status_bar.config(text=f"ERRORE: Impossibile aprire {SERIAL_PORT}. {e}", fg="red")
    ser = None

def on_closing():
    global is_auto_requesting
    is_auto_requesting = False
    if ser and ser.is_open:
        ser.close()
    root.destroy()

root.protocol("WM_DELETE_WINDOW", on_closing)
root.mainloop()