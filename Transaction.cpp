#include<iostream>
#include"json.hpp"
#include<string>
#include<chrono>
#include<sstream>
#include<iomanip>
#include<fstream>
#include<stdexcept>
#include"DBLock.h"

class Transaction{
    
    public:
        //Constructeur de la classe Transaction
        Transaction(const float& montantentry,const int& id_cliententry):montant(montantentry),id_client(id_cliententry),montant_centime(round(montant * 100)){}
        ~Transaction(){}
        virtual void executer() = 0;
         
    protected:
        int id_client;
        float montant;
        int montant_centime;


        virtual std::string getTypeTransaction() const = 0;
    
        std::string getTemps() const{
            //Obtient le temps actuel en format ISO 8601
            auto maintenant=std::chrono::system_clock::now();
            std::time_t temps_maintenant= std::chrono::system_clock::to_time_t(maintenant);
            std::tm *temps_maintenant_tm=std::gmtime(&temps_maintenant);
            std::stringstream ss;
            ss << std::put_time(temps_maintenant_tm, "%FT%TZ");
            return ss.str();
        }

        std::string getIdTransaction() const {
            //Crée un identifiant unique pour la transaction de type TR+temps+type+id_client
            std::string id = "TR"+getTemps()+getTypeTransaction()+std::to_string(id_client);            
            return id;
        }

        void sauvegarderTransaction() const{
            nlohmann::json transaction;
            transaction["id_transaction"] = getIdTransaction();
            transaction["id_utilisateur"] = id_client;
            transaction["horodatage"] = getTemps();
            transaction["montant"] = montant_centime;

            {
                std::lock_guard<std::mutex> lock(DBLock::mtx);
                // Lire le fichier JSON existant
                //TODO: Utiliser une approche streaming pour lire le fichier JSON -> moins de mémoire
                std::ifstream fichier_entree("BankDB.json");
                nlohmann::json donnees_json;
                fichier_entree >> donnees_json;
                fichier_entree.close();

                // Ajouter la nouvelle transaction à la liste des transactions
                donnees_json["transactions"].push_back(transaction);

                // Réécrire le fichier JSON avec les nouvelles données
                std::ofstream fichier_sortie("BankDB.json");
                fichier_sortie << donnees_json.dump(4); // 4 est le nombre d'espaces pour l'indentation
                fichier_sortie.close();
            }
        }

        std::map<std::string,std::string> actualiserSolde() const {
            {
                //Verrouille la base de données pour éviter les accès concurrents
                std::lock_guard<std::mutex> lock(DBLock::mtx);
                
                // Lit le fichier JSON existant
                std::ifstream fichier_entree("BankDB.json");
                nlohmann::json donnees_json;
                fichier_entree>>donnees_json;
                fichier_entree.close();
            
                for (auto& client : donnees_json["clients"]){
                    if (client["id"]==id_client){

                        //Obtient la valeur du solde du client
                        int Solde = client["solde_centime"].get<int>();

                        //Si il s'agit d'un dépôt
                        if (getTypeTransaction()=="DP"){
                            Solde += montant_centime;
                            client["solde_centime"] = Solde;
                        }

                        //Si il s'agit d'un retrait
                        else if (getTypeTransaction()=="RT"){
                            //Vérifie si le client a un découvert autorisé
                            if(client["decouvert_autorise"]==false && Solde<montant_centime){
                                return std::map<std::string,std::string>{{"nom",client["nom"]},{"nouveausolde","Solde insuffisant"}};
                            }
                            else{
                                Solde -= montant_centime;
                                client["solde_centime"] = Solde;
                            }
                        }
                        else{
                            throw std::runtime_error("Impossbile d'actualiser le solde d'une transaction non definie.");
                        }

                        //Crée un dictionnaire pour stocker les informations du client
                        std::map<std::string,std::string> infoClient;
                        //Ajoute le nom du client au dictionnaire à retourner
                        infoClient["nom"]=client["nom"];
                        //Ajoute le nouveau solde au dictionnaire à retourner
                        std::stringstream ss;
                        float nouveauSolde = static_cast<float>(Solde)/100;
                        ss<<nouveauSolde;
                        infoClient["nouveausolde"]=ss.str();
                        
                        //Réécrit le fichier JSON avec les nouvelles données
                        std::ofstream fichier_sortie("BankDB.json");
                        fichier_sortie << donnees_json.dump(4); // 4 est le nombre d'espaces pour l'indentation
                        fichier_sortie.close();
                        return infoClient;
                    }
                }
            }
            //Si le client n'est pas trouvé
            throw std::runtime_error("Client non trouve dans la base de donnees.");
        }
};

class Depot: public Transaction{
    public:    
        Depot(float montant, int id_client) : Transaction(montant, id_client) {}
        std::string getTypeTransaction() const override {
            return "DP";
        }

        void executer(){
            try{
                std::map<std::string,std::string> Infos = actualiserSolde();
                sauvegarderTransaction();          
                std::cout<<"Merci "<<Infos["nom"]<<", depot effectue avec succes!"<<std::endl;
                std::cout<<"Votre nouveau solde est : "<<Infos["nouveausolde"]<<" euros"<<std::endl<<std::endl;
            }
            catch(...){
                std::cout<<"Erreur lors de l'execution du depot."<<std::endl;
            }
            
        }
};

class Retrait: public Transaction{
    public:
        Retrait(float montant, int id_client) : Transaction(montant, id_client) {}
        std::string getTypeTransaction() const override {
            return "RT";
        }
    
        void executer(){
            try{
                std::map<std::string,std::string> Infos = actualiserSolde(); 
                //Gère le cas où le solde est insuffisant 
                if (Infos["nouveausolde"]=="Solde insuffisant"){
                    std::cout<<"Desole "<<Infos["nom"]<<", votre solde est insuffisant pour effectuer ce retrait et vous n'etes pas autorise a etre a decouvert."<<std::endl<<std::endl;
                    return;
                }
                sauvegarderTransaction();         
                std::cout<<"Merci "<<Infos["nom"]<<", retrait effectue avec succes!"<<std::endl;
                std::cout<<"Votre nouveau solde est : "<<Infos["nouveausolde"]<<" euros"<<std::endl<<std::endl;
            }
            catch(...){
                std::cout<<"Erreur lors de l'execution du retrait."<<std::endl;
            }
        }
};
