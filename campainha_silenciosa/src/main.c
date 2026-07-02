#include <zephyr/kernel.h>                                      // Inclui a API principal do kernel do Zephyr (threads, delays, locks)
#include <zephyr/logging/log.h>                                 // Inclui a biblioteca de logs para imprimir mensagens no terminal
#include <zephyr/shell/shell.h>                                 // Inclui suporte a comandos de terminal (shell) customizados
#include <zephyr/zbus/zbus.h>                                   // Inclui a API do ZBUS (barramento interno de mensagens/eventos)
#include <zephyr/settings/settings.h>                           // Inclui o subsistema para salvar/carregar dados da memória flash
#include <zephyr/drivers/gpio.h>                                // Inclui a API de controle dos pinos de entrada/saída (GPIO)

/* ---- WS2812 bit-bang no GPIO8 ---- */                        // Comentário original: Marca o início do driver do LED WS2812
#define GPIO_OUT_W1TS (*(volatile uint32_t *)0x60004008)        // Define um ponteiro direto na memória para ligar o pino (nível ALTO)
#define GPIO_OUT_W1TC (*(volatile uint32_t *)0x6000400C)        // Define um ponteiro direto na memória para desligar o pino (nível BAIXO)
#define PIN8 (1U << 8)                                          // Define a máscara de bits (deslocamento) correspondente ao pino GPIO 8

static const struct device *gpio0_dev = DEVICE_DT_GET(DT_NODELABEL(gpio0)); // Obtém a estrutura e referência de hardware da porta GPIO 0

static inline void pin_high(void) { GPIO_OUT_W1TS = PIN8; }     // Função rápida (inline) para forçar o GPIO 8 para nível lógico ALTO
static inline void pin_low(void)  { GPIO_OUT_W1TC = PIN8; }     // Função rápida (inline) para forçar o GPIO 8 para nível lógico BAIXO

static __attribute__((noinline)) void nop_delay(volatile uint32_t n) // Função que não pode ser otimizada (noinline) para gerar delays de hardware
{                                                               // Início da função de atraso em ciclos
    while (n--) { __asm__ volatile ("nop"); }                   // Loop executando instrução "vazia" (nop) em assembly para atraso exato
}                                                               // Fim da função de atraso em ciclos

static void ws2812_send_byte(uint8_t b)                         // Função que envia 1 byte (8 bits) de dados para o LED WS2812
{                                                               // Início da função de envio de byte
    for (int i = 7; i >= 0; i--) {                              // Loop que percorre cada bit do byte, do mais significativo para o menos
        if (b & (1 << i)) {                                     // Verifica com máscara bit-a-bit se o bit atual é 1
            pin_high(); nop_delay(30);                          // Se for 1: deixa o pino em ALTO por mais tempo (padrão WS2812 para bit 1)
            pin_low();  nop_delay(5);                           // Em seguida, deixa em BAIXO por um tempo bem curto
        } else {                                                // Se o bit atual for 0
            pin_high(); nop_delay(3);                           // Se for 0: deixa o pino em ALTO por um tempo bem curto (padrão WS2812 para bit 0)
            pin_low();  nop_delay(30);                          // Em seguida, deixa em BAIXO por mais tempo
        }                                                       // Fim do bloco if-else
    }                                                           // Fim do loop for
}                                                               // Fim da função de envio de byte

static void ws2812_set(uint8_t r, uint8_t g, uint8_t b)         // Função para definir a cor do LED usando valores Vermelho, Verde e Azul
{                                                               // Início da função ws2812_set
    unsigned int key = irq_lock();                              // Bloqueia as interrupções globais para evitar falhas no tempo do bit-bang
    ws2812_send_byte(g);                                        // Envia o byte da cor Verde (no WS2812, o padrão de envio é GRB e não RGB)
    ws2812_send_byte(r);                                        // Envia o byte da cor Vermelha
    ws2812_send_byte(b);                                        // Envia o byte da cor Azul
    irq_unlock(key);                                            // Restaura as interrupções globais de volta ao normal
    k_usleep(80);                                               // Pausa de 80 microsegundos para o LED registrar as cores e acender (LATCH)
}                                                               // Fim da função ws2812_set

static void led_acender(void)  
{ 
  ws2812_set(255, 0, 0); // Função atalho (wrapper) para acender o LED na cor vermelha (R=255)
}       
static void led_apagar(void)   
{ 
  ws2812_set(0, 0, 0); // Função atalho (wrapper) para apagar o LED (todos os valores em 0)   
}       

/* ---- fim WS2812 ---- */                                      // Comentário original: Fim do driver do LED

LOG_MODULE_REGISTER(campainha, LOG_LEVEL_INF);                  // Registra o módulo atual no sistema de log como "campainha" com nível INFO

bool bell_active;                                               // Variável global que armazena se a campainha está tocando ou não
bool do_not_disturb;                                            // Variável global que armazena se o modo "Não Perturbe" está ligado

struct ring_msg {                                               // Define a estrutura de dados da mensagem de "toque" da campainha
  bool pressed;                                                 // Variável interna da estrutura que diz se o botão foi pressionado
};                                                              // Fim da estrutura

ZBUS_CHAN_DEFINE(ring_chan,                                     // Criação de um canal ZBUS (barramento) chamado 'ring_chan'
     struct ring_msg,                                           // Define que o canal vai transmitir mensagens do tipo 'struct ring_msg'
     NULL,                                                      // Sem função validadora de mensagem
     NULL,                                                      // Sem dados customizados de usuário
     ZBUS_OBSERVERS_EMPTY,                                      // Inicia a lista de observadores/ouvintes vazia
     ZBUS_MSG_INIT(.pressed = false));                          // Inicializa o estado padrão da mensagem no canal como não pressionado (false)

static void ring_listener_cb(const struct zbus_channel *chan)   // Função de callback (gatilho) acionada quando uma mensagem chega no 'ring_chan'
{                                                               // Início da função de callback de toque
  const struct ring_msg *msg = zbus_chan_const_msg(chan);       // Extrai a mensagem recebida de dentro do canal ZBUS
  bell_active = msg->pressed;                                   // Atualiza a variável global com o status vindo da mensagem

  if (bell_active)                                              // Se a campainha foi ativada (pressionada)
  {                                                             // Início do if de ativação
    if(do_not_disturb == false)                                 // Verifica se o modo "Não Perturbe" está desativado (falso)
    {                                                           // Início if "Não Perturbe desativado"
      LOG_INF("Campainha acionada (bell_active=%d)", bell_active); // Exibe no terminal (log) que a campainha tocou
      LOG_INF("LED: ACESO");                                    // Exibe no terminal (log) que o LED vai acender
      led_acender();                                            // Chama a função para acender o LED (vermelho)
    }                                                           // Fim if "Não Perturbe desativado"
    else                                                        // Caso o modo "Não Perturbe" esteja ativado (true)
    {                                                           // Início else
      LOG_INF("Modo não perturbe está ativado");                // Exibe no terminal que o toque foi ignorado devido ao modo silencioso
    }                                                           // Fim else
  }                                                             // Fim do if de ativação

  settings_save_one("campainha/bell", &bell_active,             // Salva de forma permanente (na flash) o status atual da variável 'bell_active'
                    sizeof(bell_active));                       // Informa o tamanho em bytes da variável que será salva
}                                                               // Fim da função de callback de toque

ZBUS_LISTENER_DEFINE(ring_listener, ring_listener_cb);          // Define o ouvinte 'ring_listener', ligando-o à função de callback recém-criada
ZBUS_CHAN_ADD_OBS(ring_chan, ring_listener, 0);                 // Inscreve formalmente o ouvinte no canal 'ring_chan' para escutar os eventos

static int campainha_settings_set(const char *name, size_t len, // Função acionada durante o boot para recuperar as configurações salvas da flash
                                  settings_read_cb read_cb,     // Ponteiro de função do próprio Zephyr que realiza a leitura
                                  void *cb_arg)                 // Argumento auxiliar para a função de leitura
{                                                               // Início da função de configurações
  if (settings_name_steq(name, "bell", NULL)) {                 // Verifica se o dado sendo recuperado corresponde ao nome "bell"
    if (len != sizeof(bell_active)) {                           // Verifica se o tamanho do dado salvo é igual ao tamanho esperado
      return -EINVAL;                                           // Se o tamanho for diferente (corrompido/errado), retorna erro de parâmetro inválido
    }                                                           // Fim da verificação de tamanho
    if (read_cb(cb_arg, &bell_active, sizeof(bell_active)) < 0) // Executa a leitura física da memória gravando o resultado em 'bell_active'
    {                                                           // Início checagem erro de leitura
      return -EIO;                                              // Se a leitura falhar, retorna erro de Input/Output (EIO)
    }                                                           // Fim checagem erro de leitura
    return 0;                                                   // Retorna 0 (sucesso), indicando que a variável foi recuperada e atualizada
  }                                                             // Fim da verificação do nome da chave
  return -ENOENT;                                               // Retorna erro informando que a chave solicitada não foi encontrada
}                                                               // Fim da função de configurações

SETTINGS_STATIC_HANDLER_DEFINE(campainha, "campainha", NULL,    // Registra estaticamente a estrutura de recuperação de dados sob o nó "campainha"
                               campainha_settings_set,          // Associa a função que restaura os valores
                               NULL, NULL);                     // Ignora callbacks auxiliares (commit e export) deixando-os como nulos

static int cmd_ring(const struct shell *sh, size_t argc,        // Função para criar o comando de teste manual no terminal/shell interativo
                    char **argv)                                // Argumentos de texto passados na linha de comando
{                                                               // Início da função shell
  struct ring_msg msg = { .pressed = true };                    // Cria uma mensagem temporária forçando o estado "pressionado"
  
  shell_print(sh, "Publicando toque no ring_chan...");          // Imprime mensagem de feedback direto na tela do shell
  
  return zbus_chan_pub(&ring_chan, &msg, K_MSEC(100));          // Publica a mensagem criada no 'ring_chan' (timeout máximo de 100 milissegundos)
}                                                               // Fim da função shell

SHELL_CMD_REGISTER(ring, NULL,                                  // Registra um comando oficial de shell com o nome "ring"
                   "Simula o botao BLE da campainha",           // Texto de ajuda/descrição que aparece quando o usuário digita 'help' no terminal
                   cmd_ring);                                   // Aponta qual função C deve rodar quando "ring" for digitado

ZBUS_CHAN_DECLARE(btn_chan);                                    // Declara a existência de um canal 'btn_chan' que foi criado em outro arquivo

struct btn_evt                                                  // Define a estrutura de dados para o evento de um botão físico sendo apertado
{                                                               // Início da estrutura
  bool long_press;                                              // Booleano indicando se o usuário manteve o botão pressionado
  int64_t ms;                                                   // Guarda a quantidade exata de milissegundos que durou a pressão no botão
};                                                              // Fim da estrutura

static void btn_cb(const struct zbus_channel *chan)             // Função de callback (gatilho) quando alguém aperta o botão físico
{                                                               // Início da função de callback de botão
  const struct btn_evt *e = zbus_chan_const_msg(chan);          // Extrai o conteúdo da mensagem recebida do canal de botões

  if (e->long_press)                                            // Verifica se o evento extraído marca um clique LONGO
  {                                                             // Início if clique longo
    LOG_INF("main: recebi LONGO, alternando modo DND");         // Log indicando que o modo Não Perturbe será alternado
    if(do_not_disturb == true)                                  // Se o Não Perturbe já estava ativado
    {                                                           // Início if
      do_not_disturb = false;                                   // Desativa o modo Não Perturbe
    }                                                           // Fim if
    else                                                        // Caso o Não Perturbe estivesse desligado
    {                                                           // Início else
      do_not_disturb = true;                                    // Ativa o modo Não Perturbe
    }                                                           // Fim else
  }                                                             // Fim if clique longo
  else                                                          // Se o clique extraído não foi longo (foi um clique CURTO)
  {                                                             // Início else clique curto
    if(bell_active == true)                                     // Verifica se a campainha encontra-se ativada/tocando
    {                                                           // Início if
      LOG_INF("main: recebi CURTO, silenciando alerta");        // Registra no log que o alerta do LED/som será interrompido
      LOG_INF("LED: APAGADO");                                  // Log avisando do desligamento visual
      led_apagar();                                             // Manda o sinal WS2812 para apagar o LED
      bell_active = false;                                      // Atualiza a flag global, indicando que a campainha agora está desligada
    }                                                           // Fim if
    else                                                        // Caso houvesse um clique curto e a campainha já estivesse desligada
    {                                                           // Início else
      LOG_INF("A campainha já foi desativada.");                // Apenas exibe no log que não há nada a fazer
    }                                                           // Fim else
  }                                                             // Fim else clique curto
}                                                               // Fim da função de callback de botão

ZBUS_LISTENER_DEFINE(main_btn_listener, btn_cb);                // Define o ouvinte 'main_btn_listener', associando-o ao callback do botão físico
ZBUS_CHAN_ADD_OBS(btn_chan, main_btn_listener, 0);              // Inscreve este ouvinte no canal de botões 'btn_chan'

int main(void)                                                  // Função principal de entrada do software (main thread)
{                                                               // Início da main
  LOG_INF("Campainha silenciosa iniciada");                     // Log inicial de sistema marcando o boot do firmware

  if (!device_is_ready(gpio0_dev)) {                            // Verifica se o hardware responsável pela porta GPIO0 foi devidamente inicializado
      LOG_ERR("GPIO nao esta pronto");                          // Caso não esteja pronto, dispara um log de erro crítico
      return -1;                                                // Encerra a função principal prematuramente devolvendo um código de erro genérico
  }                                                             // Fim da checagem do hardware GPIO
  gpio_pin_configure(gpio0_dev, 8, GPIO_OUTPUT_LOW);            // Configura o pino GPIO 8 especificamente como pino de Saída (Output), iniciando em Nível Baixo (Low)
  led_apagar();                                                 // Força o envio do pacote que apaga o LED WS2812, limpando eventuais luzes remanescentes após reset

  settings_subsys_init();                                       // Inicializa formalmente o subsistema do Zephyr de salvar/carregar dados
  settings_load();                                              // Lê fisicamente a memória flash e puxa todas as variáveis salvas anteriormente (como 'bell_active')

  LOG_INF("Estado restaurado: bell_active=%d", bell_active);    // Imprime qual foi o último estado que estava salvo na memória ao religar o dispositivo

  return 0;                                                     // Finaliza a função main com sucesso (callbacks, interrupções e canais ZBUS continuam operando em background)
}                                                               // Fim da main