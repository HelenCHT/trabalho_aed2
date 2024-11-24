#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Constantes
#define MAX_PACIENTES 1000
#define MAX_SALAS 100
#define MAX_ESPECIALIDADES 50
#define MAX_MEDICOS 200
#define MAX_CONSULTAS 5000

// Estruturas
typedef struct {
    int id;
    char nome[50];
    int idade;
    float altura;
    float peso;
    char sintomas[100];
    int prioridade;
    int especialidadeNecessaria;
} Paciente;

typedef struct {
    int id;
    char nome[50];
    int especialidadeId;
    int horasTrabalhadas;
} Medico;

typedef struct {
    int id;
    char nome[50];
} Sala;

typedef struct {
    int pacienteId; 
    int medicoId;   
    int salaId;
    int horario; 
    int retorno;
} Consulta;


void lerDados(Paciente *pacientes, int *numPacientes, Medico *medicos, int *numMedicos, Sala *salas, int *numSalas) {
    FILE *arquivo = fopen("entrada.txt", "r");
    if (!arquivo) {
        printf("Erro ao abrir o arquivo de entrada.\n");
        return;
    }

    char linha[256];
    
    // Lendo pacientes
    fgets(linha, sizeof(linha), arquivo);
    while (fgets(linha, sizeof(linha), arquivo)) {
        if (strncmp(linha, "Salas:", 6) == 0) break; 
        if (*numPacientes >= MAX_PACIENTES) {
            printf("Número máximo de pacientes excedido.\n");
            break;
        }
        sscanf(linha, "%d %s %d %f %f %s %d %d", 
               &pacientes[*numPacientes].id,
               pacientes[*numPacientes].nome,
               &pacientes[*numPacientes].idade,
               &pacientes[*numPacientes].altura,
               &pacientes[*numPacientes].peso,
               pacientes[*numPacientes].sintomas,
               &pacientes[*numPacientes].prioridade,
               &pacientes[*numPacientes].especialidadeNecessaria); 
        (*numPacientes)++;
    }

    // Lendo salas
    while (fgets(linha, sizeof(linha), arquivo)) {
        if (strncmp(linha, "Especialidades:", 15) == 0) break; // Próxima seção
        if (*numSalas >= MAX_SALAS) {
            printf("Número máximo de salas excedido.\n");
            break;
        }
        sscanf(linha, "%d %s", 
               &salas[*numSalas].id,
               salas[*numSalas].nome);
        (*numSalas)++;
    }

    // Lendo especialidades
    int numEspecialidades = 0;
    char especialidades[MAX_ESPECIALIDADES][50];
    while (fgets(linha, sizeof(linha), arquivo)) {
        if (strncmp(linha, "Médicos:", 8) == 0) break; // Próxima seção
        if (numEspecialidades >= MAX_ESPECIALIDADES) {
            printf("Número máximo de especialidades excedido.\n");
            break;
        }
        sscanf(linha, "%d %s", 
               &numEspecialidades, 
               especialidades[numEspecialidades]);
        numEspecialidades++;
    }

    // Lendo médicos
    while (fgets(linha, sizeof(linha), arquivo)) {
        if (*numMedicos >= MAX_MEDICOS) {
            printf("Número máximo de médicos excedido.\n");
            break;
        }
        int especialidadeId;
        char especialidade[50];
        sscanf(linha, "%d %s %s", &medicos[(*numMedicos)].id, medicos[(*numMedicos)].nome, especialidade);

        // Converta a especialidade para o ID correspondente
        if (strcmp(especialidade, "Cardiologia") == 0) {
            especialidadeId = 1;
        } else if (strcmp(especialidade, "Clínica_Geral") == 0) {
            especialidadeId = 2;
        } else {
            printf("Especialidade desconhecida: %s\n", especialidade);
            continue;
        }

        medicos[(*numMedicos)].especialidadeId = especialidadeId;
        medicos[*numMedicos].horasTrabalhadas = 0; // Inicializa horas trabalhadas
        (*numMedicos)++;

    }

    fclose(arquivo);
    printf("Dados lidos do arquivo com sucesso!\n");
    printf("Pacientes lidos: %d\n", *numPacientes);
    printf("Salas lidas: %d\n", *numSalas);
    printf("Especialidades lidas: %d\n", numEspecialidades);
    printf("Médicos lidos: %d\n", *numMedicos);
}

// Função para alocar consultas
void alocarConsultas(Paciente *pacientes, int numPacientes, Medico *medicos, int numMedicos, 
                     Sala *salas, int numSalas, Consulta *consultas, int *numConsultas) {
    int horariosMedicos[MAX_MEDICOS][5][8] = {0}; 
    int horariosSalas[MAX_SALAS][5][8] = {0};

    srand(time(NULL)); 

    // Percorre cada paciente
    for (int i = 0; i < numPacientes; i++) {
        if (*numConsultas >= MAX_CONSULTAS) break; // Limita o número de consultas.

        // Verifica se o paciente faltará à consulta (5% de chance).
        if (rand() % 100 < 5) {
            printf("Paciente %s (ID %d) faltou à consulta.\n", pacientes[i].nome, pacientes[i].id);
            continue; // Pule para o próximo paciente.
        }

        int consultaAlocada = 0;

        // Tenta alocar consulta para o paciente
        for (int dia = 0; dia < 5 && !consultaAlocada; dia++) { 
            for (int h = 0; h < 8 && !consultaAlocada; h++) { 
                for (int m = 0; m < numMedicos && !consultaAlocada; m++) {
                    if (medicos[m].especialidadeId != pacientes[i].especialidadeNecessaria) {
                        continue; // Ignora médicos com especialidade incompatível.
                    }

                    for (int s = 0; s < numSalas && !consultaAlocada; s++) {
                        if (horariosMedicos[m][dia][h] == 0 && horariosSalas[s][dia][h] == 0) {
                            consultas[*numConsultas] = (Consulta){
                                .pacienteId = pacientes[i].id,
                                .medicoId = medicos[m].id,
                                .salaId = salas[s].id,
                                .horario = (dia * 8 + h + 8),
                                .retorno = 1 
                            };

                            horariosMedicos[m][dia][h] = 1;
                            horariosSalas[s][dia][h] = 1;

                            // Incrementa as horas trabalhadas do médico.
                            medicos[m].horasTrabalhadas++;

                            // Incrementa o número de consultas alocadas.
                            (*numConsultas)++;

                            consultaAlocada = 1; // Consulta foi alocada.
                        }
                    }
                }
            }
        }

        // Caso não seja possível alocar a consulta.
        if (!consultaAlocada) {
            printf("Erro: Não foi possível alocar consulta para o paciente %d.\n", pacientes[i].id);
        }
    }
}

const char *obterDiaDaSemana(int horario) {
    int diaIndex = (horario - 8) / 9;
    const char *diasDaSemana[] = {"Segunda-feira", "Terça-feira", "Quarta-feira", "Quinta-feira", "Sexta-feira"};
    return diasDaSemana[diaIndex % 5];
}

// Função ajustada para gerar relatório
void gerarRelatorio(Consulta *consultas, int numConsultas, Medico *medicos, int numMedicos, Paciente *pacientes, int numPacientes, Sala *salas, int numSalas) {
    FILE *arquivo = fopen("relatorio.txt", "w");
    if (!arquivo) {
        printf("Erro ao abrir arquivo para salvar o relatório.\n");
        return;
    }

    // Ordena as consultas por horário
    for (int i = 0; i < numConsultas - 1; i++) {
        for (int j = i + 1; j < numConsultas; j++) {
            if (consultas[i].horario > consultas[j].horario) {
                Consulta temp = consultas[i];
                consultas[i] = consultas[j];
                consultas[j] = temp;
            }
        }
    }

    // Inicializa contadores
    int atendimentosPorSalaDia[MAX_SALAS][5] = {0};
    int totalHorasTrabalhadas = 0; 
    int horasTrabalhadasPorMedico[MAX_MEDICOS] = {0};
    int ultimoDia = 0;

    fprintf(arquivo, "=============== Relatório de Consultas ===============\n\n");

    for (int i = 0; i < numConsultas; i++) {
        // Recupera os dados associados à consulta
        char nomePaciente[50] = "Desconhecido";
        char nomeMedico[50] = "Desconhecido";
        char nomeSala[50] = "Desconhecida";

        for (int p = 0; p < numPacientes; p++) {
            if (pacientes[p].id == consultas[i].pacienteId) {
                strcpy(nomePaciente, pacientes[p].nome);
                break;
            }
        }
        for (int m = 0; m < numMedicos; m++) {
            if (medicos[m].id == consultas[i].medicoId) {
                strcpy(nomeMedico, medicos[m].nome);
                horasTrabalhadasPorMedico[m]++;
                break;
            }
        }
        for (int s = 0; s < numSalas; s++) {
            if (salas[s].id == consultas[i].salaId) {
                strcpy(nomeSala, salas[s].nome);
                break;
            }
        }

        // Determina o horário e dia da consulta
        int dia = (consultas[i].horario - 8) / 8;
        int hora = (consultas[i].horario - 8) % 8 + 8;

        if (dia + 1 > ultimoDia) {
            ultimoDia = dia + 1;
        }

        // Identifica se é retorno ou primeira consulta
        const char *tipoConsulta = consultas[i].retorno == 0 ? "Retorno" : "Primeira Consulta";

        totalHorasTrabalhadas++; 
        for (int s = 0; s < numSalas; s++) {
            if (consultas[i].salaId == salas[s].id) {
                atendimentosPorSalaDia[s][dia]++;
                break;
            }
        }

        // Escreve detalhes no arquivo
        fprintf(arquivo, "Consulta %d:\n", i + 1);
        fprintf(arquivo, "  Paciente: %s (ID %d)\n", nomePaciente, consultas[i].pacienteId);
        fprintf(arquivo, "  Médico: %s (ID %d)\n", nomeMedico, consultas[i].medicoId);
        fprintf(arquivo, "  Sala: %s (ID %d)\n", nomeSala, consultas[i].salaId);
        fprintf(arquivo, "  Horário: Dia %d, %02dh\n", dia + 1, hora);
        fprintf(arquivo, "  Tipo: %s\n\n", tipoConsulta);
    }

    int semanasNecessarias = (ultimoDia + 4) / 5;

    fprintf(arquivo, "=============== Resumo de Atendimentos por Sala e Dia ===============\n\n");
    for (int s = 0; s < numSalas; s++) {
        fprintf(arquivo, "Sala %d (%s):\n", salas[s].id, salas[s].nome);
        for (int dia = 0; dia < ultimoDia; dia++) {
                fprintf(arquivo, "  Dia %d: %d Atendimentos: \n", dia + 1, atendimentosPorSalaDia[s][dia]);
        }

        fprintf(arquivo, "\n");
    }

    fprintf(arquivo, "=============== Resumo Geral ===============\n");
    fprintf(arquivo, "Último dia com consultas: Dia %d\n", ultimoDia);
    fprintf(arquivo, "Total de semanas necessárias: %d\n\n", semanasNecessarias);

    // Resumo por médico
    fprintf(arquivo, "Horas trabalhadas por médico:\n");
    for (int m = 0; m < numMedicos; m++) {
        fprintf(arquivo, "- Médico %s (ID %d): %d %s\n",
                medicos[m].nome, medicos[m].id, horasTrabalhadasPorMedico[m],
                horasTrabalhadasPorMedico[m] == 1 ? "hora" : "horas");
    }

    fclose(arquivo);
    printf("Relatório salvo em 'relatorio.txt'.\n");
}


void gerenciarRetornos(Consulta *consultas, int *numConsultas, int maxConsultas) {
    srand(time(NULL)); 
    int horasPorDia = 8; 

    for (int i = 0; i < *numConsultas; i++) {
        if (consultas[i].retorno == 1 && *numConsultas < maxConsultas) {
            Consulta novaConsulta = consultas[i];

            int diasExtras = 1+(rand() % 29 );
            novaConsulta.horario += diasExtras * horasPorDia;
            novaConsulta.retorno = 0;

            consultas[*numConsultas] = novaConsulta;
            (*numConsultas)++;
        }
    }
}

int main() {
    Paciente pacientes[MAX_PACIENTES];
    Medico medicos[MAX_MEDICOS];
    Sala salas[MAX_SALAS];
    Consulta consultas[MAX_CONSULTAS];
    int numPacientes = 0, numMedicos = 0, numSalas = 0, numConsultas = 0;

    lerDados(pacientes, &numPacientes, medicos, &numMedicos, salas, &numSalas);

    alocarConsultas(pacientes, numPacientes, medicos, numMedicos, salas, numSalas, consultas, &numConsultas);

    gerenciarRetornos(consultas, &numConsultas, MAX_CONSULTAS);

    gerarRelatorio(consultas, numConsultas, medicos, numMedicos, pacientes, numPacientes, salas, numSalas);

    return 0;
}