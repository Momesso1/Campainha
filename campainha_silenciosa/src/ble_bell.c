#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/zbus/zbus.h>

LOG_MODULE_REGISTER(ble_bell, LOG_LEVEL_INF);

ZBUS_CHAN_DECLARE(ring_chan);
struct ring_msg { bool pressed; };

#define BT_UUID_BELL_SVC_VAL \
    BT_UUID_128_ENCODE(0x12345678, 0x1234, 0x5678, 0x1234, 0x56789abcdef0)
#define BT_UUID_BELL_RING_VAL \
    BT_UUID_128_ENCODE(0x12345678, 0x1234, 0x5678, 0x1234, 0x56789abcdef1)

static struct bt_uuid_128 bell_svc_uuid  = BT_UUID_INIT_128(BT_UUID_BELL_SVC_VAL);
static struct bt_uuid_128 bell_ring_uuid = BT_UUID_INIT_128(BT_UUID_BELL_RING_VAL);

static ssize_t on_ring_write(struct bt_conn *conn,
                             const struct bt_gatt_attr *attr,
                             const void *buf, uint16_t len,
                             uint16_t offset, uint8_t flags)
{
    LOG_INF("BLE: escrita recebida (%u bytes) -> publicando toque", len);
    struct ring_msg msg = { .pressed = true };
    zbus_chan_pub(&ring_chan, &msg, K_MSEC(100));
    return len;
}

BT_GATT_SERVICE_DEFINE(bell_svc,
    BT_GATT_PRIMARY_SERVICE(&bell_svc_uuid),
    BT_GATT_CHARACTERISTIC(&bell_ring_uuid.uuid,
        BT_GATT_CHRC_WRITE,
        BT_GATT_PERM_WRITE,
        NULL, on_ring_write, NULL),
);

static const struct bt_data ad[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    BT_DATA(BT_DATA_NAME_COMPLETE, "Campainha", sizeof("Campainha") - 1),
};

static const struct bt_data sd[] = {
    BT_DATA_BYTES(BT_DATA_UUID128_ALL, BT_UUID_BELL_SVC_VAL),
};

static void on_connected(struct bt_conn *conn, uint8_t err)
{
    if (err) { LOG_ERR("Falha ao conectar (err %u)", err); return; }
    LOG_INF("BLE: cliente conectado");
}

static void on_disconnected(struct bt_conn *conn, uint8_t reason)
{
    LOG_INF("BLE: cliente desconectado (reason %u), re-anunciando", reason);
    bt_le_adv_start(BT_LE_ADV_CONN_FAST_1, ad, ARRAY_SIZE(ad), sd, ARRAY_SIZE(sd));
}

BT_CONN_CB_DEFINE(conn_callbacks) = {
    .connected    = on_connected,
    .disconnected = on_disconnected,
};

static void bt_ready(int err)
{
    if (err) { LOG_ERR("Bluetooth init falhou (err %d)", err); return; }
    LOG_INF("Bluetooth inicializado");

    err = bt_le_adv_start(BT_LE_ADV_CONN_FAST_1,
                          ad, ARRAY_SIZE(ad),
                          sd, ARRAY_SIZE(sd));
    if (err) { LOG_ERR("Advertising falhou (err %d)", err); return; }
    LOG_INF("Advertising iniciado como \"Campainha\"");
}

static int ble_bell_init(void)
{
    int err = bt_enable(bt_ready);
    if (err) LOG_ERR("bt_enable falhou (err %d)", err);
    return err;
}
SYS_INIT(ble_bell_init, APPLICATION, 90);