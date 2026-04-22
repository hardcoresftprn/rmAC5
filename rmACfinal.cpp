#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <iomanip>

using namespace std;

class Cont {
private:
    int id;
    string nume;
    float sold;
    vector<string> tranzactii;
public:
    Cont(int id, string nume, float sold) : id(id), nume(nume), sold(sold) {}

    void depuneBani(float suma) {
        if (suma > 0) {
            sold += suma;
            stringstream ss;
            ss << fixed << setprecision(2) << "Depunere: +" << suma << " RON. Sold: " << sold << " RON";
            tranzactii.push_back(ss.str());
        }
    }

    bool retrageBani(float suma) {
        if (suma > 0 && sold >= suma) {
            sold -= suma;
            stringstream ss;
            ss << fixed << setprecision(2) << "Retragere: -" << suma << " RON. Sold: " << sold << " RON";
            tranzactii.push_back(ss.str());
            return true;
        }
        return false;
    }

    float verificaSold() const {
        return sold;
    }

    bool trimiteBani(Cont* destinatie, float suma) {
        if (destinatie == nullptr || destinatie == this) return false;
        if (suma > 0 && sold >= suma) {
            sold -= suma;
            destinatie->sold += suma;

            stringstream ss1;
            ss1 << fixed << setprecision(2) << "Transfer catre ID " << destinatie->id << ": -" << suma << " RON. Sold: " << sold << " RON";
            tranzactii.push_back(ss1.str());

            stringstream ss2;
            ss2 << fixed << setprecision(2) << "Transfer de la ID " << this->id << ": +" << suma << " RON. Sold: " << destinatie->sold << " RON";
            destinatie->tranzactii.push_back(ss2.str());
            return true;
        }
        return false;
    }

    void vizualizeazaTranzactii() const {
        cout << "\n--- Istoric tranzactii ---\n";
        if (tranzactii.empty()) {
            cout << "Nu exista tranzactii.\n";
        }
        else {
            for (const auto& t : tranzactii) {
                cout << t << "\n";
            }
        }
        cout << "-------------------------\n";
    }

    int getId() const { return id; }
    string getNume() const { return nume; }
    float getSold() const { return sold; }
    vector<string> getTranzactii() const { return tranzactii; }

    void setNume(string n) { nume = n; }
    void setSold(float s) { sold = s; }
    void setTranzactii(vector<string> t) { tranzactii = t; }
    void adaugaTranzactie(string t) { tranzactii.push_back(t); }

    string serializeaza() const {
        stringstream ss;
        ss << id << "|" << nume << "|" << fixed << setprecision(2) << sold << "|";
        for (size_t i = 0; i < tranzactii.size(); i++) {
            ss << tranzactii[i];
            if (i != tranzactii.size() - 1) ss << ";";
        }
        return ss.str();
    }

    static Cont* deserializeaza(string linie) {
        stringstream ss(linie);
        string sid, snume, ssold, stranz;
        getline(ss, sid, '|');
        getline(ss, snume, '|');
        getline(ss, ssold, '|');
        getline(ss, stranz);

        int id = stoi(sid);
        float sold = stof(ssold);
        Cont* c = new Cont(id, snume, sold);

        if (!stranz.empty()) {
            stringstream st(stranz);
            string tr;
            while (getline(st, tr, ';')) {
                if (!tr.empty()) c->tranzactii.push_back(tr);
            }
        }
        return c;
    }
};

vector<Cont*> incarcaConturi(string fisier) {
    vector<Cont*> conturi;
    ifstream f(fisier);
    if (!f.is_open()) return conturi;
    string linie;
    while (getline(f, linie)) {
        if (!linie.empty()) {
            conturi.push_back(Cont::deserializeaza(linie));
        }
    }
    f.close();
    return conturi;
}

void salveazaConturi(string fisier, vector<Cont*> conturi) {
    ofstream f(fisier);
    for (auto c : conturi) {
        f << c->serializeaza() << "\n";
    }
    f.close();
}

void elibereazaMemorie(vector<Cont*>& conturi) {
    for (auto c : conturi) {
        delete c;
    }
    conturi.clear();
}

Cont* cautaCont(vector<Cont*> conturi, int id) {
    for (auto c : conturi) {
        if (c->getId() == id) return c;
    }
    return nullptr;
}

bool idExista(vector<Cont*> conturi, int id) {
    return cautaCont(conturi, id) != nullptr;
}

void afiseazaMeniu() {
    cout << "\n====== MENIU ATM ======\n";
    cout << "1. Inregistrare utilizator\n";
    cout << "2. Autentificare utilizator\n";
    cout << "3. Depunere bani\n";
    cout << "4. Retragere bani\n";
    cout << "5. Verificare sold\n";
    cout << "6. Transfer bani\n";
    cout << "7. Vizualizare tranzactii\n";
    cout << "8. Blocare cont\n";
    cout << "0. Iesire\n";
    cout << "=======================\n";
    cout << "Optiune: ";
}

int main() {
    const string FISIER = "conturi.txt";
    vector<Cont*> conturi = incarcaConturi(FISIER);
    Cont* contLogat = nullptr;
    int optiune;

    do {
        afiseazaMeniu();
        cin >> optiune;

        if (optiune == 1) {
            int id;
            string nume;
            cout << "ID unic: ";
            cin >> id;
            cin.ignore();
            cout << "Nume: ";
            getline(cin, nume);
            if (idExista(conturi, id)) {
                cout << "Eroare: ID-ul exista deja!\n";
            }
            else {
                conturi.push_back(new Cont(id, nume, 0.0f));
                salveazaConturi(FISIER, conturi);
                cout << "Cont creat cu succes!\n";
            }
        }
        else if (optiune == 2) {
            int id;
            cout << "ID: ";
            cin >> id;
            Cont* c = cautaCont(conturi, id);
            if (c) {
                contLogat = c;
                cout << "Autentificare reusita! Bine ai venit, " << c->getNume() << "!\n";
            }
            else {
                cout << "Eroare: ID invalid!\n";
            }
        }
        else if (optiune == 3) {
            if (!contLogat) { cout << "Eroare: Nu sunteti autentificat!\n"; continue; }
            float suma;
            cout << "Suma de depus: ";
            cin >> suma;
            if (suma > 0) {
                contLogat->depuneBani(suma);
                salveazaConturi(FISIER, conturi);
                cout << "Depunere reusita!\n";
            }
            else {
                cout << "Suma invalida!\n";
            }
        }
        else if (optiune == 4) {
            if (!contLogat) { cout << "Eroare: Nu sunteti autentificat!\n"; continue; }
            float suma;
            cout << "Suma de retras: ";
            cin >> suma;
            if (contLogat->retrageBani(suma)) {
                salveazaConturi(FISIER, conturi);
                cout << "Retragere reusita!\n";
            }
            else {
                cout << "Eroare: Fonduri insuficiente sau suma invalida!\n";
            }
        }
        else if (optiune == 5) {
            if (!contLogat) { cout << "Eroare: Nu sunteti autentificat!\n"; continue; }
            cout << fixed << setprecision(2) << "Sold curent: " << contLogat->verificaSold() << " RON\n";
        }
        else if (optiune == 6) {
            if (!contLogat) { cout << "Eroare: Nu sunteti autentificat!\n"; continue; }
            int idDest;
            float suma;
            cout << "ID destinatar: ";
            cin >> idDest;
            cout << "Suma: ";
            cin >> suma;
            Cont* dest = cautaCont(conturi, idDest);
            if (!dest) {
                cout << "Eroare: ID destinatar invalid!\n";
            }
            else if (dest == contLogat) {
                cout << "Eroare: Nu puteti transfera catre propriul cont!\n";
            }
            else if (contLogat->trimiteBani(dest, suma)) {
                salveazaConturi(FISIER, conturi);
                cout << "Transfer reusit!\n";
            }
            else {
                cout << "Eroare: Fonduri insuficiente sau suma invalida!\n";
            }
        }
        else if (optiune == 7) {
            if (!contLogat) { cout << "Eroare: Nu sunteti autentificat!\n"; continue; }
            contLogat->vizualizeazaTranzactii();
        }
        else if (optiune == 8) {
            if (!contLogat) { cout << "Eroare: Nu sunteti autentificat!\n"; continue; }
            char conf;
            cout << "Sigur doriti sa blocati contul? (d/n): ";
            cin >> conf;
            if (conf == 'd' || conf == 'D') {
                for (auto it = conturi.begin(); it != conturi.end(); ++it) {
                    if (*it == contLogat) {
                        delete* it;
                        conturi.erase(it);
                        break;
                    }
                }
                salveazaConturi(FISIER, conturi);
                contLogat = nullptr;
                cout << "Cont blocat si sters cu succes!\n";
            }
        }
        else if (optiune == 0) {
            cout << "La revedere!\n";
        }
        else {
            cout << "Optiune invalida!\n";
        }
    } while (optiune != 0);

    elibereazaMemorie(conturi);
    return 0;
}