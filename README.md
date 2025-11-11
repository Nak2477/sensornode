# Smart Sensor Node

En IoT-sensor som mäter temperatur och skickar data till molnservrar med nätverksåterställning. Utvecklad enligt kravspecifikation för periodiska mätningar med databevarande vid nätverksavbrott.

## Funktioner

- **Temperatursensor**: Simulerar realistiska temperaturavläsningar med drift och brus
- **Periodiska mätningar**: Automatiska mätningar var 60:e sekund
- **Nätverksåterställning**: Sparar data lokalt vid nätverksfel, fortsätter drift
- **HTTP POST-kommunikation**: Skickar JSON-data via TCP/IP till molnservrar
- **Lokal datalagring**: Textfil-backup för databevarande vid avbrott
- **Kontinuerlig drift**: Både enkelmätningar och kontinuerlig övervakningsläge

## Projektstruktur

```
sensornode/
├── Makefile              # Byggkonfiguration med GCC och pedantic flags
├── README.md             # Projektdokumentation
├── include/              # Header-filer
│   ├── sensor.h          # Huvudstrukturer, funktionsdeklarationer och function pointers
│   ├── http.h            # HTTP-klient strukturer och funktioner
│   └── tcp.h             # TCP socket-funktioner
├── src/                  # Källkodsfiler
│   ├── main.c            # Huvudprogram med kommandoradshantering (char **argv)
│   ├── sensor.c          # Sensordata, JSON-formatering och nätverkskommunikation
│   ├── http_client.c     # HTTP POST-implementering för httpbin.org
│   └── tcp_socket.c      # TCP socket-hantering och serveranslutning
├── build/                # Byggda filer (skapas automatiskt)
│   └── sensornode        # Körbar fil
├── obj/                  # Objekt-filer från kompilering (skapas automatiskt)
│   └── *.o               # Kompilerade objektfiler
└── bin/                  # Backup-data (skapas vid nätverksfel)
    └── saved_temp.txt    # Lokal datalagring vid nätverksavbrott
```

## Kompilering

```bash
# Kompilera hela projektet
make

# Eller kompilera och rensa i ett steg
make clean && make

# Rensa byggfiler
make clean
```

## Användning

### Kontinuerlig sensorövervakning (rekommenderat)
```bash
# Starta kontinuerliga mätningar var 60:e sekund
./build/sensornode --continuous

# Tryck Ctrl+C för att stoppa
```

### Enkelmätningar
```bash
# En automatisk sensormätning
./build/sensornode

# En mätning med specifik temperatur  
./build/sensornode --single 25.3

# Kontinuerliga mätningar var 60:e sekund
./build/sensornode --continuous

# Visa hjälp och alla alternativ
./build/sensornode --help
```

### Kommandoradsalternativ

- **`--continuous, -c`**: Kontinuerliga mätningar var 60:e sekund
- **`--single [temp]`**: En enskild mätning (standard: automatisk sensor)  
- **`--help, -h`**: Visa hjälp och användning

## Nätverksåterställning

Systemet hanterar nätverksfel elegant:

- **Normal drift**: Data skickas direkt till server via HTTP POST
- **Nätverksfel**: Data sparas lokalt i `saved_temp.txt`
- **Automatisk återställning**: Fortsätter mätningar även vid långvariga avbrott
- **Databevarande**: Ingen data går förlorad vid kortare nätverksavbrott

```bash
# Vid nätverksfel skapas backup-fil
cat bin/saved_temp.txt
id:node001temp:23.4time:2025-11-11T18:25:35Z
id:node001temp:22.5time:2025-11-11T18:26:21Z
```

## JSON-dataformat

Sensordata skickas enligt kravspecifikation i följande JSON-format:

```json
{
    "device_id": "node001",
    "temperature": 23.4,
    "timestamp": "2025-11-10T10:15:00Z"
}
```

**Fältbeskrivning:**
- **device_id**: Unik identifierare för sensornoden (node001)
- **temperature**: Temperaturvärde i Celsius (numeriskt värde)
- **timestamp**: ISO 8601-formaterad tidsstämpel i UTC

## Teknisk Implementation

### Function Pointers
Projektet använder funktionspekare för flexibel hantering:

```c
// Logger function pointer för olika typer av utskrifter
typedef void (*logger_t)(const char *message);
send_sensor_data(&sensor_data, console_log);  // Använder console_log
send_sensor_data(&sensor_data, debug_log);    // Eller debug_log

// Sensor data generator function pointer
typedef sensor_data_t (*sensor_generator_t)(void);
data_generator = generate_auto_sensor_data;
sensor_data = data_generator();  // Anropar via funktionspekare
```

### Pekare till Pekare
Kommandoradshantering med `char **argv`:
```c
int main(int argc, char **argv) {
    if (strcmp(argv[1], "--continuous") == 0) {  // argv[1] access
        // Kontinuerlig drift
    }
}
```

### TCP/IP Kommunikation
- **Server**: httpbin.org (port 80)  
- **Protokoll**: HTTP POST över TCP/IP
- **Endpoint**: /post
- **Content-Type**: application/json

## Exempel på körning

### Kontinuerlig sensorövervakning
```bash
$ ./build/sensornode --continuous
=== SensorNode Starting ===
Mode: Continuous measurements every 60 seconds
Press Ctrl+C to stop

Measurement: 23.2°C at 2025-11-10T10:15:00Z
Skickar JSON-data:
{
    "device_id": "node001",
    "timestamp": "2025-11-10T10:15:00Z",
    "temperature": 23.2
}
Data sent (HTTP 200)
✅ Data transmitted successfully
Waiting 60 seconds for next measurement...

Measurement: 23.7°C at 2025-11-10T10:16:00Z
...
```

### Enskild mätning med nätverksfel
```bash
$ ./build/sensornode --single
=== SensorNode Starting ===
Mode: Single measurement

Measurement: 24.1°C at 2025-11-10T10:20:00Z
Skickar JSON-data:
{
    "device_id": "node001", 
    "timestamp": "2025-11-10T10:20:00Z",
    "temperature": 24.1
}
Network error - no response
❌ Transmission failed - saving locally
Data saved: node001,24.1,2025-11-10T10:20:00Z
```

## Kravspecifikation - Uppfyllelse

✅ **Temperatursensor**: Simulerad sensor med realistiskt beteende  
✅ **Lokal datalagring**: Textfil för databevarande vid nätverksavbrott  
✅ **Periodiska mätningar**: Var 60:e sekund enligt krav  
✅ **Tidsstämpel, enhets-ID, temperatur**: Alla fält inkluderade  
✅ **Återuppta överföring**: Fortsätter drift efter avbrott utan dataförlust  
✅ **TCP/IP-kommunikation**: HTTP POST över TCP/IP  
✅ **JSON-format**: Exakt enligt specifikation  

## Systemkrav

- **Kompilator**: GCC med C99-standard
- **System**: Linux/Unix
- **Nätverk**: Internetanslutning för molnkommunikation
- **Minne**: Minimal användning, lämplig för mikrokontrollers

## Säkerhet och begränsningar

- **Utbildningssyfte**: Utvecklad för demonstration och lärande
- **Oskyddad kommunikation**: HTTP (inte HTTPS) - data kan avlyssnas
- **Minnesbegränsning**: Ingen automatisk rensning av gamla backupfiler
- **Miljöfaktorer**: Sensornoggrannhet kan påverkas av externa förhållanden

## Arkitektur

Systemet använder modulär C-design med:
- **Funktionspekare**: För flexibel felhantering
- **Strukturerat data**: `sensor_data_t` för typesäkerhet  
- **Minneshantering**: Korrekt allokering och frigöring
- **Felåterställning**: Graceful degradation vid nätverksfel