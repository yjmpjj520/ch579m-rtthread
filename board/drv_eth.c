#include <rtthread.h>
#include <rtdevice.h>
#include "board.h"

#include "drv_eth.h"

#ifdef BSP_USING_ETH
#include <netif/ethernetif.h>
static struct eth_device eth_device;

//DMA�����ڴ���������4�ֽڶ���
__align(4) UINT8 eth_dma_tx_buf[ETH_BUF_SIZE];
__align(4) UINT8 eth_dma_rx_buf[ETH_BUF_SIZE];

UINT16 eth_rx_len = 0; //����״̬�ͳ���
UINT8 eth_rx_buf[ETH_BUF_SIZE]; //�м仺����

UINT8 eth_mac_addr[] = { 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };

static rt_err_t eth_init(rt_device_t dev)
{
    return RT_EOK;
}

static rt_err_t eth_open(rt_device_t dev, rt_uint16_t oflag)
{
    return RT_EOK;
}

static rt_err_t eth_close(rt_device_t dev)
{
    return RT_EOK;
}

static rt_size_t eth_read(rt_device_t dev, rt_off_t pos, void *buffer, rt_size_t size)
{
    rt_set_errno(-RT_ENOSYS);
    return 0;
}

static rt_size_t eth_write(rt_device_t dev, rt_off_t pos, const void *buffer, rt_size_t size)
{
    rt_set_errno(-RT_ENOSYS);
    return 0;
}

static rt_err_t eth_control(rt_device_t dev, int cmd, void *args)
{
    switch (cmd)
    {
        case NIOCTL_GADDR:

            /* get mac address */
            if (args) rt_memcpy(args, eth_mac_addr, 6);
            else return -RT_ERROR;

            break;

        default :
            break;
    }

    return RT_EOK;
}

rt_err_t eth_tx(rt_device_t dev, struct pbuf *p)
{
    struct pbuf *q;
    rt_uint32_t framelength = 0;
    rt_uint32_t bufferoffset = 0;
    rt_uint32_t byteslefttocopy = 0;
    rt_uint32_t payloadoffset = 0;

    //�ж�eth�Ƿ��ڷ���״̬
    if ((R8_ETH_ECON1 & RB_ETH_ECON1_TXRTS) != 0x00)
    {
        return ERR_INPROGRESS;
    }

    //ȷ���������Ƿ��㹻
    if (p->tot_len > sizeof(eth_dma_tx_buf))
    {
        return ERR_MEM;
    }
    
    for (q = p; q != NULL; q = q->next)
    {
        /* Get bytes in current lwIP buffer */
        byteslefttocopy = q->len; 
        payloadoffset = 0;
        SMEMCPY((uint8_t *)((uint8_t *)eth_dma_tx_buf + bufferoffset), (uint8_t *)((uint8_t *)q->payload + payloadoffset), byteslefttocopy);
        bufferoffset = bufferoffset + byteslefttocopy;
        framelength = framelength + byteslefttocopy;
    }
    //rt_kprintf("byteslefttocopy=%d \n",framelength);   
    R8_ETH_ECON1 |= RB_ETH_ECON1_TXRTS;     //��ʼ����

    return ERR_OK;
}

struct pbuf *eth_rx(rt_device_t dev)
{
    struct pbuf *p = NULL;

    //����Ƿ�������
    if (eth_rx_len == 0)
    {
        return NULL;
    }

    p = pbuf_alloc(PBUF_RAW, eth_rx_len, PBUF_POOL);

    if (p == NULL)
    {
        rt_kprintf("eth_rx: pbuf_alloc failed\n");
        eth_rx_len = 0;
        return NULL;
    }

    //�������ݵ�pbuf
    rt_memcpy((uint8_t *)((uint8_t *)p->payload), (uint8_t *)((uint8_t *)eth_rx_buf), eth_rx_len);
    //�ָ�״̬
    eth_rx_len = 0;

    return p;
}

int read_eth_link_status()
{
    R8_ETH_MIREGADR = 0x01;//״̬�Ĵ���
    R8_ETH_MISTAT |= 0x00; //��MII�Ĵ���

    //��ȡlink״̬
    if ((R16_ETH_MIRD & 0x04) != 0)
    {
        return 1; //�Ѳ���
    }

    return 0;
}

void ETH_IRQHandler(void) /* ��̫���ж� */
{
    rt_interrupt_enter();

    //���յ����ݰ�
    if ((R8_ETH_EIR & RB_ETH_EIR_RXIF) != 0)
    {
        //�жϻ������Ƿ�������
        if (eth_rx_len == 0)
        {
            rt_memcpy(eth_rx_buf, eth_dma_rx_buf, R16_ETH_ERXLN);
            eth_rx_len = R16_ETH_ERXLN;
            //֪ͨ������
            eth_device_ready(&eth_device);
        }

        R8_ETH_EIR |= RB_ETH_EIR_RXIF; //����ж�
    }

    //���մ���
    if ((R8_ETH_EIR & RB_ETH_EIE_RXERIE) != 0)
    {
        R8_ETH_EIR |= RB_ETH_EIE_RXERIE; //����ж�
    }

    //�������
    if ((R8_ETH_EIR & RB_ETH_EIR_TXIF) != 0)
    {
        R8_ETH_EIR |= RB_ETH_EIR_TXIF; //����ж�
    }

    //���ʹ���
    if ((R8_ETH_EIR & RB_ETH_EIE_TXERIE) != 0)
    {
        R8_ETH_EIR |= RB_ETH_EIE_TXERIE; //����ж�
    }

    //Link �仯��־
    if ((R8_ETH_EIR & RB_ETH_EIR_LINKIF) != 0)
    {
        //��ȡ����״̬
        if (read_eth_link_status())
        {
            eth_device_linkchange(&eth_device, RT_TRUE);
            rt_kprintf("eth1: link is up\n");
        }
        else
        {
            eth_device_linkchange(&eth_device, RT_FALSE);
            rt_kprintf("eth1: link is down\n");
        }

        R8_ETH_EIR |= RB_ETH_EIR_LINKIF; //����ж�
    }

    rt_interrupt_leave();
}

int rt_hw_eth_init(void)
{
    //ʹ��ETH����
    R16_PIN_ANALOG_IE |= RB_PIN_ETH_IE;

    //���밲ȫ����ģʽ
    R8_SAFE_ACCESS_SIG = 0x57;
    R8_SAFE_ACCESS_SIG = 0xA8;
    //����̫��ʱ��
    R8_SLP_CLK_OFF1 &= (~RB_SLP_CLK_ETH);
    //����̫����Դ
    R8_SLP_POWER_CTRL &= (~RB_SLP_ETH_PWR_DN);
    //�˳���ȫ����ģʽ
    R8_SAFE_ACCESS_SIG = 0x00;

    //������̫���ж�
    R8_ETH_EIE |= RB_ETH_EIE_INTIE;
    //������̫�������ж�
    R8_ETH_EIE |= RB_ETH_EIE_RXIE;
    //R8_ETH_EIE |= RB_ETH_EIE_RXERIE;
    //������̫�������ж�
    R8_ETH_EIE |= RB_ETH_EIR_TXIF;
    R8_ETH_EIE |= RB_ETH_EIR_TXERIF;
    //����Link�仯�ж�
    R8_ETH_EIE |= RB_ETH_EIE_LINKIE;
    //�������õ�50ŷķ�迹ƥ�����
    R8_ETH_EIE |= RB_ETH_EIE_R_EN50;

    //���ý��չ���ģʽ
    R8_ETH_ERXFCON = RB_ETH_ERXFCON_ANDOR | RB_ETH_ERXFCON_CRCEN;

    //���÷���dma
    R16_ETH_ETXST = (uint32_t)eth_dma_tx_buf;
    //���ý���dma
    R16_ETH_ERXST = (uint32_t)eth_dma_rx_buf;
    //���������ճ���
    R16_ETH_MAMXFL = sizeof(eth_dma_rx_buf);

    //ʹ��MAC�����
    R8_ETH_MACON1 |= RB_ETH_MACON1_MARXEN;
    //����Ӳ��CRC
    R8_ETH_MACON2 |= RB_ETH_MACON2_TXCRCEN;
    //���ж̰����0��60�ֽڣ���4�ֽ� CRC
    R8_ETH_MACON2 |= 0x20;
    //ʹ�ܽ���
    R8_ETH_ECON1 |= RB_ETH_ECON1_RXEN;

    //�����ж�
    NVIC_EnableIRQ(ETH_IRQn);

    //���ûص�����
    eth_device.parent.init       = eth_init;
    eth_device.parent.open       = eth_open;
    eth_device.parent.close      = eth_close;
    eth_device.parent.read       = eth_read;
    eth_device.parent.write      = eth_write;
    eth_device.parent.control    = eth_control;
    eth_device.parent.user_data  = RT_NULL;
    eth_device.eth_rx            = eth_rx;
    eth_device.eth_tx            = eth_tx;

    return eth_device_init(&(eth_device), "e0");
}
INIT_DEVICE_EXPORT(rt_hw_eth_init);
#endif /* BSP_USING_ETH */
