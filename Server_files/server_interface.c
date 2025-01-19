#include "server_interface.h"
#include <locale.h>
#include <stdio.h>
#include <unistd.h>
#include "module_manager.h"
#include "server_utils.h"

void server_interface_init(server_interface* this) {
    setlocale(LC_ALL, "sk_SK.utf8");
    bdd_server_init(&this->server_);
    this->binded_ = false;
}

void server_interface_destroy(server_interface* this) {
    bdd_server_destroy(&this->server_);
    this->binded_ = false;
}

_Bool server_interface_get_int(server_interface* this, int* answer) {
    if (scanf("%d", answer) != 1) {
        printf("Zadali ste neplatnú hodnotu! Prosím, zadajte celé číslo.\n");
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        *answer = -1;
        return false;
    }
    return true;
}

void server_interface_start_interface(server_interface* this) {
    int answer = -1;
    do {
        server_interface_print_menu();

        if (!server_interface_get_int(this, &answer)) {
            continue;
        }

        if (answer == 0) { return; }
        switch (answer) {
            case 1:
                server_interface_bind(this);
                break;
            case 2:
                server_interface_standby(this);
                break;
            case 3:
                server_interface_test_connections(this);
                break;
            case 4:
                server_interface_run(this);
                break;
            default:
                printf("Zadali ste neplatnú možnosť, zadajte znovu.\n");
                break;
        }
    } while (true);
}

void server_interface_print_menu(void) {
    printf("\n========================\n");
    printf("Konfigurácia BDD Servera\n");
    printf("========================\n");
    printf("1 - Bindovať server na IP a port\n");
    printf("2 - Pripojiť klientov\n");
    printf("3 - Otestovať spojenia\n");
    printf("4 - Spustiť hlavný program\n");
    printf("0 - Ukončiť\n");
    printf("Vyberte možnosť: ");
}

void server_interface_bind(server_interface* this) {
    char server_address[16] = "any";
    int port = 8080;
    int answer = -1;
    do {
        printf("\n======================\n");
        printf("Bindovanie BDD Servera\n");
        printf("======================\n");
        printf("1 - Zadať IPv4 adresu (nastavená je %s)\n", server_address);
        printf("2 - Zadať port (nastavený je %d)\n", port);
        printf("3 - Pokúsiť sa bindovať\n");
        printf("0 - Späť\n");
        printf("Vyberte možnosť: ");

        if (!server_interface_get_int(this, &answer)) {
            continue;
        }

        if (answer == 0) { return; }
        switch (answer) {
            case 1:
                printf("Zadajte novú IP adresu (pre použitie všetkých dostupných rozhraní napíšte \"any\"):\n");
                scanf("%s", server_address);
                break;
            case 2: {
                printf("Zadajte nový port: ");
                if (!server_interface_get_int(this, &port)) {
                    port = 8080;
                }
                break;
            }
            case 3:
                if (this->binded_) { printf("Server už je nabindovaný!\n"); break; }
                if (bdd_server_bind_server(&this->server_, port, server_address)) {
                    printf("Server bol úspešne bindovaný na %s:%d.\n", server_address, port);
                    this->binded_ = true;
                } else {
                    printf("Bindovanie zlyhalo, skúste znova.\n");
                }
                break;
            default:
                printf("Neplatná hodnota, zadajte znovu.\n");
                break;
        }
    } while (!this->binded_);
}

void server_interface_standby(server_interface* this) {
    if (!this->binded_) {
        printf("Server nie je nabindovaný na žiadnu adresu ani port!\n");
        return;
    }

    int new_client_count = 0;
    do {
        printf("\n===================\n");
        printf("Pripájanie klientov\n");
        printf("===================\n");
        printf("Momentálne pripojených klientov: %d\n", bdd_server_get_client_count(&this->server_));
        printf("Zadajte počet klientov na pripojenie alebo 0, ak chcete ísť späť.\n");

        if (!server_interface_get_int(this, &new_client_count)) {
            continue;
        }

        if (new_client_count == 0) {
            return;
        }

        if (new_client_count > 0 && new_client_count + bdd_server_get_client_count(&this->server_) <= MAX_CLIENTS) {
            bdd_server_standby(&this->server_, new_client_count);
            return;
        }

        printf("Neplatný počet klientov! Musí to byť kladné číslo a ich celkový počet nemôže presiahnuť %d\n", MAX_CLIENTS);
    } while (true);
}

void server_interface_test_connections(server_interface * this) {
    if (bdd_server_get_client_count(&this->server_) == 0) {
        printf("Nie je pripojený žiaden klient.\n");
        return;
    }

    int disconnect_count = bdd_server_test_connections(&this->server_);

    if (disconnect_count == 0) {
        printf("Všetci klienti sú pripojení!\n");
    } else {
        printf(
            "Nie všetci klienti sú stále pripojení.\n"
            "Počet odpojených klientov: %d\n"
            "Počet stále pripojených klientov: %d\n",
            disconnect_count,
            bdd_server_get_client_count(&this->server_));
    }
}

_Bool file_exists(char* filename) {
    return access(filename, F_OK) == 0;
}

void server_interface_load_conf_file(server_interface* this, char* conf_path, size_t path_size) {
    _Bool exists = false;

    do {
        printf("\n======================================\n");
        printf("Určenie cesty ku konfiguračnému súboru\n");
        printf("======================================\n");
        printf("1 - Zmeniť cestu ku konfiguračnému súboru \n(nastavená je %s)\n", conf_path);
        printf("2 - Použiť nastavenú cestu\n");
        printf("Zadajte možnosť: ");

        int choice = 0;
        if (!server_interface_get_int(this, &choice)) {
            continue;
        }

        switch (choice) {
            case 1: {
                printf("Zadajte novú cestu ku konfiguračnému súboru:\n");

                int c;
                while ((c = getchar()) != '\n' && c != EOF);

                fgets(conf_path, path_size, stdin);

                size_t len = strlen(conf_path);
                if (len > 0 && conf_path[len - 1] == '\n') {
                    conf_path[len - 1] = '\0';
                }
                break;
            }

            case 2: {
                exists = file_exists(conf_path);
                if (!exists) {
                    printf("Súbor %s neexistuje alebo cesta k nemu je neplatná.\n", conf_path);
                }
                break;
            }

            default: {
                printf("Neplatná voľba..\n");
                break;
            }
        }
    } while (!exists);
}


void server_interface_run(server_interface* this) {
    if (bdd_server_get_client_count(&this->server_) <= 0) {
        printf("Najskôr pripojte klientov!\n");
        return;
    }

    if (!this->binded_) {
        printf("Server nie je nabindovaný na žiadnu adresu ani port!\n");
        return;
    }

    server_interface_test_connections(this);
    int client_count = bdd_server_get_client_count(&this->server_);

    if (client_count <= 0) {
        printf("Nie je pripojený žiaden klient! Musíte ich znova pripojiť.\n");
        return;
    }

    char conf_path[512] = "../Load_files/module_map.conf";
    server_interface_load_conf_file(this, conf_path, sizeof(conf_path));

    printf("Spúšťam hlavný program...\n");

    module_manager manager;
    module_manager_init(&manager, client_count);
    module_manager_load(&manager, conf_path);

    array_list* modules = module_manager_get_modules(&manager);
    int* distribution = calloc(client_count, sizeof(int));

    divider_default_divide(modules, client_count, distribution);
    module_manager_create_instructions(&manager, distribution, give_instruction);

    if (bdd_server_send_instructions(&this->server_, module_manager_get_instructions(&manager))) {
        bdd_server_send_modules(&this->server_, modules, distribution);

        bdd_message result;
        bdd_message_init(&result, 0);

        bdd_server_execute_instructions(&this->server_, &result);

        bdd_message_deserialize(&result, module_deserialize);
        module* mod = bdd_message_get_unique_payload(&result);
        bdd_message_destroy(&result);

        module_print_out(mod);
        module_destroy(mod);
        free(mod);
        printf("Hlavný program bol ukončený. Ukončuje sa aj spojenie s klientami.\n");
    } else {
        printf("Počas posielania dát sa klient odpojil od servera, výpočet sa nevykonal.\n");
        printf("Server odpája všetkých klientov.\n");
    }

    free(distribution);
    module_manager_destroy(&manager);
    bdd_server_end_sessions(&this->server_);
}