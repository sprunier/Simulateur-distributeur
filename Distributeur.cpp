#include "Transaction.cpp"
#include <iostream>
#include"DBLock.h"

class Distributeur {
    public:
        Distributeur(const std::string& numeroCarte, const int& cvv, const float& montant, const std::string& typeTransaction):numeroCarte(numeroCarte),cvv(cvv),montant(montant),typeTransaction(typeTransaction) {}

        void LancerTransaction() {
            if (typeTransaction == "Retrait") {   
                Retrait* transac = new Retrait(montant, getIdClient());
                transac->executer();
                delete transac;
            }
            else if (typeTransaction == "Depot") {
                Depot* transac = new Depot(montant, getIdClient());
                transac->executer();
                delete transac;
            }
            else {
                throw std::invalid_argument("Type de transaction invalide");
            }
        }

        
    private:
        std::string numeroCarte;
        int cvv;
        double montant;
        std::string typeTransaction;

        int getIdClient() const {
            // Lire le fichier JSON existant
            //TODO: Utiliser une approche streaming pour lire le fichier JSON -> moins de mémoire
            std::lock_guard<std::mutex> lock(DBLock::mtx);
            std::ifstream fichier_entree("BankDB.json");
            nlohmann::json donnees_json;
            fichier_entree >> donnees_json;
            fichier_entree.close();
            // Trouver le client avec le numéro de carte et le cvv
            for (auto& client : donnees_json["clients"]) {
                if (client["numero_carte"] == numeroCarte && client["cvv"] == cvv) {
                    return client["id"];
                }
            }
            throw std::invalid_argument("Numéro de carte ou cvv invalide");
        }
    };