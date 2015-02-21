#include <Servo.h> 
#include <SPI.h>
#include <Ethernet.h>

#include "Timer.h"

byte mac[] = { 0xDE, 0xAD, 0xDE, 0xAD, 0xFF, 0xFF };
char server[] = "pebolim.novatrix.com.br";


#define UNLOCK_TIME 1000
#define STATUS_TIME 10000
#define CHECKSERVER_TIME 10000
#define VALIDATE_TIMER 2000




#define GOL1 A0
#define GOL2 A1
#define LED_RESET 7
#define SERVO_1 5
#define SERVO_2 6
#define BOTAO_RESET 2
#define CHAVE 3
#define BUZZER 4


// status de trava dos servos
#define OPEN true
#define CLOSED false

// posicao dos servos
#define GOL1_CLOSED 40
#define GOL1_OPEN 90
#define GOL2_CLOSED 90
#define GOL2_OPEN 40


unsigned long lastConnectionTime = 0;          // last time you connected to the server, in milliseconds
boolean lastConnected = false;                 // state of the connection last time through the main loop
const unsigned long postingInterval = 60*1000;  // delay between updates, in milliseconds

boolean gol1, gol2;
Servo servo1, servo2;
EthernetClient client;
Timer t;



boolean ethernet_connected = false;
boolean pode_desistir_partida = false;
int botao_reset = HIGH;
String httpBuffer = "";
int tReset = 0;


//OK: detectar bolinhas nos dois sensores
//OK: controlar servos
//OK: se ligar com a chave virada para ON funciona no modo aberto e entra num loop
//OK: status partida servidor
//OK: marcar gol no servidor
//OK: utilizar timer de tempo para verificar status acada 10 segundos quando a bolinha esta parada no gol

//TODO: utilizar timer para fechar trava das bolinhas
//TODO: matar conexao se nao tiver resposta em 10 segundos e tentar novamente


#define SENSOR_LIMITE 50
boolean marcou_gol = false;
boolean marcou_erro = false;
boolean modo_livre = false;



void setup() {
  
  // initialize serial communications at 9600 bps:
  Serial.begin(9600); 
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  Serial.println("Pebolim v2");
  
  
  
  
  pinMode(LED_RESET, OUTPUT);
  digitalWrite(LED_RESET, LOW);


  pinMode(CHAVE, INPUT);
  digitalWrite(CHAVE, HIGH);

  pinMode(BOTAO_RESET, INPUT);
  digitalWrite(BOTAO_RESET, HIGH);
  
  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, LOW);
  
  servo1.attach(SERVO_1);
  servo2.attach(SERVO_2);
  
  marcou_gol = update_gol_status();


  modo_livre = digitalRead(CHAVE) == LOW;
  if (modo_livre) {
    Serial.println("Modo LIVRE");
    unlock_gols(false);
    
  } else {
    lock_gols();
    ethernet_connected = Ethernet.begin(mac) != 0;
    
    if (!ethernet_connected && !modo_livre) {
      Serial.println("Failed to configure Ethernet using DHCP");
      // no point in carrying on, so do nothing forevermore:
      for(;;) {
        beep(10, 500, 100);
        delay(30 * 1000);
      }
    }
    
    
    Serial.print("My IP address: ");
    for (byte thisByte = 0; thisByte < 4; thisByte++) {
      // print the value of each byte of the IP address:
      Serial.print(Ethernet.localIP()[thisByte], DEC);
      Serial.print("."); 
    }
    Serial.println();
    
    checkServer("");
    
  }

  
  t.every(CHECKSERVER_TIME, checkServer);
  //tReset = t.oscillate(LED_RESET, 1000, HIGH);
  
  beep(3, 50, 100);

}

int checkStatusGolMarcado = 0;



boolean server_connected = false;






















void checkServer() {
  if (server_connected || !update_gol_status() || checkStatusGolMarcado!=0 || botao_reset == LOW) return;
  checkServer("");
}

void checkServer(String gol) {
  
  Serial.print("recebeu gol ");
  Serial.println(gol);
  
  // if there's a successful connection:
  if (client.connect(server, 80)) {
    Serial.println("connecting...");
    // send the HTTP PUT request:
    client.print("GET /arduino/");
    client.print(gol);
    client.println(" HTTP/1.1");
    
    client.print("Host: ");
    client.println(server);
    
    client.println("User-Agent: arduino-ethernet");
    client.println("Connection: close");
    client.println();

    // note the time that the connection was made:
    lastConnectionTime = millis();
    server_connected = true;
    Serial.println("---");
    
  } 
  else {
    // if you couldn't make a connection:
    Serial.println("connection failed");
    Serial.println("disconnecting.");
    client.stop();
    server_connected = false;
  }
  
}








void confirmaGolMarcado() {
  boolean novo_marcou_gol = update_gol_status();


  Serial.println("confirmando gol marcado");
  Serial.print("checking gols ");
  Serial.print(gol1);
  Serial.print(" ");
  Serial.print(gol2);
  Serial.println();
  
  
  if (novo_marcou_gol) {
    beep(1, 50, 100);

    // avisar servidor que marcou gol para um dos times
    checkServer(gol1 ? "A/" : "B/");
    checkStatusGolMarcado = 0;
  }
}









void checkGols() {
  //Serial.println(".");
  
  boolean novo_marcou_gol = update_gol_status();

  if (marcou_gol != novo_marcou_gol) {
    //Serial.println("MUDOU STATUS GOL");
    marcou_gol = novo_marcou_gol;
    
    //Serial.print("checkStatusGolMarcado = ");
    //Serial.println(checkStatusGolMarcado);
    
    if (marcou_gol && checkStatusGolMarcado == 0) {
      // cria timer para verificar se a bola estabilizou no gol
      //Serial.println("criou timer");
      checkStatusGolMarcado = t.after(VALIDATE_TIMER, confirmaGolMarcado);
      
    } else if (!marcou_gol && checkStatusGolMarcado != 0) {
      // cancelar timer para checar bola estabilizada
      t.stop(checkStatusGolMarcado);
      checkStatusGolMarcado = 0;
      //Serial.println("cancelou timer");
    }

  }



}











boolean update_gol_status() {
  gol1 = analogRead(GOL1) < SENSOR_LIMITE;
  gol2 = analogRead(GOL2) < SENSOR_LIMITE;
  return gol1 || gol2;
}


void servo_control(boolean status) {
  if (status == OPEN) {
    servo1.write(GOL1_OPEN);
    servo2.write(GOL2_OPEN);
  } else {
    servo1.write(GOL1_CLOSED);
    servo2.write(GOL2_CLOSED);
  }
  
}


void unlock_gols() {
  unlock_gols(true);
}
void unlock_gols(boolean lock) {
  //Serial.println("UNLOCK GOLS");
  servo_control(OPEN);
  if (lock) {
    delay(UNLOCK_TIME);
    lock_gols();
  }
}

void lock_gols() {
  //Serial.println("LOCK GOLS");
  servo_control(CLOSED);
}













void checkPodeDesistirPartida() {
  Serial.print("PODE DESISTIR ");
  Serial.println(pode_desistir_partida);
  
  if (pode_desistir_partida) {
    //if (tReset == 0)
    //  tReset = t.oscillate(LED_RESET, 1000, HIGH);
    digitalWrite(LED_RESET, HIGH);

  } else {
    digitalWrite(LED_RESET, LOW);
    //if (tReset > 0) {
    //  t.stop(tReset);
    //  tReset = 0;
    //}
  }

}





void processServerResponse(String s) {
  
  //\t\t %s %d %02d %02d %04d \n%s\n"
  //status = "N"
  //release_ball = 1
  //goals_a = 00
  //goals_b = 00
  //timeout = 0000
  
  
  char status = s.charAt(0);
  boolean releaseBall = s.substring(1,2).toInt() == 1;
  
  //0 1 23 45 6789
  //N 1 00 00 0000
  
  int goals_a = s.substring(2,4).toInt();
  int goals_b = s.substring(4,6).toInt();
  int timeout = s.substring(6).toInt();
  
  pode_desistir_partida = goals_a + goals_b > 0;
  checkPodeDesistirPartida();
  
  Serial.print("Status: "); Serial.println(status);
  Serial.print("Release Ball: "); Serial.println(releaseBall);
  Serial.print("GOALS A: "); Serial.println(goals_a);
  Serial.print("GOALS B: "); Serial.println(goals_b);
  Serial.print("TIMEOUT: "); Serial.println(timeout);
  Serial.println();
  Serial.println();
  Serial.println();
  Serial.println();
  Serial.println();
  Serial.println();


  if (gol1) {
    //beep(goals_a, 100, 500);
  } else if (gol2) {
    //beep(goals_b, 100, 500);
  }


  if (releaseBall && update_gol_status()) {
    beep(2, 10, 200);
    Serial.println("RELEASE BALL");
    
    unlock_gols();
    
  } else {
    Serial.println("LOCK BALL");
    beep(1, 0, 50);
  }
  
  
}




void loop() {
  t.update();

  if (modo_livre != digitalRead(CHAVE) == LOW) {
    Serial.println("MUDOU MODO DE JOGO");
    modo_livre = digitalRead(CHAVE) == LOW;
    if (modo_livre) {
      unlock_gols(false);
      pode_desistir_partida = false;
      checkPodeDesistirPartida();

    } else {
      lock_gols();
      if (!ethernet_connected) {
        Serial.println("Ativando ethernet no loop. Saiu do modo livre.");
        setup();
      }
      checkServer("");
    }
  }
  

  if (modo_livre) {
    
    if (update_gol_status())
      beep(1, 0, 100);

    return;
  }


  // verificar reset 
  if (pode_desistir_partida) {
    if (botao_reset != digitalRead(BOTAO_RESET)) {
      botao_reset = digitalRead(BOTAO_RESET);
      if (botao_reset == LOW) {
        delay(1000);
        //TODO: chamada de RESET da PARTIDA
        beep(4, 50, 200);
        
        Serial.println("DESISTINDO DA PARTIDA...");
        checkServer("R/");
        
        
      }
    }
  }

  checkGols();
  
  if (ethernet_connected) {
    Ethernet.maintain();

    // if there's incoming data from the net connection.
    // send it out the serial port.  This is for debugging
    // purposes only:
    if (client.available()) {
      char c = client.read();
      if (c=='\n') {
        //TODO: verificar buffer e ver se comeca com \t\t
        if (httpBuffer.length() == 12 && httpBuffer.startsWith("\t\t")) {
          processServerResponse(httpBuffer.substring(2));
        } 
        //else {
        //  Serial.println(httpBuffer);
        //}
        httpBuffer = "";
      } else {
         httpBuffer.concat(c);
      }
    }
  
    // if there's no net connection, but there was one last time
    // through the loop, then stop the client:
    if (!client.connected() && lastConnected) {
      Serial.println();
      Serial.println("xxx");
      client.stop();
      server_connected = false;
    }
  
    // if you're not connected, and ten seconds have passed since
    // your last connection, then connect again and send data:
    //if(!client.connected() && (millis() - lastConnectionTime > postingInterval)) {
    //  httpRequest();
    //}
    // store the state of the connection for next time through
    // the loop:
    lastConnected = client.connected();  

  }

}













void beep(int times, int delay_off, int delay_on) {
  for(int i=0; i<times; i++) {
    digitalWrite(BUZZER, HIGH);
    delay(delay_on);
    digitalWrite(BUZZER, LOW);
    delay(delay_off);
  }
}

