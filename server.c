#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>

int main(int argc, char **argv)
{
    struct sockaddr_rc loc_addr = { 0 }, rem_addr = { 0 }; //address local e remoto
    char buf[1024] = { 0 }; //buffer de transferencia de dados
    int s, client, bytes_read;
    socklen_t opt = sizeof(rem_addr); //tamanho da estrutura de address do dispositivo remoto/cliente
    
    

    // Aloca o socket. AF_BLUETOOTH eh o protocolo de comunicacao bluetooth.SOCK_STREAM eh um socket orientado a fluxo de bytes(TCP)
    s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

    
    //Vincula(BIND) endereco local ao socket. Nesse momento estamos lidando com o dispositivo local de bluetooth
    loc_addr.rc_family = AF_BLUETOOTH;
    loc_addr.rc_bdaddr = *BDADDR_ANY;
    loc_addr.rc_channel = (uint8_t) 1;
    bind(s, (struct sockaddr *)&loc_addr, sizeof(loc_addr));

    //Coloca socket em modo listen
    listen(s, 1);

    //Aceita conexoes no socket
    client = accept(s, (struct sockaddr *)&rem_addr, &opt);

	
    ba2str( &rem_addr.rc_bdaddr, buf );
    fprintf(stderr, "accepted connection from %s\n", buf);
    memset(buf, 0, sizeof(buf));

    //Le dados recebidos
    bytes_read = read(client, buf, sizeof(buf));
    if( bytes_read > 0 ) {
        printf("received [%s]\n", buf);
    }

    // close connection
    close(client);
    close(s);
    return 0;
}
