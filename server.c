#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>

#include <bluetooth/sdp.h>
#include <bluetooth/sdp_lib.h>





#include <sys/ioctl.h>
#include <unistd.h>
#include <linux/kd.h>

int beep(){
		int freq[] = { /* C   D    E    F    G    A    B    C */
						523, 587, 659, 698, 784, 880, 988, 1046 };
		int i;

		for (i=0; i<8; i++)
		{
				ioctl(STDOUT_FILENO, KIOCSOUND, 1193180/freq[i]);
				usleep(500000);
		}
		ioctl(STDOUT_FILENO, KIOCSOUND, 0); /*Stop silly sound*/
		return 0;
}

sdp_session_t *register_service()
{
    //uint8_t service_uuid_int[] = { 0,0,0x11,0x01,0,0,0x10,0,0x80,0,0,0x80,0x5F,0x9B,0x34,0xFB};    
    int service_uuid_int[] = { 0,0,0,0};    
    
    uint8_t rfcomm_channel = 11;
    const char *service_name = "Roto-Rooter Data Router";
    const char *service_dsc = "An experimental plumbing router";
    const char *service_prov = "Roto-Rooter";

    uuid_t root_uuid, l2cap_uuid, rfcomm_uuid, svc_uuid;
    sdp_list_t *l2cap_list = 0, 
               *rfcomm_list = 0,
               *root_list = 0,
               *proto_list = 0, 
               *access_proto_list = 0;
    sdp_data_t *channel = 0, *psm = 0;

	

    sdp_record_t *record = sdp_record_alloc();

    // set the general service ID
    sdp_uuid128_create( &svc_uuid, &service_uuid_int );
    sdp_set_service_id( record, svc_uuid );
    
    //WORKAROUND: set the service class ID
    sdp_list_t service_class = {NULL, &service_uuid_int};
	sdp_set_service_classes( record, &service_class);
    

    // make the service record publicly browsable
    sdp_uuid16_create(&root_uuid, PUBLIC_BROWSE_GROUP);
    root_list = sdp_list_append(0, &root_uuid);
    sdp_set_browse_groups( record, root_list );

    // set l2cap information
    sdp_uuid16_create(&l2cap_uuid, L2CAP_UUID);
    l2cap_list = sdp_list_append( 0, &l2cap_uuid );
    proto_list = sdp_list_append( 0, l2cap_list );

    // set rfcomm information
    sdp_uuid16_create(&rfcomm_uuid, RFCOMM_UUID);
    channel = sdp_data_alloc(SDP_UINT8, &rfcomm_channel);
    rfcomm_list = sdp_list_append( 0, &rfcomm_uuid );
    sdp_list_append( rfcomm_list, channel );
    sdp_list_append( proto_list, rfcomm_list );

    // attach protocol information to service record
    access_proto_list = sdp_list_append( 0, proto_list );
    sdp_set_access_protos( record, access_proto_list );

    
    // set the name, provider, and description
    sdp_set_info_attr(record, service_name, service_prov, service_dsc);
	int err = 0;
    sdp_session_t *session = 0;
	
	
    // connect to the local SDP server, register the service record, and 
    // disconnect   
    session = sdp_connect( BDADDR_ANY, BDADDR_LOCAL, SDP_RETRY_IF_BUSY );
    
    err = sdp_record_register(session, record, 0);
    	
    // cleanup
    sdp_data_free( channel );
    sdp_list_free( l2cap_list, 0 );
    sdp_list_free( rfcomm_list, 0 );
    sdp_list_free( root_list, 0 );
    sdp_list_free( access_proto_list, 0 );

    return session;
}

int main(int argc, char **argv)
{
    struct sockaddr_rc loc_addr = { 0 }, rem_addr = { 0 }; //address local e remoto
    char buf[1024] = { 0 }; //buffer de transferencia de dados
    int s, client, bytes_read;
    socklen_t opt = sizeof(rem_addr); //tamanho da estrutura de address do dispositivo remoto/cliente
    sdp_session_t * session; //descritor da sessão atual
	int messageCount = 0;

	while(1){
			// Aloca o socket. AF_BLUETOOTH eh o protocolo de comunicacao bluetooth.SOCK_STREAM eh um socket orientado a fluxo de bytes(TCP)
			s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

			
			//Vincula(BIND) endereco local ao socket. Nesse momento estamos lidando com o dispositivo local de bluetooth
			loc_addr.rc_family = AF_BLUETOOTH;
			loc_addr.rc_bdaddr = *BDADDR_ANY;
			loc_addr.rc_channel = (uint8_t) 10;
			bind(s, (struct sockaddr *)&loc_addr, sizeof(loc_addr));
			

			session = register_service();

			//Coloca socket em modo listen
			listen(s, 1);

			//Aceita conexoes no socket
			client = accept(s, (struct sockaddr *)&rem_addr, &opt);

			
			ba2str( &rem_addr.rc_bdaddr, buf );
			fprintf(stderr, "%d|Conexao criada com: %s\n",messageCount, buf);
			
			
			memset(buf, 0, sizeof(buf));

			//Le dados recebidos
			bytes_read = read(client, buf, sizeof(buf));
			if( bytes_read > 0 ) {
			//printf("received [%s]\n", buf);
				printf(" |--Cancela aberta para o cliente: [%s]\n", buf);
				messageCount++;				
			}
			
			
			
			// close connection
			close(client);
			close(s);

			
	}
	
	return 0;

    
}
