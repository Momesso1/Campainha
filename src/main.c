#include <zephyr/kernel.h>           // API principal do kernel Zephyr (threads, K_MSEC, etc.).
                                     // (Include redundante/legado foi mantido como estava).
#include <zephyr/logging/log.h>      // Subsistema de logging (LOG_INF, LOG_ERR, etc.).
#include <zephyr/shell/shell.h>      // Shell interativo via UART/USB p/ comandos de debug.
#include <zephyr/zbus/zbus.h>        // Barramento zbus: comunicação pub/sub desacoplada.
#include <zephyr/settings/settings.h>// Persistência chave/valor em flash (NVS) pós-reboot.

LOG_MODULE_REGISTER(campainha, LOG_LEVEL_INF); // Registra este arquivo como um "módulo" de log
                                               // chamado "campainha" (INFO). Sem isso, as macros
                                               // LOG_INF/LOG_ERR não compilam neste arquivo.

/* Estado da campainha: true = acionada */
bool bell_active;                       // Guarda estado atual. 'static' limita a este .c.
bool do_not_disturb;                           // Variável para controle do modo não perturbe.

/* Mensagem que trafega no zbus quando a campainha e' acionada */
struct ring_msg {                              // "Payload" publicado no canal zbus. Mantido
  bool pressed;                                // simples (fase inicial): true ou false apenas.
};

/* Canal zbus: a entrada publica aqui; o listener abaixo consome e aciona o LED. */
ZBUS_CHAN_DEFINE(ring_chan,                    // Macro que define um canal zbus "ring_chan".
     struct ring_msg,                          // Tipo da mensagem que esse canal carrega.
     NULL,                                     // Função validadora (NULL = aceita qualquer msg).
     NULL,                                     // Ponteiro de user_data (não usado aqui).
     ZBUS_OBSERVERS_EMPTY,                     // Lista inicial de observadores vazia.
     ZBUS_MSG_INIT(.pressed = false));         // Valor inicial da mensagem armazenada no canal.

/* --- Listener: reage ao toque --- */
static void ring_listener_cb(const struct zbus_channel *chan) // Callback chamado automaticamente 
                                                              // ao publicarem num canal observado.
{
  const struct ring_msg *msg = zbus_chan_const_msg(chan);     // Obtém ponteiro const p/ a msg.
                                                              // (O listener NÃO altera o canal).

  bell_active = msg->pressed;                                 // Atualiza o estado global com a msg.

  

  if (bell_active) 
  {
    /* TODO hardware: gpio_pin_set_dt(&led, 1); */            // Placeholder: quando houver
		
		
		if(do_not_disturb == false)
		{
			LOG_INF("Campainha acionada (bell_active=%d)", bell_active);// Log informativo na UART/USB.
			LOG_INF("LED: ACESO");                                    // hardware, aciona via GPIO.
		}
		else
		{
			LOG_INF("Modo não perturbe está ativado");
		}
	
  } 


  /* Persiste o ultimo estado via settings */
  settings_save_one("campainha/bell", &bell_active,           // Grava em flash a chave com o valor.
                    sizeof(bell_active));                     // Sobrevive a reset/desligamento e é
                                                              // restaurada no próximo boot.
}

ZBUS_LISTENER_DEFINE(ring_listener, ring_listener_cb);        // Define observador tipo "listener"
                                                              // síncrono, vinculado ao callback.

ZBUS_CHAN_ADD_OBS(ring_chan, ring_listener, 0);               // Inscreve ring_listener em ring_chan
                                                              // c/ prioridade 0. Publicações disparam.

/* --- Settings: carrega o estado salvo no boot --- */
static int campainha_settings_set(const char *name, size_t len, // Handler do settings_load() para 
                                  settings_read_cb read_cb,     // CADA chave com prefixo "campainha".
                                  void *cb_arg)                 // name é o final ("bell").
{
  if (settings_name_steq(name, "bell", NULL)) {                 // Compara nome restante com "bell".
    if (len != sizeof(bell_active)) {                           // Se o tamanho for inesperado, o
      return -EINVAL;                                           // dado está incompatível (Erro).
    }

    if (read_cb(cb_arg, &bell_active, sizeof(bell_active)) < 0) // read_cb lê do storage. Se a leitura
    {                                                           // falhar, retorna erro de I/O.
      return -EIO;
    }

    return 0;                                                   // Sucesso: valor foi restaurado.
  }

  return -ENOENT;                                               // Nome não reconhecido por este handler.
}

SETTINGS_STATIC_HANDLER_DEFINE(campainha, "campainha", NULL,    // Registra o handler estaticamente:
                               campainha_settings_set,          // nome "campainha" e prefixo associado,
                               NULL, NULL);                     // usando o 'set' definido acima.



/* --- Shell: simula o botao BLE enquanto nao ha hardware --- */
static int cmd_ring(const struct shell *sh, size_t argc,        // Função do comando de shell "ring".
                    char **argv)                                // argc/argv não são usados aqui.
{
  struct ring_msg msg = { .pressed = true };                    // Monta a mensagem de toque (true).

  shell_print(sh, "Publicando toque no ring_chan...");          // Imprime feedback no terminal.

  return zbus_chan_pub(&ring_chan, &msg, K_MSEC(100));          // Publica msg no canal (timeout 100ms).
                                                                // Retorna 0 em sucesso (negativo=erro).
}

SHELL_CMD_REGISTER(ring, NULL,                                  // Registra o comando "ring" no shell,
                   "Simula o botao BLE da campainha",           // sem subcomandos, com string de
                   cmd_ring);                                   // help e callback cmd_ring.


/* BOTÃO */

ZBUS_CHAN_DECLARE(btn_chan);                                    /* declara que canal existe em outro .c */

struct btn_evt 
{ 
  bool long_press;
  int64_t ms; 
};

static void btn_cb(const struct zbus_channel *chan)
{
  const struct btn_evt *e = zbus_chan_const_msg(chan);
	
  if (e->long_press) 
  {
    LOG_INF("main: recebi LONGO, alternando modo DND");
  
    if(do_not_disturb == true)
    {
      do_not_disturb = false;                                   // Alterna o estado do modo
    }                                                           // "Não Perturbe" (DND).
    else
    {
      do_not_disturb = true;
    }
  } 
  else 
  {
		if(bell_active == true)
		{
				LOG_INF("main: recebi CURTO, silenciando alerta");
			LOG_INF("LED: APAGADO");                                  // Se pressed=false, sinaliza
																															// apagamento. Hoje só log.
			bell_active = false;
		}
		else
		{
				LOG_INF("A campainha já foi desativada.");
		}
  
  }
}

ZBUS_LISTENER_DEFINE(main_btn_listener, btn_cb);
ZBUS_CHAN_ADD_OBS(btn_chan, main_btn_listener, 0);


/* --- main --- */
int main(void)
{
  LOG_INF("Campainha silenciosa iniciada");                     // Marca boot no log (firmware subiu).

  settings_subsys_init();                                       // Inicializa subsistema settings NVS.
                                                                // Sem isso, leitura e gravação falham.

  settings_load();                                              // Lê a flash e dispara os handlers.
                                                                // bell_active é restaurado aqui.

  LOG_INF("Estado restaurado: bell_active=%d", bell_active);    // Confirma no log o estado recuperado.

  return 0;                                                     // main retorna; kernel continua rodando.
}