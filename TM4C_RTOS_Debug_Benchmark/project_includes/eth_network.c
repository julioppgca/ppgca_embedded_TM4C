/*
 *    ======== eth_network.c ========
 *    Contains non-BSD sockets code (NDK Network Open Hook)
 */

#include "project_includes/eth_network.h"
#include <string.h>

/* string to store result values */
char g_str_SendResult[SEND_PACKET_SIZE];
char g_str_SendResult2[SEND_PACKET_SIZE];

/*
 *  ======== tcpWorker ========
 *  Task to handle TCP connection. Can be multiple Tasks running
 *  this function.
 */
Void tcpWorker(UArg arg0, UArg arg1)
{
    int  clientfd = (int)arg0;
    char buffer[TCPPACKETSIZE];

    System_printf("tcpWorker: start clientfd = 0x%x\n", clientfd);

    recv(clientfd, buffer, TCPPACKETSIZE, 0);


    switch (buffer[0])
        {
        case '?': // Help string
        {
            send(clientfd, HELP, sizeof(HELP), 0);
            break;
        }
        case 'R': // rms value request
                {
                    // block access of g_str_SendResult while sending the value
                    Semaphore_pend(s_critical_section, BIOS_WAIT_FOREVER);
                        send(clientfd, g_str_SendResult, sizeof(g_str_SendResult), 0);
                    Semaphore_post(s_critical_section); // release g_str_SendResult variable
                    break;
                }
        case 'F': // fft value request
        {
            // block access of g_str_SendResult while sending the value
            Semaphore_pend(s_critical_section2, BIOS_WAIT_FOREVER);
            send(clientfd, g_str_SendResult2, sizeof(g_str_SendResult2), 0);
            Semaphore_post(s_critical_section2); // release g_str_SendResult variable
            break;
        }
        case 'U': // firmaware update request (see file USB_Serial_DFU.pdf pg.:36 to generate .bin file )
        {
            const char UpdateRequest[] = "Firmware Update Request. Reseting...";
            send(clientfd, UpdateRequest, sizeof(UpdateRequest), 0);

            //
            // Disable all processor interrupts.  Instead of disabling them
            // one at a time (and possibly missing an interrupt if new sources
            // are added), a direct write to NVIC is done to disable all
            // peripheral interrupts.
            //
            HWREG (NVIC_DIS0) = 0xffffffff;
            HWREG (NVIC_DIS1) = 0xffffffff;
            HWREG (NVIC_DIS2) = 0xffffffff;
            HWREG (NVIC_DIS3) = 0xffffffff;
            HWREG (NVIC_DIS4) = 0xffffffff;

            // Also disable the SysTick interrupt.
            SysTickIntDisable();
            SysTickDisable();

            // Return control to the boot loader.
            ROM_UpdateEMAC(120000000);

            break;
        }
        default:
        {
            send(clientfd, "Unknown Command", 15, 0);
            break;
        }

        }

    System_printf("tcpWorker stop clientfd = 0x%x\n", clientfd);

    close(clientfd);
}

/*
 *  ======== tcpHandler ========
 *  Creates new Task to handle new TCP connections.
 */
Void tcpHandler(UArg arg0, UArg arg1)
{
    int                status;
    int                clientfd;
    int                server;
    struct sockaddr_in localAddr;
    struct sockaddr_in clientAddr;
    int                optval;
    int                optlen = sizeof(optval);
    socklen_t          addrlen = sizeof(clientAddr);
    Task_Handle        taskHandle;
    Task_Params        taskParams;
    Error_Block        eb;

    server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server == -1) {
        System_printf("Error: socket not created.\n");
        goto shutdown;
    }


    memset(&localAddr, 0, sizeof(localAddr));
    localAddr.sin_family = AF_INET;
    localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    localAddr.sin_port = htons(arg0);

    status = bind(server, (struct sockaddr *)&localAddr, sizeof(localAddr));
    if (status == -1) {
        System_printf("Error: bind failed.\n");
        goto shutdown;
    }

    status = listen(server, NUMTCPWORKERS);
    if (status == -1) {
        System_printf("Error: listen failed.\n");
        goto shutdown;
    }

    optval = 1;
    if (setsockopt(server, SOL_SOCKET, SO_KEEPALIVE, &optval, optlen) < 0) {
        System_printf("Error: setsockopt failed\n");
        goto shutdown;
    }

    while ((clientfd =
            accept(server, (struct sockaddr *)&clientAddr, &addrlen)) != -1) {

        System_printf("tcpHandler: Creating thread clientfd = %d\n", clientfd);

        /* Init the Error_Block */
        Error_init(&eb);

        /* Initialize the defaults and set the parameters. */
        Task_Params_init(&taskParams);
        taskParams.arg0 = (UArg)clientfd;
        taskParams.stackSize = 1280;
        taskHandle = Task_create((Task_FuncPtr)tcpWorker, &taskParams, &eb);
        if (taskHandle == NULL) {
            System_printf("Error: Failed to create new Task\n");
            close(clientfd);
        }

        /* addrlen is a value-result param, must reset for next accept call */
        addrlen = sizeof(clientAddr);
    }

    System_printf("Error: accept failed.\n");

shutdown:
    if (server > 0) {
        close(server);
    }
}


/*
 *  ======== netOpenHook ========
 *  NDK network open hook used to initialize IPv6
 */
void netOpenHook()
{
    Task_Handle taskHandle;
    Task_Params taskParams;
    Error_Block eb;

    /* Make sure Error_Block is initialized */
    Error_init(&eb);

    /*
     *  Create the Task that farms out incoming TCP connections.
     *  arg0 will be the port that this task listens to.
     */
    Task_Params_init(&taskParams);
    taskParams.stackSize = TCPHANDLERSTACK;
    taskParams.priority = 1;
    taskParams.arg0 = TCPPORT;
    taskHandle = Task_create((Task_FuncPtr)tcpHandler, &taskParams, &eb);
    if (taskHandle == NULL) {
        System_printf("netOpenHook: Failed to create tcpHandler Task\n");
    }

    System_flush();
}
