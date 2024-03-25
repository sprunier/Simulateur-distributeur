# Projet Simulateur de distributeur

Ce projet est une simulation d'un système bancaire. Il permet de gérer des transactions bancaires comme les dépôts et les retraits.

L'objectif de ce projet était pour moi de revoir/apprendre certaines notions de base de C++.

## Structure du projet

Le projet est structuré comme suit :
- `Simulation_utilisation.cpp` : Simule l'utilisation du système bancaire par plusieurs utilisateurs simultanément.
  - Multithreading: Le script simule des transaction simultanées pour plusieurs utilisateurs, il créé un thread pour chaque interaction avec `Distributeur`
  - Allocation dynamique: La mémoire consommée par les objets de la classe `Distributeur` est allouée dynamiquement. Ceci permet la bonne éxecution des threads, sans détruire les objets à chaque itération de la boucle for.
  - Génération aléatoire: Le type de transaction et le montant sont déterminés "aléatoirement"
 
- `Distributeur.cpp` : Simule un distributeur automatique qui interagit avec l'utilisateur.
  - Classe utilisée pour faire le lien entre le numero de carte/code de carte avec l'identifiant du client et créer une transaction `Depot`/`Retrait` en fonction d'un paramètre.

- `Transaction.cpp` : Contient les classes `Depot` et `Retrait` qui héritent de la classe `Transaction`. Ces classes permettent d'enregistrer les modifications des soldes clients et de sauvegarder les transactions pour en garder une trace.
  - Fonctions virtuelles: Les méthodes getTypeTransaction et executer sont surchargées pour remplacer les méthodes virtuelles de la classe de base.
  - Constructeur: Les classes Retrait et Depot ont un constructeur qui prend deux arguments et les passe à la classe de base Transaction via une liste d'initialisation de membre.
  - Gestion des exceptions: La méthode executer utilise un bloc try/catch pour gérer les exceptions.
    
- `DBLock.cpp` : Gère les accès concurrents à la base de données.
  - Initialise un mutex static qui sera commun à l'utilisation de toutes les classes ayant besoin d'accéder à des fichiers. Ceci dans le but d'éviter les interférences entre les threads lors de la lecture/écriture dans les bases de données.
  
- `json.hpp` : Fournit des fonctionnalités pour travailler avec des fichiers JSON. (Provient de la librairie de nlohmann [https://github.com/nlohmann/json]

On trouve aussi les fichiers JSON:
- `BankDB.json` : Contient les informations fictives des clients de la banque. Contient également un eneregistrement des transactions effectuées.
- `RandomUser.json` : Contient les informations de carte bancaire de clients, permet de simuler un client qui vient retirer/déposer de l'argent.

## Exécution du projet
Pour exécuter le projet, une version build en .exe est présente dans le dépôt: `Simulation_utilisation.exe`.
Sinon, il faudra build et exécuter le fichier `Simulation_utilisation.cpp`
