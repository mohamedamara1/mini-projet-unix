```markdown
# Instructions d'exécution

Voici un guide étape par étape pour compiler et exécuter les fichiers du serveur et du client en utilisant un Makefile.

## 1. Création du Makefile

```make
CC = gcc
CFLAGS = -Wall -Iinclude

SERVER_EXEC = serveurTCP_fork
CLIENT_EXEC = clientTCP_IHM

all: $(SERVER_EXEC) $(CLIENT_EXEC)

$(SERVER_EXEC): src/serveurTCP_fork.c
	$(CC) $(CFLAGS) $^ -o $@

$(CLIENT_EXEC): src/clientTCP_IHM.c
	$(CC) $(CFLAGS) $^ -o $@ `pkg-config --cflags --libs gtk+-3.0`

clean:
	rm -f $(SERVER_EXEC) $(CLIENT_EXEC)

.PHONY: all clean
```

## 2. Compilation avec le Makefile

Ouvrez un terminal et assurez-vous que le Makefile est dans le même répertoire que vos fichiers source. Ensuite, exécutez la commande suivante :

```bash
make
```

Cela va compiler le serveur (`serveurTCP_fork`) et le client (`clientTCP_IHM`).

## 3. Démarrage du Serveur

Dans le même terminal, exécutez le serveur avec la commande :

```bash
./serveurTCP_fork 12345
```

## 4. Démarrage des Clients

Ouvrez plusieurs terminaux ou utilisez plusieurs instances de votre terminal. Exécutez le client dans chaque terminal avec la commande :

```bash
./clientTCP_IHM 127.0.0.1 12345
```

## 5. Authentification des Clients

- Chaque client affiche une interface graphique avec des champs pour le nom d'utilisateur et le mot de passe.
- Chaque client saisit des informations d'identification et clique sur le bouton "Se connecter".

## 6. Interactions des Clients avec le Serveur

- Les clients peuvent sélectionner des services dans l'IHM.
- Les clients envoient des demandes de services au serveur.
- Le serveur répond aux demandes et envoie les résultats aux clients.
- Les résultats sont affichés dans l'IHM des clients.

## 7. Déconnexion des Clients

- Les clients peuvent choisir l'option "Fin" dans l'IHM pour se déconnecter du serveur.

## 8. Nettoyage avec le Makefile

Lorsque vous avez terminé, vous pouvez nettoyer les fichiers exécutables avec la commande :

```bash
make clean
```

Cela supprimera les fichiers exécutables générés (`serveurTCP_fork` et `clientTCP_IHM`).
