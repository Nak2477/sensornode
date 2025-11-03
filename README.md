# SensorNode HTTP POST Client

En ren och fokuserad C-applikation som demonstrerar TCP socket-kommunikation, HTTP POST-förfrågningar, funktionspekare och pekare till pekare.

## Funktioner

- **TCP Socket-anslutningar**: Ansluter till servrar via TCP (example.com port 80, httpbin.org)
- **HTTP POST-förfrågningar**: Skickar JSON-formaterad sensordata till servrar
- **Funktionspekare**: Använder funktionspekare för olika responshanteringsfunktioner
- **Pekare till pekare**: Demonstrerar `char **argv` och dynamiskt tilldelade stränglister
- **Modulär design**: Separata .c och .h-filer för olika funktionaliteter

## Projektstruktur

```
sensornode/
├── Makefile
├── README.md
├── include/
│   └── http_client.h      # Funktionsdeklarationer och strukturer
└── src/
    ├── main.c             # Huvudprogram
    ├── tcp_client.c       # TCP socket-funktionalitet
    ├── data_formatter.c   # JSON/XML formatering av sensordata
    ├── handlers.c         # Funktionspekare för response-hantering
    └── pointer_utils.c    # Pekare-till-pekare funktionalitet
```

## Kompilering

```bash
# Kompilera hela projektet
make

# Kompilera och kör med standardparametrar
make run

# Rensa byggfiler
make clean

# Visa alla tillgängliga mål
make info
```

## Användning

```bash
# Grundläggande användning
./bin/sensornode

# Med specifika parametrar
./bin/sensornode [handler] [temperatur]

# Exempel
./bin/sensornode print 25.3
./bin/sensornode log 18.7

# Visa hjälp
./bin/sensornode --help
```

### Parametrar

- **handler**: `print` eller `log` (default: print)
- **temperatur**: Temperaturvärde i Celsius (default: 23.5)

## Funktionspekare

Programmet demonstrerar funktionspekare genom:

- **Response handlers**: `print_response_handler()` och `log_response_handler()`
- **Error handlers**: `default_error_handler()`

Användaren kan välja vilka funktioner som ska användas via kommandoradsparametrar.

## Pekare till pekare

Programmet demonstrerar pekare till pekare genom:

- **`char **argv`**: Kommandoradsargument
- **Dynamiska stränglister**: `parse_arguments()` funktionalitet
- **Strängarray-hantering**: `free_string_array()` för minneshantering

## HTTP POST Format

Programmet skickar sensordata i följande JSON-format:

```json
{
    "device": "UUID",
    "time": "<timestamp>",
    "temperature": "<temperature>°C"
}
```

## Testservrar

- **httpbin.org**: Primär testserver för HTTP POST-förfrågningar
- **example.com**: Testar grundläggande TCP-anslutning (port 80)

## Exempel på körning

```bash
$ make run
Kompilerar src/main.c...
Kompilerar src/tcp_client.c...
Kompilerar src/data_formatter.c...
Kompilerar src/handlers.c...
Kompilerar src/pointer_utils.c...
Länker bin/sensornode...
Kompilering klar! Kör med: ./bin/sensornode
./bin/sensornode

=== SensorNode HTTP POST Client ===
Utvecklad för att demonstrera:
- TCP socket-anslutningar
- HTTP POST-förfrågningar
- Funktionspekare
- Pekare till pekare (char **argv och dynamiska listor)

=== Demonstration av pekare till pekare ===
1. Kommandoradsargument (char **argv):
   argv[0] = "./bin/sensornode"

[... mer output ...]
```

## Systemkrav

- GCC compiler
- Linux/Unix system
- Internetanslutning för att testa mot externa servrar

## Säkerhet

Denna applikation är utvecklad för utbildningssyfte. Använd inte i produktionsmiljöer utan ytterligare säkerhetsförstärkningar.