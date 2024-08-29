# Echo Client-Server Application

Ce projet est une application client-serveur simple qui permet le transfert de fichiers entre un serveur et un client. Le client peut demander le téléchargement de fichiers depuis le serveur et les sauvegarder localement. Cette application a été développée en C et utilise des sockets pour la communication réseau.

## Fonctionnalités

- Connexion à un serveur distant via TCP.
- Téléchargement de fichiers depuis le serveur.
- Gestion des erreurs de base telles que fichier non trouvé ou permission refusée.
- Affichage d'une barre de progression pendant le téléchargement des fichiers.
- Possibilité de ralentir les opérations d'écriture pour des fins de test.

## Dépendances

Le projet dépend de la bibliothèque `csapp.c` qui fournit des wrappers pour diverses opérations système et réseau, facilitant ainsi la gestion des sockets, des threads, et des opérations d'entrée/sortie.

## Compilation

Pour compiler le projet, seulement depuis le /server_side, utilisez la commande suivante :
Make

## Utilisation

Pour démarrer le serveur, exécuter depuis /server_side :
./echoserver 

Pour connecter un client au serveur, utiliser depuis le client_side :
./echoclient <hostname>
Remplacez <hostname> par l'adresse du serveur, ou par localhost sur le même ordi.

Une fois connecté, vous pouvez utiliser la commande get <filename> pour télécharger un fichier depuis le serveur, et bye pour fermer la connexion.


