#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char cpf[15];
    char nome[100];
    char nascimento[11];
}
Paciente;

typedef struct {
    Paciente paciente;
    int risco;
    int ordemEntrada;
} PacienteFila;

typedef struct {
    Paciente paciente;
    int risco;
    int ordemEntrada;
    int ordemAtendimento;
} PacienteAtendido;

Paciente *cadastro = NULL;
int totalPacientes = 0;

PacienteFila *fila = NULL;
int tamanhoFila = 0;

PacienteAtendido *atendidos = NULL;
int totalAtendidos = 0;

int relogio_eventos = 0;

int cadastrar(const char *cpf, const char *nome, const char *nascimento) {

    for (int i = 0; i < totalPacientes; i++) {

        if (strcmp(cadastro[i].cpf, cpf) == 0) {
            return 0;
        }
    }

    Paciente *temp = realloc(
        cadastro,
        (totalPacientes + 1) * sizeof(Paciente)
    );

    if (temp == NULL) {
        return 0;
    }

    cadastro = temp;

    strcpy(cadastro[totalPacientes].cpf, cpf);
    strcpy(cadastro[totalPacientes].nome, nome);
    strcpy(cadastro[totalPacientes].nascimento, nascimento);

    totalPacientes++;

    return 1;
}

Paciente *buscar_cadastro(const char *cpf) {

    for (int i = 0; i < totalPacientes; i++) {

        if (strcmp(cadastro[i].cpf, cpf) == 0) {
            return &cadastro[i];
        }
    }

    return NULL;
}

int dar_entrada(const char *cpf, int risco) {

    Paciente *paciente = buscar_cadastro(cpf);

    if (paciente == NULL) {
        return 0;
    }

    for (int i = 0; i < tamanhoFila; i++) {

        if (strcmp(fila[i].paciente.cpf, cpf) == 0) {
            return 0;
        }
    }

    PacienteFila *temp = realloc(
        fila,
        (tamanhoFila + 1) * sizeof(PacienteFila)
    );

    if (temp == NULL) {
        return 0;
    }

    fila = temp;

    fila[tamanhoFila].paciente = *paciente;
    fila[tamanhoFila].risco = risco;
    fila[tamanhoFila].ordemEntrada = relogio_eventos;
    relogio_eventos++;

    tamanhoFila++;

    return 1;
}

PacienteFila *chamar_proximo() {

    if (tamanhoFila == 0) {
        return NULL;
    }

    int indiceMelhor = 0;

    for (int i = 1; i < tamanhoFila; i++) {

        if (fila[i].risco < fila[indiceMelhor].risco) {

            indiceMelhor = i;

        } else if (
            fila[i].risco == fila[indiceMelhor].risco &&
            fila[i].ordemEntrada < fila[indiceMelhor].ordemEntrada
        ) {

            indiceMelhor = i;
        }
    }

    PacienteFila *resultado = malloc(sizeof(PacienteFila));

    if (resultado == NULL) {
        return NULL;
    }

    *resultado = fila[indiceMelhor];

    PacienteAtendido *temp = realloc(
        atendidos,
        (totalAtendidos + 1) * sizeof(PacienteAtendido)
    );

    if (temp == NULL) {
        free(resultado);
        return NULL;
    }

    atendidos = temp;

    atendidos[totalAtendidos].paciente = resultado->paciente;
    atendidos[totalAtendidos].risco = resultado->risco;
    atendidos[totalAtendidos].ordemEntrada =
        resultado->ordemEntrada;
    atendidos[totalAtendidos].ordemAtendimento = relogio_eventos;
    relogio_eventos++;

    totalAtendidos++;

    for (int i = indiceMelhor; i < tamanhoFila - 1; i++) {
        fila[i] = fila[i + 1];
    }

    tamanhoFila--;

    if (tamanhoFila == 0) {

        free(fila);
        fila = NULL;

    } else {

        PacienteFila *novaFila = realloc(
            fila,
            tamanhoFila * sizeof(PacienteFila)
        );

        if (novaFila != NULL) {
            fila = novaFila;
        }
    }

    return resultado;
}

int desistir(const char *cpf) {

    int indice = -1;

    for (int i = 0; i < tamanhoFila; i++) {

        if (strcmp(fila[i].paciente.cpf, cpf) == 0) {
            indice = i;
            break;
        }
    }

    if (indice == -1) {
        return 0;
    }

    for (int i = indice; i < tamanhoFila - 1; i++) {
        fila[i] = fila[i + 1];
    }

    tamanhoFila--;

    if (tamanhoFila == 0) {

        free(fila);
        fila = NULL;

    } else {

        PacienteFila *temp = realloc(
            fila,
            tamanhoFila * sizeof(PacienteFila)
        );

        if (temp != NULL) {
            fila = temp;
        }
    }

    relogio_eventos++;
    return 1;
}

int tamanho_fila() {
    return tamanhoFila;
}

void relatorio_do_dia() {

    printf("\n===== RELATORIO DO DIA =====\n");

    if (totalAtendidos == 0) {
        printf("Nenhum paciente atendido.\n");
        return;
    }

    for (int i = 0; i < totalAtendidos; i++) {

        for (int j = i + 1; j < totalAtendidos; j++) {

            int esperaI =
                atendidos[i].ordemAtendimento -
                atendidos[i].ordemEntrada;

            int esperaJ =
                atendidos[j].ordemAtendimento -
                atendidos[j].ordemEntrada;

            if (esperaJ > esperaI) {

                PacienteAtendido temp = atendidos[i];

                atendidos[i] = atendidos[j];

                atendidos[j] = temp;
            }
        }
    }

    for (int i = 0; i < totalAtendidos; i++) {

        int espera =
            atendidos[i].ordemAtendimento -
            atendidos[i].ordemEntrada;

        printf(
            "%d. %s | CPF: %s | Espera: %d eventos\n",
            i + 1,
            atendidos[i].paciente.nome,
            atendidos[i].paciente.cpf,
            espera
        );
    }
}

void liberar_memoria() {

    free(cadastro);
    free(fila);
    free(atendidos);

    cadastro = NULL;
    fila = NULL;
    atendidos = NULL;
}

int main() {

    cadastrar(
        "111.111.111-11",
        "Joao",
        "15/03/2000"
    );

    cadastrar(
        "222.222.222-22",
        "Maria",
        "20/05/1998"
    );

    cadastrar(
        "333.333.333-33",
        "Carlos",
        "10/10/1985"
    );

    cadastrar(
        "444.444.444-44",
        "Ana",
        "01/01/1990"
    );

    Paciente *paciente =
        buscar_cadastro("222.222.222-22");

    if (paciente != NULL) {

        printf(
            "Paciente encontrado: %s\n",
            paciente->nome
        );
    }

    dar_entrada("111.111.111-11", 3);
    dar_entrada("222.222.222-22", 1);
    dar_entrada("333.333.333-33", 2);
    dar_entrada("444.444.444-44", 1);

    printf(
        "\nTamanho da fila: %d\n",
        tamanho_fila()
    );

    PacienteFila *proximo = chamar_proximo();

    if (proximo != NULL) {

        printf(
            "Proximo paciente: %s | Risco: %d\n",
            proximo->paciente.nome,
            proximo->risco
        );

        free(proximo);
    }

    printf(
        "Tamanho da fila: %d\n",
        tamanho_fila()
    );

    if (desistir("333.333.333-33")) {

        printf("Paciente desistiu da fila.\n");

    } else {

        printf("Paciente nao encontrado na fila.\n");
    }

    printf(
        "Tamanho da fila: %d\n",
        tamanho_fila()
    );

    proximo = chamar_proximo();

    if (proximo != NULL) {

        printf(
            "Proximo paciente: %s | Risco: %d\n",
            proximo->paciente.nome,
            proximo->risco
        );

        free(proximo);
    }

    relatorio_do_dia();

    liberar_memoria();

    return 0;
}
