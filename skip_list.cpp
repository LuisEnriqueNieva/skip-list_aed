#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <cstdlib>
using namespace std;

const int MAX_NIVEL = 16; //lo podemos cambiar

template <typename T>
struct NodoSkip {
    T dato;
    vector<NodoSkip*> siguiente;  // siguiente[i] = siguiente en el nivel i+1 (i=0: lista completa)
    vector<NodoSkip*> anterior;   // anterior[i]  = anterior en el nivel i+1
    NodoSkip(T d, int nivel)
        : dato(d), siguiente(nivel, nullptr), anterior(nivel, nullptr) {}
};

template <typename T>
struct SkipList {
    NodoSkip<T>* cab;
    int nivelActual = 1;
    int n = 0;

    // --- para el video y el informe ---
    bool verbose = false;       // true: cada operación escribe sus pasos en pantalla
    int nivelForzado = 0;       // 0 = moneda normal; k>0 = todo nodo nuevo tiene altura k
    long long avances = 0;      // cuántas veces se avanza de nodo (mide el costo de buscar)

    SkipList() { cab = new NodoSkip<T>(T(), MAX_NIVEL); }
    SkipList(const SkipList&) = delete;
    SkipList& operator=(const SkipList&) = delete;

    bool empty() const { return n == 0; }
    int size() const   { return n; }

    // nombre de un nodo para el registro: "CAB", "NULL" o su dato
    string nom(NodoSkip<T>* p) const {
        if (p == nullptr) return "NULL";
        if (p == cab) return "CAB";
        ostringstream os; os << p->dato;
        return os.str();
    }

    int nivelAleatorio() {
        if (nivelForzado > 0) return min(nivelForzado, MAX_NIVEL);
        int nivel = 1;
        while (nivel < MAX_NIVEL) {
            bool cara = (rand() % 2 == 0);
            if (verbose) cout << "MONEDA " << (cara ? "cara" : "cruz") << "\n";
            if (!cara) break;
            nivel++;
        }
        return nivel;
    }

    // Baja desde el nivel más alto hasta el 1. En update[i] deja el predecesor de x
    // en el nivel i. Devuelve el predecesor en el nivel 0.
    NodoSkip<T>* descender(T x, vector<NodoSkip<T>*>& update) {
        NodoSkip<T>* p = cab;
        for (int i = nivelActual - 1; i >= 0; i--) {
            while (p->siguiente[i] != nullptr && p->siguiente[i]->dato < x) {
                NodoSkip<T>* q = p->siguiente[i];
                if (verbose) cout << "AVANZA " << i + 1 << " " << nom(p) << " " << nom(q) << "\n";
                p = q;
                avances++;
            }
            update[i] = p;
            if (verbose) cout << "PARA " << i + 1 << " " << nom(p) << "\n";
        }
        return p;
    }

    NodoSkip<T>* buscar(T x) {
        if (verbose) cout << "BUSCAR " << x << "\n";
        vector<NodoSkip<T>*> update(MAX_NIVEL, cab);
        NodoSkip<T>* p = descender(x, update)->siguiente[0];
        bool ok = (p != nullptr && p->dato == x);
        if (verbose) cout << (ok ? "ENCONTRADO " : "NO_ENCONTRADO ") << x << "\n";
        return ok ? p : nullptr;
    }

    bool insertar(T x) {
        if (verbose) cout << "INSERTAR " << x << "\n";
        vector<NodoSkip<T>*> update(MAX_NIVEL, cab);
        NodoSkip<T>* p = descender(x, update);
        NodoSkip<T>* sig = p->siguiente[0];
        if (sig != nullptr && sig->dato == x) {
            if (verbose) cout << "DUPLICADO " << x << "\n";
            return false;
        }
        int nivel = nivelAleatorio();
        if (verbose) cout << "NIVEL " << nivel << "\n";
        if (nivel > nivelActual) {
            nivelActual = nivel;
            if (verbose) cout << "SUBE_TECHO " << nivelActual << "\n";
        }
        NodoSkip<T>* nuevo = new NodoSkip<T>(x, nivel);
        for (int i = 0; i < nivel; i++) {
            NodoSkip<T>* s = update[i]->siguiente[i];
            nuevo->anterior[i]  = update[i];
            nuevo->siguiente[i] = s;
            if (s != nullptr) s->anterior[i] = nuevo;
            update[i]->siguiente[i] = nuevo;
            if (verbose) cout << "ENLAZA " << i + 1 << " " << nom(update[i]) << " " << x << " " << nom(s) << "\n";
        }
        n++;
        return true;
    }

    void eliminarNodo(NodoSkip<T>* p) {
        T x = p->dato;
        for (int i = 0; i < (int)p->siguiente.size(); i++) {
            p->anterior[i]->siguiente[i] = p->siguiente[i];
            if (p->siguiente[i] != nullptr)
                p->siguiente[i]->anterior[i] = p->anterior[i];
            if (verbose) cout << "DESENLAZA " << i + 1 << " " << nom(p->anterior[i]) << " " << x << " " << nom(p->siguiente[i]) << "\n";
        }
        delete p;
        n--;
        while (nivelActual > 1 && cab->siguiente[nivelActual - 1] == nullptr) {
            nivelActual--;
            if (verbose) cout << "BAJA_TECHO " << nivelActual << "\n";
        }
    }

    bool eliminar(T x) {
        if (verbose) cout << "ELIMINAR " << x << "\n";
        vector<NodoSkip<T>*> update(MAX_NIVEL, cab);
        NodoSkip<T>* p = descender(x, update)->siguiente[0];
        if (p == nullptr || !(p->dato == x)) {
            if (verbose) cout << "NO_ENCONTRADO " << x << "\n";
            return false;
        }
        if (verbose) cout << "ENCONTRADO " << x << "\n";
        eliminarNodo(p);
        return true;
    }

    void imprimir() const {
        for (NodoSkip<T>* p = cab->siguiente[0]; p != nullptr; p = p->siguiente[0])
            cout << p->dato << " ";
        cout << "\n";
    }

    void imprimirInverso() const {
        NodoSkip<T>* p = cab;
        for (int i = nivelActual - 1; i >= 0; i--)
            while (p->siguiente[i] != nullptr) p = p->siguiente[i];
        for (; p != cab; p = p->anterior[0])
            cout << p->dato << " ";
        cout << "\n";
    }

    // foto de la estructura: un renglón por nivel, de arriba hacia abajo
    void mostrarEstado() const {
        for (int i = nivelActual - 1; i >= 0; i--) {
            cout << "ESTADO " << i + 1 << ":";
            for (NodoSkip<T>* p = cab->siguiente[i]; p != nullptr; p = p->siguiente[i])
                cout << " " << p->dato;
            cout << "\n";
        }
    }

    ~SkipList() {
        NodoSkip<T>* p = cab;
        while (p != nullptr) {
            NodoSkip<T>* sig = p->siguiente[0];
            delete p;
            p = sig;
        }
    }
};

int main() {
    srand(42);
    SkipList<int> s;
    s.verbose = true;

    s.buscar(5);                       // caso borde: lista vacía
    for (int x : {3, 6, 9, 12, 19}) s.insertar(x);
    s.mostrarEstado();
    s.buscar(12);
    s.insertar(7);
    s.insertar(6);                     // caso borde: duplicado
    s.eliminar(9);
    s.eliminar(100);                   // caso borde: no existe
    s.mostrarEstado();

    // ---- complejidad: avances promedio por búsqueda ----
    for (int N : {100, 1000, 10000, 100000}) {
        SkipList<int> t;               // verbose apagado
        for (int i = 0; i < N; i++) t.insertar(i);
        t.avances = 0;
        int consultas = 10000;
        for (int k = 0; k < consultas; k++) t.buscar(rand() % N);
        cout << "N=" << N << "  avances promedio=" << (double)t.avances / consultas << "\n";
    }

    // ---- peor caso: todos los nodos de altura 1 ----
    SkipList<int> peor;
    peor.nivelForzado = 1;
    for (int i = 0; i < 1000; i++) peor.insertar(i);
    peor.avances = 0;
    peor.buscar(999);
    cout << "peor caso N=1000: avances=" << peor.avances << "\n";
}