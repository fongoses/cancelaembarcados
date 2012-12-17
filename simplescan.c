#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

int main(int argc, char **argv)
{
    inquiry_info *ii = NULL;
    int max_rsp, num_rsp;
    int dev_id, sock, len, flags;
    int i;
    char addr[19] = { 0 };//string com o endereco (fisico) do dispositivo encontrado no alcance da interface local de bluetooth.
    char name[248] = { 0 };// string com o nome dos dispositivo  ||.

    dev_id = hci_get_route(NULL); //obtem id do adaptador local a partir do 'bluetooth address' especificado. null faz com que seja retornado o id do primeiro adptador bluetooth disponivel.
    
    
    sock = hci_open_dev( dev_id ); //Abre dispositivo(obs: Essa eh uma conexao com o dispositivo local, apenas)
    
    
    //verifica erros na busca e abertura pelo dispositivo
    if (dev_id < 0 || sock < 0) {
        perror("opening socket");
        exit(1);
    }
    
    
	/* parametros da busca por dispositivos na area de alcance */
    len  = 8;
    max_rsp = 255; //numero maximo de dispositivos a serem listados na busca
    flags = IREQ_CACHE_FLUSH; //dispositivos encontrados anteriormente sofrerao flush para a nova busca
    ii = (inquiry_info*)malloc(max_rsp * sizeof(inquiry_info)); //aloca buffer para armazenamento das infos dos disposit. encontrados
    
    num_rsp = hci_inquiry(dev_id, len, max_rsp, NULL, &ii, flags); // realiza busca. A funcao retorna o numero de dispositivos encontrados  
    
    
    /*Struct de armazenamento das informações */    
	/*	typedef struct {
			bdaddr_t    bdaddr;
			uint8_t     pscan_rep_mode;
			uint8_t     pscan_period_mode;
			uint8_t     pscan_mode;
			uint8_t     dev_class[3];
			uint16_t    clock_offset;
		} __attribute__ ((packed)) inquiry_info;
    */  
    
    if( num_rsp < 0 ) perror("hci_inquiry");
    
    
    
	//Lista todos dispositivos encontrados
    for (i = 0; i < num_rsp; i++) {
        ba2str(&(ii+i)->bdaddr, addr);// ba2str: bluetooth address to string
        
        memset(name, 0, sizeof(name));        
        if (hci_read_remote_name(sock, &(ii+i)->bdaddr, sizeof(name), 
            name, 0) < 0) //obtem string do nome do dispositivo encontrado.
        strcpy(name, "[unknown]");
        
        printf("%s  %s\n", addr, name);
    }

    free( ii );
    close( sock );
    return 0;
}
