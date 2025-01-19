#include "klient_interface.h"
#include <locale.h>

void klient_interface_init(klient_interface *this) {
    setlocale(LC_ALL, "sk_SK.utf8");
    bdd_klient_init(&this->klient_);
    this->connected_ = false;
}

void klient_interface_destroy(klient_interface *this) {
    bdd_klient_destroy(&this->klient_);
    this->connected_ = false;
}

void klient_interface_start_interface(klient_interface *this) {
    int answer = -1;
    do {
        klient_interface_print_menu();

        if (!klient_interface_get_int(this, &answer)) {
            continue;
        }

        if (answer == 0) { return; }
        switch (answer) {
            case 1:
                klient_interface_connect(this);
                break;
            case 2:
                klient_interface_disconnect(this);
                break;
            case 3:
                klient_interface_test_connection(this);
                break;
            case 4:
                klient_interface_run(this);
                break;
            default:
                printf("Zadali ste neplatnú možnosť, zadajte znovu.\n");
                break;
        }
    } while (true);
}

void klient_interface_print_menu(void) {
    printf("\n========================\n");
    printf("Konfigurácia BDD Klienta\n");
    printf("========================\n");
    printf("1 - Pripojiť sa k serveru\n");
    printf("2 - Odpojiť sa od servera\n");
    printf("3 - Otestovať spojenie\n");
    printf("4 - Spustiť inštrukcie\n");
    printf("0 - Ukončiť\n");
    printf("Vyberte možnosť: ");
}

void klient_interface_connect(klient_interface *this) {
    char server_address[16] = "127.0.0.1";
    int port = 8080;
    int answer = -1;
    do {
        printf("\n============================\n");
        printf("Pripojenie klienta na server\n");
        printf("============================\n");
        printf("1 - Zadať IPv4 adresu (nastavená je %s)\n", server_address);
        printf("2 - Zadať port (nastavený je %d)\n", port);
        printf("3 - Pokúsiť sa pripojiť\n");
        printf("0 - Späť\n");
        printf("Vyberte možnosť: ");
        if (!klient_interface_get_int(this, &answer)) {
            continue;
        }
        if (answer == 0) { return; }
        switch (answer) {
            case 1:
                printf("Zadajte novú IP adresu: ");
                scanf("%s", server_address);
                break;
            case 2: {
                printf("Zadajte nový port: ");
                if (!klient_interface_get_int(this, &port)) {
                    port = 8080;
                }
                break;
            }
            case 3: {
                if (this->connected_) { printf("Klient už je pripojený k serveru!\n"); break; }
                printf("Pripája sa na socket %s:%d\n", server_address, port);
                this->connected_ = bdd_klient_connect(&this->klient_, server_address, port);
                printf("Spojenie bolo %súspešné!\n", this->connected_ ? "" : "ne");
                break;
            }
            default:
                printf("Neplatná hodnota, zadajte znovu.\n");
                break;
        }
    } while (!this->connected_);
}

void klient_interface_disconnect(klient_interface *this) {
    if (this->connected_) {
        bdd_klient_disconnect(&this->klient_);
        this->connected_ = false;
        printf("Klient sa od servera odpojil.\n");
    } else {
        printf("Nie ste pripojený k žiadnemu serveru!\n");
    }
}

void klient_interface_run(klient_interface *this) {
    klient_interface_test_connection(this);

    if (!this->connected_) {
        return;
    }

    printf("Získavanie informácií od servera...\n");
    if (bdd_klient_receive_info(&this->klient_)) {
        printf("Klient vykonáva výpočet...\n");
        bdd_klient_execute_instructions(&this->klient_, bdd_klient_merge_modules);
        printf("Klient svoju časť dokončil. Odpája sa od servera.\n");
    } else {
        printf("Tento klient sa nebude používať, server ukončuje spojenie.\n");
    }

    bdd_klient_clear_klient(&this->klient_);
    bdd_klient_disconnect(&this->klient_);
    this->connected_ = false;
}

void klient_interface_test_connection(klient_interface *this) {
    if (!this->connected_) {
        printf("Nie ste pripojený k serveru!");
        return;
    }

    if (bdd_klient_test_connection(&this->klient_)) {
        printf("Spojenie stále funkčné!\n");
        return;
    }

    bdd_klient_disconnect(&this->klient_);
    this->connected_ = false;
    printf("Spojenie nefunkčné, klient uzatvára socket.\n");
}

bool klient_interface_get_int(klient_interface *this, int *answer) {
    if (scanf("%d", answer) != 1) {
        printf("Zadali ste neplatnú hodnotu! Prosím, zadajte celé číslo.\n");
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        *answer = -1;
        return false;
    }
    return true;
}
