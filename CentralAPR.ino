#include <SoftwareSerial.h> // XBEE COORDINADOR
SoftwareSerial XBee(52, 53); ////Rx = 0; Tx = 1; //Rx = 10; Tx = 11; //Rx = 50; Tx = 51;

byte respaldoSerieXbee[9];

///////Tiempo///////////////////////////
int ActivarTiempo = 0;
int conteoMilisegundos = 0; // conteoMilisegundos 3 Seg
int T = 0; 
int Rta = 0; 
int t = 0; 
//////////////////////////////////////

void setup() {
  Serial.begin(9600);
  ////////////////////////XBEE///////////////////////////////////////
  XBee.begin(9600);
  delay(10);
  //pinMode(52);// SoftwareSerial XBee(52, 53);
  //pinMode(53);// SoftwareSerial XBee(52, 53);

}

//////////////////////////Ciclos For ////////////////////////////////////////////////
  void cicloFor_byteArray(byte recibir[], byte transmitir[]){        
    for (int i = 0; i <=7; i++){
      recibir[i] = transmitir[i]; // descarta todos los bytes hasta llegar al de lectura de canal analogico 
    }
  }
/////////////////////////////////////////////////////////////////////////////////////

void receptdatos(){
  byte ValorCero_byte[9] = {0, 0, 0, 0, 0, 0, 0, 0};
     cicloFor_byteArray(respaldoSerieXbee, ValorCero_byte);      
}
/////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////
        int Verificacion(byte respaldo[], byte NumeroSerie[]){ //Verificar si contienen los mismos Seriales
          int conteo = 0; 
          int permitir = 0; 
              for (int i = 0; i <=7; i++){                
                if(respaldo[i] == NumeroSerie[i]){
                  conteo = conteo + 1;
                }                
              }

              if(conteo == 8){
                  permitir = 1; 
      
                  //Reiniciar Tiempo para poder Escuchar otro Xbee (Dato Externo)
                  ActivarTiempo = 1;
                  conteoMilisegundos = 0; 
                  T = 0;
                  Rta = 0; 
                  t = 0; 
              }
              return permitir;           
        }
//////////////////////////////////////////////////////////////////////////////////// 

void entradaDatosXbee()
{ 
  Serial.println("Entrada de Datos Xbee");
  byte descartar; 
  int i = 0;
  byte NumeroSerieXbee[9];
  char DATOSxbee[16];
  int DatoXbeeNumero[4];
  int PermitirEntradaDeDatosXbee = 0;
  int cantidadDatosXbee = 0;
  char incoming_char;


        for (i = 0; i<3; i++){ //
           descartar = XBee.read(); // Descartar Datos Xbee
           Serial.print(descartar, HEX);
           Serial.print(", ");
           delay(10);
        }
        
        for (i = 0; i <=7; i++){  //Recolectar Datos Serial de cada xbee
          NumeroSerieXbee[i] = XBee.read();
          Serial.print(NumeroSerieXbee[i], HEX);
          Serial.print(", ");
          delay(10);
        }

        for (i = 0; i<3; i++){
          descartar = XBee.read(); // descarta todos los bytes hasta llegar al de lectura de canal analogico 
          delay(10);
        }

        for (i = 0; i <= 15; i++){  //Informacion De perifericos
          DATOSxbee[i] = XBee.read();
          Serial.print(DATOSxbee[i]);
          Serial.print(", ");
          delay(10);
          if(DATOSxbee[i] == '*'){ // '*' Comando para salir del for (un Caracter que no sea comun)
            cantidadDatosXbee = i - 1; //solo datos utiles
            i = 16; //Valor para salir del for
          }
       }
       Serial.println(" ");

    if(NumeroSerieXbee[0] != 1){ //Negar entradas de mensaje de confirmacion de envio de xbee
      PermitirEntradaDeDatosXbee = Verificacion(respaldoSerieXbee, NumeroSerieXbee); //Verificar si contienen los mismos Seriales

        if(PermitirEntradaDeDatosXbee == 0){   
                   if (ActivarTiempo == 0) {
                      //Almacinamos la informacion, para que solamente recibir informacion de este Xbee hasta un tiempo determinado   
                      cicloFor_byteArray(respaldoSerieXbee, NumeroSerieXbee); 
                      PermitirEntradaDeDatosXbee = 1; //Este valor se reinicia solo al inicializar la funcion
                      Serial.println("Inicio de Tiempo");
                      ActivarTiempo = 1;
                   }  
        }
      /////////////////////////////////////////////////////////////////////////////////////
        if (PermitirEntradaDeDatosXbee == 1){  //Leer Datos entrante de los XBee remotos 
                      /*Leyenda:
            XAT01AA
            X = Xbee
            A = Letra del router A (Primer Router)
            T = Tablero
            01 = Primer Tablero
            AA = Direccion del registro o Cambio a realizar (Para saber en que parte de la APP van los cambios. Ejemplo: Texto, botones, Leds, Grafico, Mensajes de alerta etc.)
            Caracteres (15) Datos a enviar*/
            
           }
      /////////////////////////////////////////////////////////////////////////////////////      
        }
}
void loop() {

 if (ActivarTiempo == 1) { //
    conteoMilisegundos = ++T;
    delay(1);
    if (conteoMilisegundos >= 1000) { //  con el (filtroSerie==1) acumula un segundo
      conteoMilisegundos = 0;
      T = 0;
      Rta = ++t;
      if (Rta >= 3) { // se espera 3 segundos para poder otra vez intentar comunicarnos con la central
        Rta = 0;
        t = 0;
        
        ActivarTiempo = 0;
        

        
        receptdatos();
        Serial.println("Termino El tiempo");
        
      }
    }
  }
  
 if (XBee.read() == 0x7E){ //byte de inicio    //// INICIO DE LECTURA XBEE  ///////
      entradaDatosXbee();
 }
/////
}
/////////////////////////////////////////////////////////////////////////////////////


