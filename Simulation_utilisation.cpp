#include <random>
#include <thread>
#include <vector>
#include <fstream>
#include "json.hpp"
#include "Distributeur.cpp"
#include "DBLock.cpp"

std::string random_operation() {
    //Simule une opération aléatoire de retrait ou de dépôt
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 1);
    return dis(gen) ? "Retrait" : "Depot";
}

float random_amount() {
    //Simule un montant aléatoire entre 1 et 1000
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(1.0, 1000.0);
    float random_float = dis(gen);
    float rounded_float = roundf(random_float * 100) / 100;
    return rounded_float;
}

int main() {
    std::ifstream fichier_entree("RandomUser.json");
    nlohmann::json donnees_json;
    fichier_entree >> donnees_json;
    fichier_entree.close();
    
    std::vector<std::thread> threads;
    std::vector<Distributeur*> distribs;

    for (const auto info : donnees_json["Utilisateurs"]) {
        std::string operation = random_operation();
        float amount = random_amount();
        std::string numero_carte = info["numero_carte"];
        int cvv = info["cvv"];

        Distributeur* distrib = new Distributeur(numero_carte, cvv, amount, operation);
        distribs.push_back(distrib);
        threads.push_back(std::thread(&Distributeur::LancerTransaction, distrib));
    }
    for (auto& t : threads) {
        t.join();
    }
    for (auto* distrib : distribs) {
        delete distrib;
    }
    std::cout << "Toutes les transactions ont été effectuées avec succès" << std::endl;
    std::cout<<"Appuyez sur Entrée pour quitter"<<std::endl;
    std::cin.get();
    return 0;
}