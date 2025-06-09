#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <time.h>
#include <conio.h>

// Configurações
#define INSERT_FLIGHT_KEY 'i' // Tecla para inserir um plano de vôo
#define REMOVE_FLIGHT_KEY 'r' // Tecla para remover um plano de vôo
#define EDIT_FLIGHT_KEY 'e'   // Tecla para editar um plano de vôo

#define EDIT_FLIGHT_TIME_OF_DEPARTURE_KEY '1' // Tecla para editar o horário de saída do vôo
#define EDIT_FLIGHT_NUMBER_KEY '2'            // Tecla para editar o número do vôo
#define EDIT_FLIGHT_AIRLINE_KEY '3'           // Tecla para editar a linha aérea do vôo
#define EDIT_FLIGHT_DESTINATION_KEY '4'       // Tecla para editar o destino do vôo
#define FINISH_EDITING_FLIGHT_KEY '0'         // Tecla para finalizar a edição do vôo

#define CONTROL_PANEL_INPUT_BUFFER_SIZE 64     // Tamanho do buffer de entrada do painel de controle
#define CONTROL_PANEL_DISPLAY_BUFFER_SIZE 4096 // Tamanho do buffer de exibição do painel de controle

#define CHANCE_FOR_FLIGHT_DELAY 75 // Chance de atraso do vôo (em porcentagem)

#define MIN_FLIGHT_DELAY_TIME 5  // Tempo mínimo de atraso do vôo (em segundos)
#define MAX_FLIGHT_DELAY_TIME 20 // Tempo máximo de atraso do vôo (em segundos)

#define ERROR_STATE_TRANSITION_DELAY 1500 // Tempo de transição para o estado de erro (em milissegundos)

// Cores
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_RESET "\x1b[0m"

// Constantes
#define ESC_KEY 27
#define ENTER_KEY 13
#define BACKSPACE_KEY 8

// Utilitários
#define GENERATE_ENUM_TO_STRING_LIST_FUNC(FuncName, EnumType, LastValue, ToStringFunc) \
    static inline const char *FuncName()                                               \
    {                                                                                  \
        char *buffer = malloc(1024);                                                   \
        buffer[0] = '\0';                                                              \
                                                                                       \
        for (int idx = 0; idx < LastValue + 1; idx++)                                  \
        {                                                                              \
            char *str = ToStringFunc((EnumType)idx);                                   \
            strcat(buffer, str);                                                       \
                                                                                       \
            if (idx < LastValue)                                                       \
                strcat(buffer, ", ");                                                  \
        }                                                                              \
                                                                                       \
        return buffer;                                                                 \
    }

typedef enum
{
    GOL,
    AZUL,
    LATAM,
    AVIANCA,
    TOTAL_AIRLINES
} Airline;

/**
 * Converte o enum Airline para uma string.
 */
static inline char *airline_to_string(Airline airline)
{
    switch (airline)
    {
    case GOL:
        return "GOL";

    case AZUL:
        return "Azul";

    case LATAM:
        return "LATAM";

    case AVIANCA:
        return "Avianca";

    default:
        return "Desconhecido";
    }
}

/**
 * Converte uma string para o enum Airline.
 */
static inline Airline string_to_airline(const char *airline)
{
    if (strcmp(airline, "GOL") == 0)
        return GOL;
    else if (strcmp(airline, "Azul") == 0)
        return AZUL;
    else if (strcmp(airline, "LATAM") == 0)
        return LATAM;
    else if (strcmp(airline, "Avianca") == 0)
        return AVIANCA;

    return -1;
}

GENERATE_ENUM_TO_STRING_LIST_FUNC(get_all_airlines_string, Airline, TOTAL_AIRLINES - 1, airline_to_string)

typedef enum
{
    CONGONHAS,
    GUARULHOS,
    RECIFE,
    PORTO_ALEGRE,
    FORTALEZA,
    TOTAL_DESTINATIONS
} Destination;

/**
 * Converte o enum Destination para uma string.
 */
static inline char *destination_to_string(Destination destination)
{
    switch (destination)
    {
    case CONGONHAS:
        return "Congonhas";

    case GUARULHOS:
        return "Guarulhos";

    case RECIFE:
        return "Recife";

    case PORTO_ALEGRE:
        return "Porto Alegre";

    case FORTALEZA:
        return "Fortaleza";

    default:
        return "Desconhecido";
    }
}

/**
 * Converte uma string para o enum Destination.
 */
static inline Destination string_to_destination(const char *destination)
{
    if (strcmp(destination, "Congonhas") == 0)
        return CONGONHAS;
    else if (strcmp(destination, "Guarulhos") == 0)
        return GUARULHOS;
    else if (strcmp(destination, "Recife") == 0)
        return RECIFE;
    else if (strcmp(destination, "Porto Alegre") == 0)
        return PORTO_ALEGRE;
    else if (strcmp(destination, "Fortaleza") == 0)
        return FORTALEZA;

    return -1;
}

GENERATE_ENUM_TO_STRING_LIST_FUNC(get_all_destinations_string, Destination, TOTAL_DESTINATIONS - 1, destination_to_string)

typedef enum
{
    WAITING,
    CANCELLED,
    DEPARTED,
    DELAYED
} FlightStatus;

/**
 * Converte o enum FlightStatus para uma string.
 */
static inline char *flight_status_to_string(FlightStatus flight_status)
{
    switch (flight_status)
    {
    case WAITING:
        return "Aguardando";

    case CANCELLED:
        return "Cancelado";

    case DEPARTED:
        return "Partiu";

    case DELAYED:
        return "Atrasado";

    default:
        return "Desconhecido";
    }
}

/**
 * Converte uma string para o enum FlightStatus.
 */
static inline FlightStatus string_to_flight_status(const char *flight_status)
{
    if (strcmp(flight_status, "Aguardando") == 0)
        return WAITING;
    else if (strcmp(flight_status, "Cancelado") == 0)
        return CANCELLED;
    else if (strcmp(flight_status, "Partido") == 0)
        return DEPARTED;
    else if (strcmp(flight_status, "Atrasado") == 0)
        return DELAYED;

    return -1;
}

typedef struct Flight
{
    long time_of_departure;
    short flight_number;
    Airline airline;
    Destination destination;
    FlightStatus flight_status;
    struct Flight *next;

    /* Offset aleatório para o horário de partida, apenas para fins de simulação de atraso */
    long time_of_departure_random_offset;
} Flight;

typedef struct Board
{
    struct Flight *head;
} Board;

typedef enum
{
    IDLE,

    INSERTING_FLIGHT,
    I_INSERTING_FLIGHT_TIME_OF_DEPARTURE,
    I_INVALID_FLIGHT_TIME_OF_DEPARTURE,

    I_INSERTING_FLIGHT_NUMBER,
    I_INVALID_FLIGHT_NUMBER,
    I_FLIGHT_NUMBER_ALREADY_EXISTS,

    I_INSERTING_FLIGHT_AIRLINE,
    I_INVALID_FLIGHT_AIRLINE,

    I_INSERTING_FLIGHT_DESTINATION,
    I_INVALID_FLIGHT_DESTINATION,

    I_FINISH_INSERTING_FLIGHT,

    REMOVING_FLIGHT,
    R_SPECIFYING_FLIGHT_NUMBER,
    R_INVALID_FLIGHT_NUMBER,
    R_FLIGHT_NUMBER_NOT_FOUND,

    R_FINISH_REMOVING_FLIGHT,

    EDITING_FLIGHT,
    E_SPECIFYING_FLIGHT_NUMBER,
    E_FLIGHT_NUMBER_NOT_FOUND,
    E_INVALID_FLIGHT_NUMBER_SPECIFYING_FLIGHT_NUMBER,

    E_SELECTING_WHAT_TO_EDIT,

    E_EDITING_FLIGHT_TIME_OF_DEPARTURE,
    E_INVALID_FLIGHT_TIME_OF_DEPARTURE,

    E_EDITING_FLIGHT_NUMBER,
    E_INVALID_FLIGHT_NUMBER_EDITING_FLIGHT_NUMBER,
    E_FLIGHT_NUMBER_ALREADY_EXISTS,

    E_EDITING_FLIGHT_AIRLINE,
    E_INVALID_FLIGHT_AIRLINE,

    E_EDITING_FLIGHT_DESTINATION,
    E_INVALID_FLIGHT_DESTINATION,

    E_FINISH_EDITING_FLIGHT,
} ControlPanelState;

typedef struct ControlPanel
{
    char input_buffer[CONTROL_PANEL_INPUT_BUFFER_SIZE];
    char display_buffer[CONTROL_PANEL_DISPLAY_BUFFER_SIZE];
    int input_buffer_cursor_position;
    void *state_data;
    double state_transition_timestamp;
    ControlPanelState state;
    ControlPanelState previous_state;
    struct Board *board;
} ControlPanel;

/*
==================================================================================
===================================== UTILIDADES =================================
==================================================================================
*/

/**
 * Adiciona espaçamentos a uma string de texto para centralizá-la em um determinado espaço.
 */
char *add_text_paddings(int width, char *input)
{
    char *text = (char *)malloc(width + 1);
    memset(text, ' ', width);
    memcpy(text + (width - strlen(input)) / 2, input, strlen(input));
    text[width] = '\0';
    return text;
}

/**
 * Obtém o timestamp Unix atual.
 */
time_t get_unix_timestamp()
{
    return time(NULL);
}

/**
 * Obtém o timestamp Unix atual marcado para o início do dia.
 */
time_t get_unix_timestamp_at_beginning_of_day()
{
    time_t now = time(NULL);
    struct tm *tm_now = localtime(&now);

    tm_now->tm_hour = 0;
    tm_now->tm_min = 0;
    tm_now->tm_sec = 0;

    return mktime(tm_now);
}

/**
 * Obtém o timestamp Unix com um deslocamento de horas e minutos.
 */
time_t get_unix_timestamp_with_hm_offset(const int hours, const int minutes)
{
    time_t now = time(NULL);
    struct tm *tm_now = localtime(&now);

    tm_now->tm_hour = hours;
    tm_now->tm_min = minutes;
    tm_now->tm_sec = 0;

    return mktime(tm_now);
}

/**
 * Converte um timestamp Unix para uma string no formato "HH:MM".
 */
char *unix_timestamp_to_hm_string(const time_t unix)
{
    struct tm *tm = localtime(&unix);

    char *buffer = (char *)malloc(6);
    snprintf(buffer, 6, "%02d:%02d", tm->tm_hour, tm->tm_min);

    return buffer;
}

/**
 * Formata uma string para exibição no painel de controle.
 */
char *format_string_va(const char *format, ...)
{
    char *formatted_string = (char *)malloc(sizeof(char) * CONTROL_PANEL_DISPLAY_BUFFER_SIZE);
    va_list var_args_substrings;
    va_start(var_args_substrings, format);
    vsnprintf(formatted_string, sizeof(char) * CONTROL_PANEL_DISPLAY_BUFFER_SIZE, format, var_args_substrings);
    va_end(var_args_substrings);
    return formatted_string;
}

/**
 * Colore uma string para exibição no painel de controle.
 */
char *colorize_string(const char *input, const char *color)
{
    char *colored_string = (char *)malloc(strlen(input) + strlen(color) + strlen(ANSI_COLOR_RESET) + 1);
    snprintf(colored_string, strlen(input) + strlen(color) + strlen(ANSI_COLOR_RESET) + 1, "%s%s%s", color, input, ANSI_COLOR_RESET);
    return colored_string;
}

/*
==================================================================================
===================================== TERMINAL ===================================
==================================================================================
*/

/**
 * Prepara o terminal para exibir caracteres UTF-8 corretamente.
 */
void prepare_virtual_terminal_for_utf8()
{
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
}

/**
 * Limpa o terminal.
 */
void clear_terminal()
{
    system("cls");
}

/*
==================================================================================
===================================== VÔO ========================================
==================================================================================
*/

/**
 * Cria um novo vôo dado os valores iniciais.
 */
Flight *new_flight(
    long time_of_departure,
    short flight_number,
    Airline airline,
    Destination destination,
    FlightStatus flight_status)
{
    Flight *flight = (Flight *)malloc(sizeof(Flight));
    flight->time_of_departure = time_of_departure;
    flight->flight_number = flight_number;
    flight->airline = airline;
    flight->destination = destination;
    flight->flight_status = flight_status;
    return flight;
}

/*
==================================================================================
===================================== PLACAR =====================================
==================================================================================
*/

/**
 * Cria um novo placar de vôos.
 */
Board *new_board()
{
    Board *board = (Board *)malloc(sizeof(Board));
    board->head = NULL;
    return board;
}

/**
 * Exibe o placar de vôos na tela do usuário.
 */
void print_board(Board *board)
{
    printf("┌────────────────┬───────────────┬───────────────────┬───────────────┬──────────────┐\n");
    printf("│ Horário de Vôo │ Número do Vôo │    Linha Áerea    │    Destino    │    Status    │\n");
    printf("├────────────────┼───────────────┼───────────────────┼───────────────┼──────────────┤\n");

    Flight *current_flight = board->head;
    while (current_flight != NULL)
    {
        char *time_of_departure_str = unix_timestamp_to_hm_string((time_t)current_flight->time_of_departure);

        char *flight_number_str = (char *)malloc(4);
        snprintf(flight_number_str, 5, "%04d", current_flight->flight_number);

        char *time_of_departure = add_text_paddings(14, time_of_departure_str);
        char *flight_number = add_text_paddings(13, flight_number_str);

        char *airline = add_text_paddings(17, airline_to_string(current_flight->airline));
        char *destination = add_text_paddings(13, destination_to_string(current_flight->destination));
        char *flight_status = add_text_paddings(12, flight_status_to_string(current_flight->flight_status));

        printf("│ %s ┼ %s ┼ %s ┼ %s ┼ %s │\n",
               time_of_departure,
               flight_number,
               airline,
               destination,
               colorize_string(flight_status, current_flight->flight_status == WAITING ? ANSI_COLOR_YELLOW : ANSI_COLOR_RED));

        current_flight = current_flight->next;

        free(time_of_departure_str);
        free(flight_number_str);
        free(time_of_departure);
        free(flight_number);
        free(airline);
        free(destination);
        free(flight_status);
    }

    printf("└────────────────┴───────────────┴───────────────────┴───────────────┴──────────────┘\n");
}

/**
 * Insere um vôo no placar posterior ao vôo com um hórario de saída do vôo menor.
 */
void insert_flight(Board *board, Flight *flight)
{
    if (board->head == NULL || board->head->time_of_departure > flight->time_of_departure)
    {
        flight->next = board->head;
        board->head = flight;
        return;
    }

    Flight *current_flight = board->head;
    while (current_flight->next != NULL && current_flight->next->time_of_departure < flight->time_of_departure)
    {
        current_flight = current_flight->next;
    }

    flight->next = current_flight->next;
    current_flight->next = flight;
}

/**
 * Remove um vôo do placar dado o número do vôo.
 */
void remove_flight(Board *board, short flight_number)
{
    Flight *current_flight = board->head;
    Flight *previous_flight = NULL;

    while (current_flight != NULL)
    {
        if (current_flight->flight_number == flight_number)
        {
            if (previous_flight == NULL)
                board->head = current_flight->next;
            else
                previous_flight->next = current_flight->next;

            free(current_flight);
            break;
        }

        previous_flight = current_flight;
        current_flight = current_flight->next;
    }
}

/**
 * Verifica se um vôo já existe no placar dado o número do vôo.
 */
int flight_already_exists(Board *board, short flight_number, Flight *flight_prt)
{
    Flight *current_flight = board->head;
    while (current_flight != NULL)
    {
        if (current_flight->flight_number == flight_number)
        {
            if (flight_prt != NULL)
                *flight_prt = *current_flight;

            return 1;
        }

        current_flight = current_flight->next;
    }

    return 0;
}

/*
==================================================================================
===================================== PAINEL DE CONTROLE =========================
==================================================================================
*/

/**
 * Cria um novo usuário painel de controle da aplicação.
 */
ControlPanel *new_control_panel(Board *board)
{
    ControlPanel *control_panel = (ControlPanel *)malloc(sizeof(ControlPanel));

    memset(control_panel->input_buffer, 0, sizeof(control_panel->input_buffer));
    memset(control_panel->display_buffer, 0, sizeof(control_panel->display_buffer));
    control_panel->input_buffer[0] = '\0';
    control_panel->display_buffer[0] = '\0';

    control_panel->input_buffer_cursor_position = 0;

    control_panel->state_data = NULL;
    control_panel->state_transition_timestamp = 0;
    control_panel->state = IDLE;
    control_panel->previous_state = IDLE;

    control_panel->board = board;
    return control_panel;
}

void set_control_panel_state(ControlPanel *control_panel, ControlPanelState state, int transition_delay_in_ms)
{
    if (control_panel->state == state)
        return;

    control_panel->previous_state = control_panel->state;
    control_panel->state = state;
    control_panel->state_transition_timestamp = get_unix_timestamp() + (transition_delay_in_ms / 1000.0);
}

/**
 * Adiciona um caractere ao buffer de entrada do painel de controle.
 */
void append_control_panel_input_buffer(ControlPanel *control_panel, char input)
{
    if (control_panel->input_buffer_cursor_position < sizeof(control_panel->input_buffer) - 1)
    {
        control_panel->input_buffer[control_panel->input_buffer_cursor_position++] = input;
        control_panel->input_buffer[control_panel->input_buffer_cursor_position] = '\0';
    }
}

/**
 * Remove o último caractere do buffer de entrada do painel de controle.
 */
void remove_control_panel_input_buffer(ControlPanel *control_panel)
{
    if (control_panel->input_buffer_cursor_position > 0)
        control_panel->input_buffer[--control_panel->input_buffer_cursor_position] = '\0';
}

/**
 * Limpa o buffer de entrada do painel de controle.
 */
void clear_control_panel_input_buffer(ControlPanel *control_panel)
{
    memset(control_panel->input_buffer, 0, sizeof(control_panel->input_buffer));
    control_panel->input_buffer_cursor_position = 0;
}

/**
 * Define o buffer de entrada do painel de controle.
 */
void set_control_panel_display_text(ControlPanel *control_panel, const char *display_buffer)
{
    strncpy(control_panel->display_buffer, display_buffer, sizeof(control_panel->display_buffer) - 1);
    control_panel->display_buffer[sizeof(control_panel->display_buffer) - 1] = '\0';
}

/**
 * Exibe o estado atual das interações do usuário na tela.
 */
void print_control_panel_interaction_state(ControlPanel *control_panel)
{
    printf(control_panel->display_buffer);
}

/**
 * Verifica se o terminal está pronto para receber entradas do usuário.
 */
void handle_control_panel_input(ControlPanel *control_panel, void (*callback)(ControlPanel *, char))
{
    if (_kbhit())
    {
        char input = (char)_getch();
        if (input == ENTER_KEY)
        {
        }
        else if (input == BACKSPACE_KEY)
            remove_control_panel_input_buffer(control_panel);
        else if (input == ESC_KEY)
        {
        }
        else
            append_control_panel_input_buffer(control_panel, input);

        if (callback != NULL)
            callback(control_panel, input);
    }
}

/*
==================================================================================
===================================== ESTADOS DO PAINEL DE CONTROLE ==============
==================================================================================
*/

/*
===================================== Funções utilitárias para estados ===========
*/

/**
 * Verifica se o horário de saída do vôo é válido.
 */
int is_flight_time_of_departure_valid(const char *input, long *time_of_departure_ptr)
{
    if (strspn(input, "0123456789") != strlen(input))
    {
        return 0;
    }

    if (strlen(input) != 4)
    {
        return 0;
    }

    int hours, minutes;
    if (sscanf(input, "%2d%2d", &hours, &minutes) != 2)
    {
        return 0;
    }

    if (hours < 0 || hours > 23 || minutes < 0 || minutes > 59)
    {
        return 0;
    }

    long time_of_departure = get_unix_timestamp_with_hm_offset(hours, minutes);
    if (time_of_departure < get_unix_timestamp())
    {
        return 0;
    }

    *time_of_departure_ptr = time_of_departure;
    return 1;
}

/**
 * Verifica se o número do vôo é válido.
 */
int is_flight_number_valid(const char *input, short *flight_number_ptr)
{
    if (strspn(input, "0123456789") != strlen(input))
    {
        return 0;
    }

    short flight_number = strtol(input, NULL, 10);
    if (flight_number < 0 || flight_number > 9999)
    {
        return 0;
    }

    *flight_number_ptr = flight_number;
    return 1;
}

/**
 * Define o estado do painel de controle e limpa o buffer de entrada.
 */
void transition_control_panel_state(ControlPanel *control_panel, ControlPanelState state, int transition_delay_in_ms)
{
    set_control_panel_state(control_panel, state, transition_delay_in_ms);
    clear_control_panel_input_buffer(control_panel);
}

/*
===================================== Parado =====================================
*/
void handle_IDLE_state_input(ControlPanel *control_panel, char input)
{
    if (input == ESC_KEY)
    {
        set_control_panel_state(control_panel, IDLE, 0);
        clear_control_panel_input_buffer(control_panel);
    }
    else if (input == INSERT_FLIGHT_KEY)
    {
        set_control_panel_state(control_panel, INSERTING_FLIGHT, 0);
        clear_control_panel_input_buffer(control_panel);
    }
    else if (input == REMOVE_FLIGHT_KEY)
    {
        set_control_panel_state(control_panel, REMOVING_FLIGHT, 0);
        clear_control_panel_input_buffer(control_panel);
    }
    else if (input == EDIT_FLIGHT_KEY)
    {
        set_control_panel_state(control_panel, EDITING_FLIGHT, 0);
        clear_control_panel_input_buffer(control_panel);
    }
}

void handle_IDLE_state(ControlPanel *control_panel)
{
    char display_text[CONTROL_PANEL_DISPLAY_BUFFER_SIZE];
    snprintf(display_text, sizeof(display_text), "%s%s%s%s%s%s%s",
             colorize_string("Selecione uma das opções abaixo:\n", ANSI_COLOR_YELLOW),
             colorize_string("  [%c]", ANSI_COLOR_RED), " - Inserir plano de vôo\n",
             colorize_string("  [%c]", ANSI_COLOR_RED), " - Remover plano de vôo\n",
             colorize_string("  [%c]", ANSI_COLOR_RED), " - Editar plano de vôo\n");

    set_control_panel_display_text(control_panel, format_string_va(display_text,
                                                                   INSERT_FLIGHT_KEY,
                                                                   REMOVE_FLIGHT_KEY,
                                                                   EDIT_FLIGHT_KEY));
    handle_control_panel_input(control_panel, handle_IDLE_state_input);
}

/*
===================================== Inserindo um vôo ===========================
*/
void handle_INSERTING_FLIGHT_state(ControlPanel *control_panel)
{
    if (control_panel->state_data != NULL)
    {
        free(control_panel->state_data);
        control_panel->state_data = NULL;
    }

    Flight *flight = (Flight *)malloc(sizeof(Flight));
    memset(flight, 0, sizeof(Flight));

    control_panel->state_data = flight;
    set_control_panel_state(control_panel, I_INSERTING_FLIGHT_TIME_OF_DEPARTURE, 0);
}

/*
===================================== Inserindo horário de saída do vôo ==========
*/
void handle_I_INSERTING_FLIGHT_TIME_OF_DEPARTURE_state_input(ControlPanel *control_panel, char input)
{
    if (input == ESC_KEY)
    {
        set_control_panel_state(control_panel, IDLE, 0);
        clear_control_panel_input_buffer(control_panel);
    }
    else if (input == ENTER_KEY)
    {
        long time_of_departure;
        if (!is_flight_time_of_departure_valid(control_panel->input_buffer, &time_of_departure))
        {
            transition_control_panel_state(control_panel, I_INVALID_FLIGHT_TIME_OF_DEPARTURE, 0);
            return;
        }

        long time_of_departure_random_offset = 0;
        if (rand() % 100 <= CHANCE_FOR_FLIGHT_DELAY)
            time_of_departure_random_offset = MIN_FLIGHT_DELAY_TIME + rand() % (MAX_FLIGHT_DELAY_TIME - MIN_FLIGHT_DELAY_TIME + 1);

        Flight *flight = (Flight *)control_panel->state_data;
        flight->time_of_departure = time_of_departure;
        flight->time_of_departure_random_offset = time_of_departure_random_offset;

        transition_control_panel_state(control_panel, I_INSERTING_FLIGHT_NUMBER, 0);
    }
}

void handle_I_INSERTING_FLIGHT_TIME_OF_DEPARTURE_state(ControlPanel *control_panel)
{
    char display_text[CONTROL_PANEL_DISPLAY_BUFFER_SIZE];
    snprintf(display_text, sizeof(display_text), "%s%s%s%s%s%s",
             colorize_string("Presione ", ANSI_COLOR_YELLOW), colorize_string("[ESC]", ANSI_COLOR_RED), colorize_string(" para cancelar a operação.\n", ANSI_COLOR_YELLOW),
             colorize_string("Digite o horário de saída do vôo ", ANSI_COLOR_YELLOW), "(HHMM): ",
             "%s_\n");

    set_control_panel_display_text(control_panel, format_string_va(display_text, control_panel->input_buffer));
    handle_control_panel_input(control_panel, handle_I_INSERTING_FLIGHT_TIME_OF_DEPARTURE_state_input);
}

void handle_I_INVALID_FLIGHT_TIME_OF_DEPARTURE_state(ControlPanel *control_panel)
{
    set_control_panel_display_text(control_panel, colorize_string("Horário de saída do vôo inválido. Tente novamente.\n", ANSI_COLOR_RED));
    transition_control_panel_state(control_panel, I_INSERTING_FLIGHT_TIME_OF_DEPARTURE, ERROR_STATE_TRANSITION_DELAY);
}

/*
===================================== Inserindo número do vôo ====================
*/
void handle_I_INSERTING_FLIGHT_NUMBER_state_input(ControlPanel *control_panel, char input)
{
    if (input == ESC_KEY)
    {
        transition_control_panel_state(control_panel, IDLE, 0);
    }
    else if (input == ENTER_KEY)
    {
        short flight_number;
        if (!is_flight_number_valid(control_panel->input_buffer, &flight_number))
        {
            transition_control_panel_state(control_panel, I_INVALID_FLIGHT_NUMBER, 0);
            return;
        }

        if (flight_already_exists(control_panel->board, flight_number, NULL))
        {
            transition_control_panel_state(control_panel, I_FLIGHT_NUMBER_ALREADY_EXISTS, 0);
            return;
        }

        Flight *flight = (Flight *)control_panel->state_data;
        flight->flight_number = flight_number;

        transition_control_panel_state(control_panel, I_INSERTING_FLIGHT_AIRLINE, 0);
    }
}

void handle_I_INSERTING_FLIGHT_NUMBER_state(ControlPanel *control_panel)
{
    char display_text[CONTROL_PANEL_DISPLAY_BUFFER_SIZE];
    snprintf(display_text, sizeof(display_text), "%s%s%s%s%s%s",
             colorize_string("Presione ", ANSI_COLOR_YELLOW), colorize_string("[ESC]", ANSI_COLOR_RED), colorize_string(" para cancelar a operação.\n", ANSI_COLOR_YELLOW),
             colorize_string("Digite o número do vôo ", ANSI_COLOR_YELLOW), "(0000): ",
             "%s_\n");

    set_control_panel_display_text(control_panel, format_string_va(display_text, control_panel->input_buffer));
    handle_control_panel_input(control_panel, handle_I_INSERTING_FLIGHT_NUMBER_state_input);
}

void handle_I_INVALID_FLIGHT_NUMBER_state(ControlPanel *control_panel)
{
    set_control_panel_display_text(control_panel, colorize_string("Número do vôo inválido. Tente novamente.\n", ANSI_COLOR_RED));
    transition_control_panel_state(control_panel, I_INSERTING_FLIGHT_NUMBER, ERROR_STATE_TRANSITION_DELAY);
}

void handle_I_FLIGHT_NUMBER_ALREADY_EXISTS_state(ControlPanel *control_panel)
{
    set_control_panel_display_text(control_panel, colorize_string("Número do vôo já existe. Tente novamente.\n", ANSI_COLOR_RED));
    transition_control_panel_state(control_panel, I_INSERTING_FLIGHT_NUMBER, ERROR_STATE_TRANSITION_DELAY);
}

/*
===================================== Inserindo linha aérea do vôo ===============
*/
void handle_I_INSERTING_FLIGHT_AIRLINE_state_input(ControlPanel *control_panel, char input)
{
    if (input == ESC_KEY)
    {
        transition_control_panel_state(control_panel, IDLE, 0);
    }
    else if (input == ENTER_KEY)
    {
        Airline airline = string_to_airline(control_panel->input_buffer);
        if (airline == -1)
        {
            transition_control_panel_state(control_panel, I_INVALID_FLIGHT_AIRLINE, 0);
            return;
        }

        Flight *flight = (Flight *)control_panel->state_data;
        flight->airline = airline;

        transition_control_panel_state(control_panel, I_INSERTING_FLIGHT_DESTINATION, 0);
    }
}

void handle_I_INSERTING_FLIGHT_AIRLINE_state(ControlPanel *control_panel)
{
    char display_text[CONTROL_PANEL_DISPLAY_BUFFER_SIZE];
    snprintf(display_text, sizeof(display_text), "%s%s%s%s%s%s%s%s",
             colorize_string("Presione ", ANSI_COLOR_YELLOW), colorize_string("[ESC]", ANSI_COLOR_RED), colorize_string(" para cancelar a operação.\n", ANSI_COLOR_YELLOW),
             colorize_string("Digite a linha aérea: ", ANSI_COLOR_YELLOW),
             "%s_\n",
             "\n",
             colorize_string("Linhas aéreas disponíveis:\n", ANSI_COLOR_YELLOW),
             colorize_string("%s\n", ANSI_COLOR_GREEN));

    set_control_panel_display_text(control_panel, format_string_va(display_text, control_panel->input_buffer, get_all_airlines_string()));
    handle_control_panel_input(control_panel, handle_I_INSERTING_FLIGHT_AIRLINE_state_input);
}

void handle_I_INVALID_FLIGHT_AIRLINE_state(ControlPanel *control_panel)
{
    set_control_panel_display_text(control_panel, colorize_string("Linha aérea inválida. Tente novamente.\n", ANSI_COLOR_RED));
    transition_control_panel_state(control_panel, I_INSERTING_FLIGHT_AIRLINE, ERROR_STATE_TRANSITION_DELAY);
}

/*
===================================== Inserindo destino do vôo ===================
*/
void handle_I_INSERTING_FLIGHT_DESTINATION_state_input(ControlPanel *control_panel, char input)
{
    if (input == ESC_KEY)
    {
        transition_control_panel_state(control_panel, IDLE, 0);
    }
    else if (input == ENTER_KEY)
    {
        Destination destination = string_to_destination(control_panel->input_buffer);
        if (destination == -1)
        {
            transition_control_panel_state(control_panel, I_INVALID_FLIGHT_DESTINATION, 0);
            return;
        }

        Flight *flight = (Flight *)control_panel->state_data;
        flight->destination = destination;

        transition_control_panel_state(control_panel, I_FINISH_INSERTING_FLIGHT, 0);
    }
}

void handle_I_INSERTING_FLIGHT_DESTINATION_state(ControlPanel *control_panel)
{
    char display_text[CONTROL_PANEL_DISPLAY_BUFFER_SIZE];
    snprintf(display_text, sizeof(display_text), "%s%s%s%s%s%s%s%s",
             colorize_string("Presione ", ANSI_COLOR_YELLOW), colorize_string("[ESC]", ANSI_COLOR_RED), colorize_string(" para cancelar a operação.\n", ANSI_COLOR_YELLOW),
             colorize_string("Digite o destino: ", ANSI_COLOR_YELLOW),
             "%s_\n",
             "\n",
             colorize_string("Destinos disponíveis:\n", ANSI_COLOR_YELLOW),
             colorize_string("%s\n", ANSI_COLOR_GREEN));

    set_control_panel_display_text(control_panel, format_string_va(display_text, control_panel->input_buffer, get_all_destinations_string()));
    handle_control_panel_input(control_panel, handle_I_INSERTING_FLIGHT_DESTINATION_state_input);
}

void handle_I_INVALID_FLIGHT_DESTINATION_state(ControlPanel *control_panel)
{
    set_control_panel_display_text(control_panel, colorize_string("Destino inválido. Tente novamente.\n", ANSI_COLOR_RED));
    transition_control_panel_state(control_panel, I_INSERTING_FLIGHT_DESTINATION, ERROR_STATE_TRANSITION_DELAY);
}

/*
===================================== Finalizando inserção do vôo ================
*/
void handle_I_FINISH_INSERTING_FLIGHT_state(ControlPanel *control_panel)
{
    if (control_panel->state_data == NULL)
    {
        transition_control_panel_state(control_panel, IDLE, 0);
    }

    Flight *flight = malloc(sizeof(Flight));
    memcpy(flight, control_panel->state_data, sizeof(Flight));

    insert_flight(control_panel->board, flight);

    free(control_panel->state_data);
    control_panel->state_data = NULL;

    transition_control_panel_state(control_panel, IDLE, 0);
}

/*
===================================== Removendo um vôo ===========================
*/
void handle_REMOVING_FLIGHT_state(ControlPanel *control_panel)
{
    if (control_panel->state_data != NULL)
    {
        free(control_panel->state_data);
        control_panel->state_data = NULL;
    }

    transition_control_panel_state(control_panel, R_SPECIFYING_FLIGHT_NUMBER, 0);
}

void handle_R_SPECIFYING_FLIGHT_NUMBER_state_input(ControlPanel *control_panel, char input)
{
    if (input == ESC_KEY)
    {
        transition_control_panel_state(control_panel, IDLE, 0);
    }
    else if (input == ENTER_KEY)
    {
        short flight_number;
        if (!is_flight_number_valid(control_panel->input_buffer, &flight_number))
        {
            transition_control_panel_state(control_panel, R_INVALID_FLIGHT_NUMBER, 0);
            return;
        }

        if (!flight_already_exists(control_panel->board, flight_number, NULL))
        {
            transition_control_panel_state(control_panel, R_FLIGHT_NUMBER_NOT_FOUND, 0);
            return;
        }

        control_panel->state_data = (short *)malloc(sizeof(short));
        memcpy(control_panel->state_data, &flight_number, sizeof(short));

        transition_control_panel_state(control_panel, R_FINISH_REMOVING_FLIGHT, 0);
    }
}

void handle_R_SPECIFYING_FLIGHT_NUMBER_state(ControlPanel *control_panel)
{
    char display_text[CONTROL_PANEL_DISPLAY_BUFFER_SIZE];
    snprintf(display_text, sizeof(display_text), "%s%s%s%s%s",
             colorize_string("Presione ", ANSI_COLOR_YELLOW), colorize_string("[ESC]", ANSI_COLOR_RED), colorize_string(" para cancelar a operação.\n", ANSI_COLOR_YELLOW),
             colorize_string("Digite o número do vôo: ", ANSI_COLOR_YELLOW),
             "%s_\n");

    set_control_panel_display_text(control_panel, format_string_va(display_text, control_panel->input_buffer));
    handle_control_panel_input(control_panel, handle_R_SPECIFYING_FLIGHT_NUMBER_state_input);
}

void handle_R_INVALID_FLIGHT_NUMBER_state(ControlPanel *control_panel)
{
    set_control_panel_display_text(control_panel, colorize_string("Número do vôo inválido. Tente novamente.\n", ANSI_COLOR_RED));
    transition_control_panel_state(control_panel, R_SPECIFYING_FLIGHT_NUMBER, ERROR_STATE_TRANSITION_DELAY);
}

void handle_R_FLIGHT_NUMBER_NOT_FOUND_state(ControlPanel *control_panel)
{
    set_control_panel_display_text(control_panel, colorize_string("Número do vôo não encontrado. Tente novamente.\n", ANSI_COLOR_RED));
    transition_control_panel_state(control_panel, R_SPECIFYING_FLIGHT_NUMBER, ERROR_STATE_TRANSITION_DELAY);
}

/*
===================================== Finalizando remoção do vôo =================
*/
void handle_R_FINISH_REMOVING_FLIGHT_state(ControlPanel *control_panel)
{
    if (control_panel->state_data == NULL)
    {
        transition_control_panel_state(control_panel, IDLE, 0);
    }

    short flight_number = *(short *)control_panel->state_data;
    remove_flight(control_panel->board, flight_number);

    free(control_panel->state_data);
    control_panel->state_data = NULL;

    transition_control_panel_state(control_panel, IDLE, 0);
}

/*
===================================== Editando um vôo ============================
*/
void handle_EDITING_FLIGHT_state(ControlPanel *control_panel)
{
    if (control_panel->state_data != NULL)
    {
        free(control_panel->state_data);
        control_panel->state_data = NULL;
    }

    transition_control_panel_state(control_panel, E_SPECIFYING_FLIGHT_NUMBER, 0);
}

/*
===================================== Especificando número do vôo para editar ====
*/
void handle_E_SPECIFYING_FLIGHT_NUMBER_state_input(ControlPanel *control_panel, char input)
{
    if (input == ESC_KEY)
    {
        transition_control_panel_state(control_panel, IDLE, 0);
    }
    else if (input == ENTER_KEY)
    {
        short flight_number;
        if (!is_flight_number_valid(control_panel->input_buffer, &flight_number))
        {
            transition_control_panel_state(control_panel, E_INVALID_FLIGHT_NUMBER_SPECIFYING_FLIGHT_NUMBER, 0);
            return;
        }

        Flight flight;
        if (!flight_already_exists(control_panel->board, flight_number, &flight))
        {
            transition_control_panel_state(control_panel, E_FLIGHT_NUMBER_NOT_FOUND, 0);
            return;
        }

        control_panel->state_data = malloc(sizeof(short) + sizeof(Flight));
        memcpy(control_panel->state_data, &flight_number, sizeof(short));
        memcpy((char *)control_panel->state_data + sizeof(short), &flight, sizeof(Flight));

        transition_control_panel_state(control_panel, E_SELECTING_WHAT_TO_EDIT, 0);
    }
}

void handle_E_SPECIFYING_FLIGHT_NUMBER_state(ControlPanel *control_panel)
{
    char display_text[CONTROL_PANEL_DISPLAY_BUFFER_SIZE];
    snprintf(display_text, sizeof(display_text), "%s%s%s%s%s",
             colorize_string("Presione ", ANSI_COLOR_YELLOW), colorize_string("[ESC]", ANSI_COLOR_RED), colorize_string(" para cancelar a operação.\n", ANSI_COLOR_YELLOW),
             colorize_string("Digite o número do vôo: ", ANSI_COLOR_YELLOW),
             "%s_\n");

    set_control_panel_display_text(control_panel, format_string_va(display_text, control_panel->input_buffer));
    handle_control_panel_input(control_panel, handle_E_SPECIFYING_FLIGHT_NUMBER_state_input);
}

void handle_E_INVALID_FLIGHT_NUMBER_SPECIFYING_FLIGHT_NUMBER_state(ControlPanel *control_panel)
{
    set_control_panel_display_text(control_panel, colorize_string("Número do vôo inválido. Tente novamente.\n", ANSI_COLOR_RED));
    transition_control_panel_state(control_panel, E_SPECIFYING_FLIGHT_NUMBER, ERROR_STATE_TRANSITION_DELAY);
}

void handle_E_FLIGHT_NUMBER_NOT_FOUND_state(ControlPanel *control_panel)
{
    set_control_panel_display_text(control_panel, colorize_string("Número do vôo não encontrado. Tente novamente.\n", ANSI_COLOR_RED));
    transition_control_panel_state(control_panel, E_SPECIFYING_FLIGHT_NUMBER, ERROR_STATE_TRANSITION_DELAY);
}

/*
===================================== Selecionando o que editar ==================
*/
void handle_E_SELECTING_WHAT_TO_EDIT_state_input(ControlPanel *control_panel, char input)
{
    if (input == ESC_KEY)
    {
        transition_control_panel_state(control_panel, IDLE, 0);
    }
    else
    {
        switch (input)
        {
        case EDIT_FLIGHT_TIME_OF_DEPARTURE_KEY:
            transition_control_panel_state(control_panel, E_EDITING_FLIGHT_TIME_OF_DEPARTURE, 0);
            break;

        case EDIT_FLIGHT_NUMBER_KEY:
            transition_control_panel_state(control_panel, E_EDITING_FLIGHT_NUMBER, 0);
            break;

        case EDIT_FLIGHT_AIRLINE_KEY:
            transition_control_panel_state(control_panel, E_EDITING_FLIGHT_AIRLINE, 0);
            break;

        case EDIT_FLIGHT_DESTINATION_KEY:
            transition_control_panel_state(control_panel, E_EDITING_FLIGHT_DESTINATION, 0);
            break;

        case FINISH_EDITING_FLIGHT_KEY:
            transition_control_panel_state(control_panel, E_FINISH_EDITING_FLIGHT, 0);
            break;

        default:
            break;
        }
    }
}

void handle_E_SELECTING_WHAT_TO_EDIT_state(ControlPanel *control_panel)
{
    Flight *flight = (Flight *)((char *)control_panel->state_data + sizeof(short));
    if (flight == NULL)
    {
        transition_control_panel_state(control_panel, IDLE, 0);
    }

    char display_text[CONTROL_PANEL_DISPLAY_BUFFER_SIZE];
    snprintf(display_text, sizeof(display_text), "%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s",
             colorize_string("Presione ", ANSI_COLOR_YELLOW), colorize_string("[ESC]", ANSI_COLOR_RED), colorize_string(" para cancelar a operação.\n", ANSI_COLOR_YELLOW),
             colorize_string("Selecione o que deseja editar:\n", ANSI_COLOR_YELLOW),
             colorize_string("  [%c]", ANSI_COLOR_RED), " - Horário de saída\n",
             colorize_string("  [%c]", ANSI_COLOR_RED), " - Número do vôo\n",
             colorize_string("  [%c]", ANSI_COLOR_RED), " - Linha aérea\n",
             colorize_string("  [%c]", ANSI_COLOR_RED), " - Destino\n",
             colorize_string("  [%c]", ANSI_COLOR_RED), " - Finalizar edição\n\n",
             colorize_string("Novas informações do vôo:\n", ANSI_COLOR_YELLOW),
             colorize_string("  Horário de saída: ", ANSI_COLOR_YELLOW), colorize_string("%s\n", ANSI_COLOR_GREEN),
             colorize_string("  Número do vôo: ", ANSI_COLOR_YELLOW), colorize_string("%04d\n", ANSI_COLOR_GREEN),
             colorize_string("  Linha aérea: ", ANSI_COLOR_YELLOW), colorize_string("%s\n", ANSI_COLOR_GREEN),
             colorize_string("  Destino: ", ANSI_COLOR_YELLOW), colorize_string("%s\n", ANSI_COLOR_GREEN));

    set_control_panel_display_text(control_panel, format_string_va(display_text,
                                                                   EDIT_FLIGHT_TIME_OF_DEPARTURE_KEY,
                                                                   EDIT_FLIGHT_NUMBER_KEY,
                                                                   EDIT_FLIGHT_AIRLINE_KEY,
                                                                   EDIT_FLIGHT_DESTINATION_KEY,
                                                                   FINISH_EDITING_FLIGHT_KEY,
                                                                   unix_timestamp_to_hm_string((time_t)flight->time_of_departure),
                                                                   flight->flight_number,
                                                                   airline_to_string(flight->airline),
                                                                   destination_to_string(flight->destination)));
    handle_control_panel_input(control_panel, handle_E_SELECTING_WHAT_TO_EDIT_state_input);
}

/*
===================================== Editando horário de saída do vôo ===========
*/
void handle_E_EDITING_FLIGHT_TIME_OF_DEPARTURE_state_input(ControlPanel *control_panel, char input)
{
    if (input == ESC_KEY)
    {
        transition_control_panel_state(control_panel, E_SELECTING_WHAT_TO_EDIT, 0);
    }
    else if (input == ENTER_KEY)
    {
        long time_of_departure;
        if (!is_flight_time_of_departure_valid(control_panel->input_buffer, &time_of_departure))
        {
            transition_control_panel_state(control_panel, E_INVALID_FLIGHT_TIME_OF_DEPARTURE, 0);
            return;
        }

        Flight *flight = (Flight *)((char *)control_panel->state_data + sizeof(short));
        flight->time_of_departure = time_of_departure;

        transition_control_panel_state(control_panel, E_SELECTING_WHAT_TO_EDIT, 0);
    }
}

void handle_E_EDITING_FLIGHT_TIME_OF_DEPARTURE_state(ControlPanel *control_panel)
{
    char display_text[CONTROL_PANEL_DISPLAY_BUFFER_SIZE];
    snprintf(display_text, sizeof(display_text), "%s%s%s%s%s%s",
             colorize_string("Presione ", ANSI_COLOR_YELLOW), colorize_string("[ESC]", ANSI_COLOR_RED), colorize_string(" para cancelar a operação.\n", ANSI_COLOR_YELLOW),
             colorize_string("Digite o horário de saída do vôo ", ANSI_COLOR_YELLOW), "(HHMM): ",
             "%s_\n");

    set_control_panel_display_text(control_panel, format_string_va(display_text, control_panel->input_buffer));
    handle_control_panel_input(control_panel, handle_E_EDITING_FLIGHT_TIME_OF_DEPARTURE_state_input);
}

void handle_E_INVALID_FLIGHT_TIME_OF_DEPARTURE_state(ControlPanel *control_panel)
{
    set_control_panel_display_text(control_panel, colorize_string("Horário de saída do vôo inválido. Tente novamente.\n", ANSI_COLOR_RED));
    transition_control_panel_state(control_panel, E_EDITING_FLIGHT_TIME_OF_DEPARTURE, ERROR_STATE_TRANSITION_DELAY);
}

/*
===================================== Editando número do vôo =====================
*/
void handle_E_EDITING_FLIGHT_NUMBER_state_input(ControlPanel *control_panel, char input)
{
    if (input == ESC_KEY)
    {
        transition_control_panel_state(control_panel, E_SELECTING_WHAT_TO_EDIT, 0);
    }
    else if (input == ENTER_KEY)
    {
        short flight_number;
        if (!is_flight_number_valid(control_panel->input_buffer, &flight_number))
        {
            transition_control_panel_state(control_panel, E_INVALID_FLIGHT_NUMBER_EDITING_FLIGHT_NUMBER, 0);
            return;
        }

        if (flight_already_exists(control_panel->board, flight_number, NULL))
        {
            transition_control_panel_state(control_panel, E_FLIGHT_NUMBER_ALREADY_EXISTS, 0);
            return;
        }

        Flight *flight = (Flight *)((char *)control_panel->state_data + sizeof(short));
        flight->flight_number = flight_number;

        transition_control_panel_state(control_panel, E_SELECTING_WHAT_TO_EDIT, 0);
    }
}

void handle_E_EDITING_FLIGHT_NUMBER_state(ControlPanel *control_panel)
{
    char display_text[CONTROL_PANEL_DISPLAY_BUFFER_SIZE];
    snprintf(display_text, sizeof(display_text), "%s%s%s%s%s",
             colorize_string("Presione ", ANSI_COLOR_YELLOW), colorize_string("[ESC]", ANSI_COLOR_RED), colorize_string(" para cancelar a operação.\n", ANSI_COLOR_YELLOW),
             colorize_string("Digite o número do vôo: ", ANSI_COLOR_YELLOW),
             "%s_\n");

    set_control_panel_display_text(control_panel, format_string_va(display_text, control_panel->input_buffer));
    handle_control_panel_input(control_panel, handle_E_EDITING_FLIGHT_NUMBER_state_input);
}

void handle_E_INVALID_FLIGHT_NUMBER_EDITING_FLIGHT_NUMBER_state(ControlPanel *control_panel)
{
    set_control_panel_display_text(control_panel, colorize_string("Número do vôo inválido. Tente novamente.\n", ANSI_COLOR_RED));
    transition_control_panel_state(control_panel, E_EDITING_FLIGHT_NUMBER, ERROR_STATE_TRANSITION_DELAY);
}

void handle_E_FLIGHT_NUMBER_ALREADY_EXISTS_state(ControlPanel *control_panel)
{
    set_control_panel_display_text(control_panel, colorize_string("Número do vôo já existe. Tente novamente.\n", ANSI_COLOR_RED));
    transition_control_panel_state(control_panel, E_EDITING_FLIGHT_NUMBER, ERROR_STATE_TRANSITION_DELAY);
}

/*
===================================== Editando linha aérea do vôo ================
*/
void handle_E_EDITING_FLIGHT_AIRLINE_state_input(ControlPanel *control_panel, char input)
{
    if (input == ESC_KEY)
    {
        transition_control_panel_state(control_panel, E_SELECTING_WHAT_TO_EDIT, 0);
    }
    else if (input == ENTER_KEY)
    {
        Airline airline = string_to_airline(control_panel->input_buffer);
        if (airline == -1)
        {
            transition_control_panel_state(control_panel, E_INVALID_FLIGHT_AIRLINE, 0);
            return;
        }

        Flight *flight = (Flight *)((char *)control_panel->state_data + sizeof(short));
        flight->airline = airline;

        transition_control_panel_state(control_panel, E_SELECTING_WHAT_TO_EDIT, 0);
    }
}

void handle_E_EDITING_FLIGHT_AIRLINE_state(ControlPanel *control_panel)
{
    char display_text[CONTROL_PANEL_DISPLAY_BUFFER_SIZE];
    snprintf(display_text, sizeof(display_text), "%s%s%s%s%s%s%s%s",
             colorize_string("Presione ", ANSI_COLOR_YELLOW), colorize_string("[ESC]", ANSI_COLOR_RED), colorize_string(" para cancelar a operação.\n", ANSI_COLOR_YELLOW),
             colorize_string("Digite a linha aérea: ", ANSI_COLOR_YELLOW),
             "%s_\n",
             "\n",
             colorize_string("Linhas aéreas disponíveis:\n", ANSI_COLOR_YELLOW),
             colorize_string("%s\n", ANSI_COLOR_GREEN));

    set_control_panel_display_text(control_panel, format_string_va(display_text, control_panel->input_buffer, get_all_airlines_string()));
    handle_control_panel_input(control_panel, handle_E_EDITING_FLIGHT_AIRLINE_state_input);
}

void handle_E_INVALID_FLIGHT_AIRLINE_state(ControlPanel *control_panel)
{
    set_control_panel_display_text(control_panel, colorize_string("Linha aérea inválida. Tente novamente.\n", ANSI_COLOR_RED));
    transition_control_panel_state(control_panel, E_EDITING_FLIGHT_AIRLINE, ERROR_STATE_TRANSITION_DELAY);
}

/*
===================================== Editando destino do vôo ====================
*/
void handle_E_EDITING_FLIGHT_DESTINATION_state_input(ControlPanel *control_panel, char input)
{
    if (input == ESC_KEY)
    {
        transition_control_panel_state(control_panel, E_SELECTING_WHAT_TO_EDIT, 0);
    }
    else if (input == ENTER_KEY)
    {
        Destination destination = string_to_destination(control_panel->input_buffer);
        if (destination == -1)
        {
            transition_control_panel_state(control_panel, E_INVALID_FLIGHT_DESTINATION, 0);
            return;
        }

        Flight *flight = (Flight *)((char *)control_panel->state_data + sizeof(short));
        flight->destination = destination;

        transition_control_panel_state(control_panel, E_SELECTING_WHAT_TO_EDIT, 0);
    }
}

void handle_E_EDITING_FLIGHT_DESTINATION_state(ControlPanel *control_panel)
{
    char display_text[CONTROL_PANEL_DISPLAY_BUFFER_SIZE];
    snprintf(display_text, sizeof(display_text), "%s%s%s%s%s%s%s%s",
             colorize_string("Presione ", ANSI_COLOR_YELLOW), colorize_string("[ESC]", ANSI_COLOR_RED), colorize_string(" para cancelar a operação.\n", ANSI_COLOR_YELLOW),
             colorize_string("Digite o destino: ", ANSI_COLOR_YELLOW),
             "%s_\n",
             "\n",
             colorize_string("Destinos disponíveis:\n", ANSI_COLOR_YELLOW),
             colorize_string("%s\n", ANSI_COLOR_GREEN));

    set_control_panel_display_text(control_panel, format_string_va(display_text, control_panel->input_buffer, get_all_destinations_string()));
    handle_control_panel_input(control_panel, handle_E_EDITING_FLIGHT_DESTINATION_state_input);
}

void handle_E_INVALID_FLIGHT_DESTINATION_state(ControlPanel *control_panel)
{
    set_control_panel_display_text(control_panel, colorize_string("Destino inválido. Tente novamente.\n", ANSI_COLOR_RED));
    transition_control_panel_state(control_panel, E_EDITING_FLIGHT_DESTINATION, ERROR_STATE_TRANSITION_DELAY);
}

/*
===================================== Finalizando edição do vôo ================
*/
void handle_E_FINISH_EDITING_FLIGHT_state(ControlPanel *control_panel)
{
    if (control_panel->state_data == NULL)
    {
        transition_control_panel_state(control_panel, IDLE, 0);
    }

    short flight_number = *(short *)control_panel->state_data;
    Flight *flight = (Flight *)((char *)control_panel->state_data + sizeof(short));

    remove_flight(control_panel->board, flight_number);
    insert_flight(control_panel->board, flight);

    free(control_panel->state_data);
    control_panel->state_data = NULL;

    transition_control_panel_state(control_panel, IDLE, 0);
}

/*
==================================================================================
===================================== PROCEDIMENTOS ==============================
==================================================================================
*/

/**
 * Controla as interações do usuário com o placar de vôos.
 */
void handle_control_panel_input_proc(Board *board, ControlPanel *control_panel)
{
    if (get_unix_timestamp() < control_panel->state_transition_timestamp)
        return;

    switch (control_panel->state)
    {
    case IDLE:
        handle_IDLE_state(control_panel);
        break;

    case INSERTING_FLIGHT:
        handle_INSERTING_FLIGHT_state(control_panel);
        break;

    case I_INSERTING_FLIGHT_TIME_OF_DEPARTURE:
        handle_I_INSERTING_FLIGHT_TIME_OF_DEPARTURE_state(control_panel);
        break;

    case I_INVALID_FLIGHT_TIME_OF_DEPARTURE:
        handle_I_INVALID_FLIGHT_TIME_OF_DEPARTURE_state(control_panel);
        break;

    case I_INSERTING_FLIGHT_NUMBER:
        handle_I_INSERTING_FLIGHT_NUMBER_state(control_panel);
        break;

    case I_INVALID_FLIGHT_NUMBER:
        handle_I_INVALID_FLIGHT_NUMBER_state(control_panel);
        break;

    case I_FLIGHT_NUMBER_ALREADY_EXISTS:
        handle_I_FLIGHT_NUMBER_ALREADY_EXISTS_state(control_panel);
        break;

    case I_INSERTING_FLIGHT_AIRLINE:
        handle_I_INSERTING_FLIGHT_AIRLINE_state(control_panel);
        break;

    case I_INVALID_FLIGHT_AIRLINE:
        handle_I_INVALID_FLIGHT_AIRLINE_state(control_panel);
        break;

    case I_INSERTING_FLIGHT_DESTINATION:
        handle_I_INSERTING_FLIGHT_DESTINATION_state(control_panel);
        break;

    case I_INVALID_FLIGHT_DESTINATION:
        handle_I_INVALID_FLIGHT_DESTINATION_state(control_panel);
        break;

    case I_FINISH_INSERTING_FLIGHT:
        handle_I_FINISH_INSERTING_FLIGHT_state(control_panel);
        break;

    case REMOVING_FLIGHT:
        handle_REMOVING_FLIGHT_state(control_panel);
        break;

    case R_SPECIFYING_FLIGHT_NUMBER:
        handle_R_SPECIFYING_FLIGHT_NUMBER_state(control_panel);
        break;

    case R_INVALID_FLIGHT_NUMBER:
        handle_R_INVALID_FLIGHT_NUMBER_state(control_panel);
        break;

    case R_FLIGHT_NUMBER_NOT_FOUND:
        handle_R_FLIGHT_NUMBER_NOT_FOUND_state(control_panel);
        break;

    case R_FINISH_REMOVING_FLIGHT:
        handle_R_FINISH_REMOVING_FLIGHT_state(control_panel);
        break;

    case EDITING_FLIGHT:
        handle_EDITING_FLIGHT_state(control_panel);
        break;

    case E_SPECIFYING_FLIGHT_NUMBER:
        handle_E_SPECIFYING_FLIGHT_NUMBER_state(control_panel);
        break;

    case E_INVALID_FLIGHT_NUMBER_SPECIFYING_FLIGHT_NUMBER:
        handle_E_INVALID_FLIGHT_NUMBER_SPECIFYING_FLIGHT_NUMBER_state(control_panel);
        break;

    case E_FLIGHT_NUMBER_NOT_FOUND:
        handle_E_FLIGHT_NUMBER_NOT_FOUND_state(control_panel);
        break;

    case E_SELECTING_WHAT_TO_EDIT:
        handle_E_SELECTING_WHAT_TO_EDIT_state(control_panel);
        break;

    case E_EDITING_FLIGHT_TIME_OF_DEPARTURE:
        handle_E_EDITING_FLIGHT_TIME_OF_DEPARTURE_state(control_panel);
        break;

    case E_INVALID_FLIGHT_TIME_OF_DEPARTURE:
        handle_E_INVALID_FLIGHT_TIME_OF_DEPARTURE_state(control_panel);
        break;

    case E_EDITING_FLIGHT_NUMBER:
        handle_E_EDITING_FLIGHT_NUMBER_state(control_panel);
        break;

    case E_INVALID_FLIGHT_NUMBER_EDITING_FLIGHT_NUMBER:
        handle_E_INVALID_FLIGHT_NUMBER_EDITING_FLIGHT_NUMBER_state(control_panel);
        break;

    case E_FLIGHT_NUMBER_ALREADY_EXISTS:
        handle_E_FLIGHT_NUMBER_ALREADY_EXISTS_state(control_panel);
        break;

    case E_EDITING_FLIGHT_AIRLINE:
        handle_E_EDITING_FLIGHT_AIRLINE_state(control_panel);
        break;

    case E_INVALID_FLIGHT_AIRLINE:
        handle_E_INVALID_FLIGHT_AIRLINE_state(control_panel);
        break;

    case E_EDITING_FLIGHT_DESTINATION:
        handle_E_EDITING_FLIGHT_DESTINATION_state(control_panel);
        break;

    case E_INVALID_FLIGHT_DESTINATION:
        handle_E_INVALID_FLIGHT_DESTINATION_state(control_panel);
        break;

    case E_FINISH_EDITING_FLIGHT:
        handle_E_FINISH_EDITING_FLIGHT_state(control_panel);
        break;

    default:
        break;
    }
}

/**
 * Atualiza os vôos já existentes conforme o tempo passa.
 */
void handle_flights_update_proc(Board *board, ControlPanel *control_panel)
{
    Flight *current_flight = board->head;
    while (current_flight != NULL)
    {
        if (current_flight->flight_status == CANCELLED)
            continue;

        time_t now = get_unix_timestamp();
        if (now >= current_flight->time_of_departure)
        {
            if (now < current_flight->time_of_departure + current_flight->time_of_departure_random_offset)
                current_flight->flight_status = DELAYED;
            else
                current_flight->flight_status = DEPARTED;
        }

        current_flight = current_flight->next;
    }
}

/**
 * Realiza a exibição do placar de vôos e do estado atual das interações do usuário.
 */
void handle_terminal_view_update_proc(Board *board, ControlPanel *control_panel)
{
    clear_terminal();
    print_board(board);
    print_control_panel_interaction_state(control_panel);
}

int main()
{
    prepare_virtual_terminal_for_utf8();

    Board *board = new_board();
    ControlPanel *control_panel = new_control_panel(board);

    while (1)
    {
        handle_control_panel_input_proc(board, control_panel);
        handle_flights_update_proc(board, control_panel);
        handle_terminal_view_update_proc(board, control_panel);
        Sleep(1000 / 60); // 60 FPS
    }

    return 0;
}