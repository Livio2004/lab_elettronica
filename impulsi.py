import math
import numpy as np
import matplotlib.pyplot as plt

# --- 1. Definizione delle costanti fisiche ---
# Le stesse del tuo esempio
tau_1 = 15.0  # Prima costante di tempo (secondi)
tau_2 = 10.0  # Seconda costante di tempo (secondi)
Delta_t = 1.0 # Intervallo di campionamento (secondi)
N_SAMPLES = 100 # Numero di campioni nel tempo (da 0 a 99 secondi)
A_m = 60000   # Ampiezza iniziale a t=0, come nel tuo esempio

# --- 2. Calcolo delle costanti per l'aritmetica a virgola fissa ---
# Questo simula esattamente i #define che useresti nel microcontrollore.
# Usiamo 2^10 (1024) come fattore di scala, come suggerito.
SCALE_FACTOR_BITS = 10
SCALE_FACTOR = 1 << SCALE_FACTOR_BITS  # Questo è 2**10, ovvero 1024

# Calcola il valore in virgola mobile
costante_1_float = math.exp(-Delta_t / tau_1) # Risultato: ~0.9355
costante_2_float = math.exp(-Delta_t / tau_2) # Risultato: ~0.9048

# Converte in intero a 16 bit scalato (fixed-point)
# Questo è l'equivalente di: (uint16_t)((0.9355)*(1<<10)) in C
costante_1 = np.uint16(math.floor(costante_1_float * SCALE_FACTOR))
costante_2 = np.uint16(math.floor(costante_2_float * SCALE_FACTOR))

print(f"Costante 1 (float): {costante_1_float:.4f}")
print(f"Costante 1 (intero scalato a 16 bit): {costante_1}")
print(f"Costante 2 (float): {costante_2_float:.4f}")
print(f"Costante 2 (intero scalato a 16 bit): {costante_2}")
print("-" * 30)


# --- 3. Generazione dei vettori esponenziali ---

# Inizializziamo gli array a 16 bit, come faresti sul micro.
# Usiamo numpy perché ci permette di specificare il tipo di dato (uint16).
esponenziale_1 = np.zeros(N_SAMPLES, dtype=np.uint16)
esponenziale_2 = np.zeros(N_SAMPLES, dtype=np.uint16)

# Imposta il valore iniziale (a t=0)
esponenziale_1[0] = np.uint16(A_m)
esponenziale_2[0] = np.uint16(A_m)

# Calcola i valori successivi in modo iterativo, simulando il microcontrollore
for ii in range(1, N_SAMPLES): # In Python, il loop va da 1 a 99
    # Calcolo per il primo esponenziale
    # Eseguiamo il casting a 32 bit per evitare overflow durante la moltiplicazione,
    # esattamente come suggerito nel tuo testo.
    moltiplicazione_1 = np.uint32(esponenziale_1[ii-1]) * np.uint32(costante_1)
    
    # Eseguiamo la divisione con lo shift a destra di 10 bit.
    # Il risultato viene poi salvato nell'array a 16 bit.
    esponenziale_1[ii] = moltiplicazione_1 >> SCALE_FACTOR_BITS

    # Calcolo per il secondo esponenziale
    moltiplicazione_2 = np.uint32(esponenziale_2[ii-1]) * np.uint32(costante_2)
    esponenziale_2[ii] = moltiplicazione_2 >> SCALE_FACTOR_BITS

# Stampiamo i primi 10 elementi per verifica, come in Matlab
# (in Python gli indici partono da 0)
print("Primi 10 elementi di esponenziale_1:")
print(esponenziale_1[0:10])
print("\nPrimi 10 elementi di esponenziale_2:")
print(esponenziale_2[0:10])
print("-" * 30)


# --- 4. Calcolo della differenza ---

# Grazie a numpy, la sottrazione tra array è semplice come in Matlab.
# ATTENZIONE: il risultato potrebbe avere valori negativi, quindi non usiamo uint16
# ma un tipo con segno (int32 è una scelta sicura).
vet_dif = np.int32(esponenziale_1) - np.int32(esponenziale_2)

print("Primi 10 elementi del vettore differenza:")
print(vet_dif[0:10])
print("-" * 30)


# --- 5. Disegno del grafico (Plot) ---

# Creiamo un asse temporale per il grafico, da 0 a 99
tempo = np.arange(N_SAMPLES)

plt.figure(figsize=(10, 6)) # Crea una finestra per il grafico di dimensioni leggibili
plt.plot(tempo, vet_dif, marker='o', linestyle='-', markersize=4) # Disegna i punti e li collega con una linea

# Aggiungiamo etichette e titolo per chiarezza
plt.title("Differenza tra due esponenziali (Simulazione con Aritmetica a Interi)")
plt.xlabel("Tempo (campioni, t = k * Delta_t)")
plt.ylabel("Ampiezza (valore intero)")
plt.grid(True) # Aggiunge una griglia per facilitare la lettura
plt.axhline(0, color='black', linewidth=0.5) # Disegna la linea dello zero

# Mostra il grafico a schermo
plt.show()


'''
In C 
uint_16_t espo[N_SAMPLES];
uint_t *spedizione = espo; // Puntatore per scorrere l'array

spedizione = (uint8_t *)&espo[0]; // Puntatore a 8 bit per invio byte-wise
for (int ii=0; ii<N_SAMPLES; ii++) {
}
'''