import tkinter as tk
from tkinter import messagebox
import serial
import serial.tools.list_ports

# Configura qui la porta e il baudrate
SERIAL_PORT = '/dev/cu.usbmodem102'      # Cambia in base al tuo sistema
BAUDRATE = 9600
TIMEOUT = 1               # Timeout per la ricezione in secondi
MAX_CHARS = 20            # Massimo numero di caratteri (escluso terminatore)
STRING_TERMINATOR = '\n'  # Terminatore di stringa (puoi cambiare in '\0' se necessario)

def send_and_receive():
    text = entry.get()
    
    # Cancella il messaggio precedente
    output_label.config(text="")
    root.update_idletasks()  # Forza il refresh immediato della GUI
    
    # Verifica lunghezza
    if len(text) == 0:
        messagebox.showerror("Errore", "Inserisci almeno un carattere.")
        return
    
    if len(text) > MAX_CHARS:
        messagebox.showerror("Errore", f"La stringa deve essere massimo {MAX_CHARS} caratteri.")
        return
    
    try:
        # Prepara la stringa con terminatore
        data_to_send = text + STRING_TERMINATOR
        
        # Pulisci il buffer e invia
        ser.reset_input_buffer()
        ser.write(data_to_send.encode('ascii'))
        
        # Ricevi fino al terminatore o timeout
        received_bytes = bytearray()
        while True:
            byte = ser.read(1)
            if not byte:  # Timeout
                output_label.config(text="Timeout: nessuna risposta ricevuta", fg="red")
                return
            
            received_bytes.extend(byte)
            
            # Controlla se abbiamo ricevuto il terminatore
            if byte.decode('ascii') == STRING_TERMINATOR:
                break
        
        # Decodifica e mostra (rimuovi il terminatore per la visualizzazione)
        received_text = received_bytes.decode('ascii').rstrip(STRING_TERMINATOR)
        output_label.config(
            text=f"Ricevuto ({len(received_text)} char): '{received_text}'", 
            fg="blue"
        )
        
    except UnicodeDecodeError:
        output_label.config(text="Errore: caratteri non ASCII ricevuti", fg="red")
    except Exception as e:
        messagebox.showerror("Errore", f"Errore durante l'invio/ricezione: {e}")

# Inizializza la porta seriale
try:
    ser = serial.Serial(SERIAL_PORT, BAUDRATE, timeout=TIMEOUT)
except serial.SerialException as e:
    ser = None
    print(f"Errore apertura porta seriale: {e}")
    messagebox.showerror("Errore Seriale", 
                        f"Impossibile aprire {SERIAL_PORT}\n{e}\n\nL'applicazione continuerà ma non funzionerà.")

# Crea la GUI
root = tk.Tk()
root.title("USART String Tool")

tk.Label(root, text=f"Inserisci una stringa (max {MAX_CHARS} caratteri):").pack(pady=10)

entry = tk.Entry(root, width=30, font=("Arial", 14), justify='center')
entry.pack(pady=5)

# Mostra contatore caratteri
char_counter = tk.Label(root, text="0/20 caratteri", font=("Arial", 10), fg="gray")
char_counter.pack(pady=2)

def update_counter(event=None):
    current_len = len(entry.get())
    char_counter.config(text=f"{current_len}/{MAX_CHARS} caratteri")
    if current_len > MAX_CHARS:
        char_counter.config(fg="red")
    else:
        char_counter.config(fg="gray")

entry.bind("<KeyRelease>", update_counter)

send_button = tk.Button(root, text="Invia e Ricevi", command=send_and_receive, 
                       font=("Arial", 12), bg="#4CAF50", fg="white", padx=20)
send_button.pack(pady=10)

# Info sul terminatore
tk.Label(root, text=f"Terminatore: {repr(STRING_TERMINATOR)}", 
         font=("Arial", 9), fg="gray").pack(pady=5)

output_label = tk.Label(root, text="", font=("Arial", 12), fg="blue", 
                       width=40, anchor="center", wraplength=350)
output_label.pack(pady=10)

root.mainloop()

# Chiudi la seriale alla fine
if ser and ser.is_open:
    ser.close()