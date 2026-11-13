/*
   Practica 2 - Lenguajes y Paradigmas de la Computacion
   Miguel Salazar
   Juan Sebastian

   Compilar: g++ -std=c++17 -Wall -Wextra -o practice main.cpp
*/

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <numeric>
#include <map>
#include <functional>

// APOYO: Formato de pesos colombianos
// Convierte un valor como 3820 en el texto "$ 3.820" para mostrarlo en pantalla.
std::string formatCOP(long long value) {
    std::string digits = std::to_string(value);
    std::string out;
    int count = 0;
    for (auto it = digits.rbegin(); it != digits.rend(); ++it) {
        out.push_back(*it);
        if (++count % 3 == 0 && it + 1 != digits.rend()) {
            out.push_back('.');
        }
    }
    std::reverse(out.begin(), out.end());
    return "$ " + out;
}

// PARTE 1: Medios de transporte y herencia
// TransportMode guarda lo comun: nombre, estaciones y reglas del transporte.
// Es abstracta: baseFare y serviceType tienen = 0 y las clases hijas los implementan.
// El polimorfismo permite usar un TransportMode y ejecutar la version de cada hijo.
// El destructor virtual permite destruir correctamente los hijos desde la clase base.
class TransportMode {
public:
    TransportMode(std::string name, int stations)
        : name_(std::move(name)), stations_(stations) {}

    virtual ~TransportMode() = default;          // mandatory: deleted through base pointer

    const std::string& name() const { return name_; }
    int stations() const { return stations_; }

    virtual long long baseFare() const = 0;
    virtual std::string serviceType() const = 0;

    // Estas reglas indican si admite tarifa estudiantil, integracion y abrir la ventana.
    virtual bool allowsStudentFlatFare() const { return true; }
    virtual bool allowsIntegration() const { return true; }
    virtual bool opensIntegrationWindow() const { return true; }

    std::string describe() const {
        return serviceType() + " " + name() + " (" +
               std::to_string(stations()) + " stations) - full fare " +
               formatCOP(baseFare());
    }

private:
    std::string name_;
    int stations_;
};

// Metro, tranvia y cable heredan los datos comunes y definen tarifa y tipo de servicio.
class MetroLine : public TransportMode {
public:
    MetroLine(std::string name, int stations)
        : TransportMode(std::move(name), stations) {}
    long long baseFare() const override { return 3820; }
    std::string serviceType() const override { return "Metro line"; }
};

class TramLine : public TransportMode {
public:
    TramLine(std::string name, int stations)
        : TransportMode(std::move(name), stations) {}
    long long baseFare() const override { return 3820; }
    std::string serviceType() const override { return "Tram line"; }
};

class CableLine : public TransportMode {
public:
    CableLine(std::string name, int stations)
        : TransportMode(std::move(name), stations) {}
    long long baseFare() const override { return 3820; }
    std::string serviceType() const override { return "Cable line"; }
};

// El cable turistico reutiliza la tarifa del cable y suma un recargo.
// No admite tarifa plana estudiantil ni participa en los transbordos integrados.
class TouristCable : public CableLine {
public:
    TouristCable(std::string name, int stations, long long surcharge)
        : CableLine(std::move(name), stations), surcharge_(surcharge) {}
    long long baseFare() const override {
        return CableLine::baseFare() + surcharge_;
    }
    std::string serviceType() const override { return "Tourist cable"; }
    bool allowsStudentFlatFare() const override { return false; }
    bool allowsIntegration() const override { return false; }
    bool opensIntegrationWindow() const override { return false; }

private:
    long long surcharge_;
};

// El alimentador tiene tarifa base cero y no abre la ventana de integracion.
// Su cobro final depende del perfil del pasajero.
class FeederRoute : public TransportMode {
public:
    FeederRoute(std::string name, int stations)
        : TransportMode(std::move(name), stations) {}
    long long baseFare() const override { return 0; }
    std::string serviceType() const override { return "Feeder route"; }
    bool opensIntegrationWindow() const override { return false; }
};

// PARTE 2: Pasajeros y calculo de la tarifa
// Passenger define el comportamiento comun; cada perfil adapta computeFare.
// fareOn es la entrada publica: llama al calculo del perfil y evita valores negativos.

class Passenger {
public:
    Passenger(std::string name) : name_(std::move(name)) {}
    virtual ~Passenger() = default;
    const std::string& name() const { return name_; }
    virtual std::string profile() const = 0;

    long long fareOn(const TransportMode& mode) const {
        return std::max(0LL, computeFare(mode));
    }

protected:
    virtual long long computeFare(const TransportMode& mode) const {
        return mode.baseFare();
    }

private:
    std::string name_;
};

// El frecuente conserva la tarifa base; no necesita cambiar computeFare.
class FrequentPassenger : public Passenger {
public:
    FrequentPassenger(std::string name) : Passenger(std::move(name)) {}
    std::string profile() const override { return "Civica frequent"; }
};

// El estudiante paga 1600 si el transporte lo permite; si no, paga la tarifa base.
class StudentPassenger : public Passenger {
public:
    StudentPassenger(std::string name) : Passenger(std::move(name)) {}
    std::string profile() const override { return "Municipal student"; }

protected:
    long long computeFare(const TransportMode& mode) const override {
        const long long base = Passenger::computeFare(mode);
        return mode.allowsStudentFlatFare() ? 1600 : base;
    }
};

// El adulto mayor descuenta 490 de la tarifa base.
class SeniorPassenger : public Passenger {
public:
    SeniorPassenger(std::string name) : Passenger(std::move(name)) {}
    std::string profile() const override { return "Senior citizen"; }

protected:
    long long computeFare(const TransportMode& mode) const override {
        return Passenger::computeFare(mode) - 490;
    }
};

// El perfil de discapacidad descuenta 1100 de la tarifa base.
class DisabilityPassenger : public Passenger {
public:
    DisabilityPassenger(std::string name) : Passenger(std::move(name)) {}
    std::string profile() const override { return "Person with disability"; }

protected:
    long long computeFare(const TransportMode& mode) const override {
        return Passenger::computeFare(mode) - 1100;
    }
};

// El pasajero ocasional suma 580 a la tarifa base.
class BearerPassenger : public Passenger {
public:
    BearerPassenger(std::string name) : Passenger(std::move(name)) {}
    std::string profile() const override { return "Bearer (occasional)"; }

protected:
    long long computeFare(const TransportMode& mode) const override {
        return Passenger::computeFare(mode) + 580;
    }
};

// PARTE 3: Datos del recorrido y red de transportes
// Leg guarda un viaje: minuto y transporte. ChargedLeg guarda el resultado del cobro.
// integrated distingue un transbordo gratis de un viaje con tarifa cero por otro motivo.
struct Leg {
    int minute;                   // minutes elapsed since the journey started
    const TransportMode* mode;    // non-owning observer pointer
};

struct ChargedLeg {
    int minute;
    std::string modeName;
    std::string serviceType;
    long long charge;
    bool integrated;              // true: free transfer inside the window
};

// Network es duena de los transportes: unique_ptr libera su memoria automaticamente.
// add transfiere la propiedad con std::move y devuelve una referencia al transporte.
// Los punteros de Leg solo consultan esos transportes; no son sus duenos.
class Network {
private:
    std::vector<std::unique_ptr<TransportMode>> modes_;

public:
    TransportMode& add(std::unique_ptr<TransportMode> mode) {
        modes_.push_back(std::move(mode));
        return *modes_.back();
    }

    const std::vector<std::unique_ptr<TransportMode>>& modes() const {
        return modes_;
    }

    void printInventory() const {
        std::for_each(modes_.begin(), modes_.end(),
            [](const std::unique_ptr<TransportMode>& mode) {
                std::cout << "  - " << mode->describe() << '\n';
            });
    }
};

// PARTE 4: Cobro del recorrido y transbordos
// Recibimos una copia de los viajes y la ordenamos por minuto con sort.
// La lambda indica el criterio de orden: primero el viaje con menor minuto.
std::vector<ChargedLeg> chargeItinerary(std::vector<Leg> legs,
                                       const Passenger& passenger) {
    std::sort(legs.begin(), legs.end(),
        [](const Leg& a, const Leg& b) { return a.minute < b.minute; });

    std::vector<ChargedLeg> charged;
    // Recordamos si hay ventana, cuando empezo y cuantos transbordos se usaron.
    bool windowOpen = false;
    long long windowStart = 0;
    int freeTransfers = 0;

    for (const Leg& leg : legs) {
        const TransportMode& mode = *leg.mode;
        // Primero calculamos la tarifa del perfil; luego revisamos si hay transbordo gratis.
        long long charge = passenger.fareOn(mode);
        bool integrated = false;

        // Los modos excluidos cobran sin alterar la ventana ni su contador.
        if (mode.allowsIntegration() && mode.opensIntegrationWindow()) {
            const long long elapsed = leg.minute - windowStart;
            // Se permiten hasta dos transbordos gratis dentro de los 90 minutos.
            if (windowOpen && elapsed <= 90 && freeTransfers < 2) {
                charge = 0;
                integrated = true;
                ++freeTransfers;
            } else {
                // Si no hay beneficio disponible, se cobra y comienza una nueva ventana.
                windowOpen = true;
                windowStart = leg.minute;
                freeTransfers = 0;
            }
        }

        // Guardamos el resultado de este viaje para mostrarlo y construir los reportes.
        charged.push_back({leg.minute, mode.name(), mode.serviceType(),
                           charge, integrated});
    }
    return charged;
}

// PARTE 5: Reportes con algoritmos STL y lambdas
// Las lambdas son funciones pequenas que indican que hacer o que condicion revisar.

// accumulate suma los cobros, comenzando en cero.
long long totalPaid(const std::vector<ChargedLeg>& charged) {
    return std::accumulate(charged.begin(), charged.end(), 0LL,
        [](long long total, const ChargedLeg& leg) {
            return total + leg.charge;
        });
}

// count_if cuenta los viajes marcados como integrados, no todos los cobros en cero.
long long freeTransferCount(const std::vector<ChargedLeg>& charged) {
    return std::count_if(charged.begin(), charged.end(),
        [](const ChargedLeg& leg) { return leg.integrated; });
}

// for_each recorre los viajes y un map acumula el dinero por tipo de servicio.
// [&totals] permite que la lambda modifique el mapa creado fuera de ella.
std::map<std::string, long long>
paidByServiceType(const std::vector<ChargedLeg>& charged) {
    std::map<std::string, long long> totals;
    std::for_each(charged.begin(), charged.end(),
        [&totals](const ChargedLeg& leg) {
            totals[leg.serviceType] += leg.charge;
        });
    return totals;
}

// Recibe una condicion (policy) y devuelve los nombres que la cumplen.
// La condicion llega como parametro: podemos cambiar el filtro sin cambiar la funcion.
std::vector<std::string> namesMatching(
    const Network& network,
    const std::function<bool(const TransportMode&)>& policy) {
    std::vector<std::string> names;
    std::for_each(network.modes().begin(), network.modes().end(),
        [&names, &policy](const std::unique_ptr<TransportMode>& mode) {
            if (policy(*mode)) {
                names.push_back(mode->name());
            }
        });
    return names;
}

// Une los nombres en un solo texto, separados por comas.
std::string joined(const std::vector<std::string>& names) {
    return std::accumulate(names.begin(), names.end(), std::string{},
        [](const std::string& result, const std::string& name) {
            return result.empty() ? name : result + ", " + name;
        });
}

// PARTE 6: Demostracion del profesor (main)
// 1. Crea la red, los transportes, el recorrido y los pasajeros con datos ya definidos.
// 2. Muestra el inventario y compara las tarifas de los perfiles.
// 3. Cobra el recorrido del frecuente y muestra cada viaje en orden.
// 4. Imprime totales, transbordos y cobros agrupados; find_if busca el primer viaje pagado.
// 5. Filtra transportes con mas de 8 estaciones y calcula totales de estudiante y mayor.
// No pide datos por teclado: ejecuta los ejemplos del profesor.
// Para explicar el resultado: frecuente 20640, estudiante 17800 y adulto mayor 19170.
int main() {
    std::cout << "============================================================\n";
    std::cout << "   INTEGRATED ITINERARY AND FARE ENGINE - Valle de Aburra\n";
    std::cout << "============================================================\n\n";

    Network network;
    TransportMode& lineA  = network.add(std::make_unique<MetroLine>("A", 21));
    TransportMode& lineB  = network.add(std::make_unique<MetroLine>("B", 7));
    TransportMode& tramTA = network.add(std::make_unique<TramLine>("T-A Ayacucho", 9));
    TransportMode& cableK = network.add(std::make_unique<CableLine>("K Santo Domingo", 4));
    TransportMode& arvi   = network.add(std::make_unique<TouristCable>("L Arvi", 2, 9180));
    TransportMode& feeder = network.add(std::make_unique<FeederRoute>("130 Robledo", 18));

    std::cout << ">> 1. NETWORK INVENTORY (polymorphic describe)\n";
    network.printInventory();
    std::cout << '\n';

    /* Itinerary deliberately given out of chronological order, so that the
       sort step is observable. */
    const std::vector<Leg> itinerary = {
        {45,  &cableK},
        {0,   &feeder},
        {12,  &lineA},
        {150, &lineB},
        {70,  &tramTA},
        {200, &arvi}
    };

    const FrequentPassenger frequent("Diego");
    const StudentPassenger  student("Valentina");
    const SeniorPassenger   senior("Dona Amparo");

    std::cout << ">> 2. FULL FARE PER PROFILE ON METRO LINE A\n";
    std::cout << "  " << frequent.profile() << " pays " << formatCOP(frequent.fareOn(lineA)) << '\n';
    std::cout << "  " << student.profile()  << " pays " << formatCOP(student.fareOn(lineA))  << '\n';
    std::cout << "  " << senior.profile()   << " pays " << formatCOP(senior.fareOn(lineA))   << '\n';
    std::cout << "  " << student.profile()  << " on the tourist cable pays "
              << formatCOP(student.fareOn(arvi)) << " (no flat fare there)\n\n";

    const std::vector<ChargedLeg> charged = chargeItinerary(itinerary, frequent);

    std::cout << ">> 3. CHARGED ITINERARY (frequent profile)\n";
    std::for_each(charged.begin(), charged.end(), [](const ChargedLeg& c) {
        std::cout << "  min " << c.minute << " | " << c.serviceType << " " << c.modeName
                  << " | " << formatCOP(c.charge)
                  << (c.integrated ? "  [free transfer]" : "") << '\n';
    });
    std::cout << '\n';

    std::cout << ">> 4. REPORTS BUILT WITH STL ALGORITHMS\n";
    std::cout << "  accumulate: total paid -> " << formatCOP(totalPaid(charged)) << '\n';
    std::cout << "  count_if: free transfers -> " << freeTransferCount(charged) << '\n';

    const auto byType = paidByServiceType(charged);
    std::for_each(byType.begin(), byType.end(),
        [](const std::pair<const std::string, long long>& entry) {
            std::cout << "  map: paid on " << entry.first << " -> "
                      << formatCOP(entry.second) << '\n';
        });

    const auto firstPaid = std::find_if(charged.begin(), charged.end(),
        [](const ChargedLeg& c) { return c.charge > 0; });
    std::cout << "  find_if: first leg that actually paid -> "
              << (firstPaid == charged.end() ? std::string("none") : firstPaid->modeName) << '\n';

    const std::function<bool(const TransportMode&)> bigModes =
        [](const TransportMode& m) { return m.stations() > 8; };
    std::cout << "  injected policy: modes with more than 8 stations -> "
              << joined(namesMatching(network, bigModes)) << '\n';

    std::cout << "  total for the student on the same itinerary -> "
              << formatCOP(totalPaid(chargeItinerary(itinerary, student))) << '\n';
    std::cout << "  total for the senior on the same itinerary -> "
              << formatCOP(totalPaid(chargeItinerary(itinerary, senior))) << '\n';

    std::cout << "\n============================================================\n";
    std::cout << "  End of demonstration.\n";
    std::cout << "============================================================\n";
    return 0;
}
