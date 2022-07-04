#include <dht.h> //Importando a biblioteca do modulo sensor de umidade

//definindo algumas portas que serão utilizadas do codigo:
#define rele1 2 // Atribui à porta digital 2 (que está ligada ao rele 1 -> Liga/Desliga os FANS) o nome de "rele1"
#define rele2 3 // Atribui à porta digital 3 (que está ligada ao rele 2 -> Liga/Desliga a bomba d'água) o nome de "rele2"
#define tempDoor A0 // Atribui à porta analógica A0 (que está ligada ao sensor de temperatura) o nome de "tempDoor"
#define higDoor A5 // Atribui à porta analógica A5 (que está ligada ao sensor de umidade do solo) o nome de "higDoor"

float SoilHumidity; // Variável que armazena a umidade momentanea do solo
int MaxIdealTemp = 30, dryLvl = 950, humiLvl = 780; // Variáveis: Armazena a temperatura necessária para ligar os FANS, o nivel de umidade necessário para regar, o nivel ideial de umidade
bool IsCooling = false, IsWatering = false; // Verificador de estado dos FANS e da bomba (LIGADO = true/DESLIGADO = false)
int pumpONCalls = 0;

dht tempS; // Instância do sensor dht (Temperatura), tempS: Objeto da classe dht

void pumpON(){
  // Função que liga a bomba e muda o estado de IsWatering (está regando) para true
  digitalWrite(rele2, LOW);
  IsWatering = true;
 
}

void pumpOFF(){
   // Função que desliga a bomba e muda o estado de IsWatering (está regando) para false
  digitalWrite(rele2, HIGH);  
  IsWatering = false;
}

void fanON(){
   // Função que liga os FANS e muda o estado de IsCooling (está resfriando) para true
  digitalWrite(rele1, LOW);
  IsCooling = true;
   
}

void fanOFF(){
  // Função que desliga os FANS e muda o estado de IsCooling (está resfriando) para false
  digitalWrite(rele1, HIGH);  
  IsCooling = false;

}

// Setup -> Conigurando informações essenciais:
void setup() {
  Serial.begin(9600); // Inicia o monitor serial
 
  pinMode(rele1, OUTPUT); // Configura a porta de controle do rele 1 para envio de informação (Arduino -> rele1)
  pinMode(rele2, OUTPUT); // Configura a porta de controle do rele 2 para envio de informação (Arduino -> rele2)
  pinMode(higDoor, INPUT); // Configura a porta de controle do sensor higrômetro para recebimento de informação (Arduino <- sensor)

  pumpOFF();
  fanOFF();

}

// Loop -> Executada a cada ciclo de processamento:
void loop() {
  tempS.read11(tempDoor);

  Serial.println("==========================================");

  Serial.print("Temperatura:                      ");
  Serial.print(tempS.temperature);
  Serial.println("°C");
  Serial.println("");
 
  // Verificando se a temeperatura está acima da desejada:
  if (tempS.temperature >= MaxIdealTemp){
    if (!IsCooling){
      fanON(); // Caso sim, e os FANS estejam desligados: ligar FANS
    }
  }

  else if (tempS.temperature < MaxIdealTemp){
    fanOFF(); // Caso não: desligar FANS
  }
 


  SoilHumidity = analogRead(higDoor); // Atribuindo à variável SoilHumidity o valor recebido do sensor higrômetro
  Serial.print("Umidade do solo:                   ");
  Serial.println(SoilHumidity);
  Serial.print("Estado do solo:                     ");
 
  // Verificando se a umidade está abaixo da desejada:
  if (SoilHumidity > dryLvl){
      Serial.println("Seco");
      if (!IsWatering){
        pumpON(); // Caso sim, e a bomba esteja desligada: ligar bomba
        pumpONCalls++;
        if (pumpONCalls >= 3){
          pumpOFF();
        }
      }
  }
 
  else if (SoilHumidity <= humiLvl){
    Serial.println("Umido");
    pumpOFF(); // Caso não: desligar bomba
    pumpONCalls = 0;
  }

  else{
    Serial.println("Regado"); // Caso esteja dentro do intervalo aceito de umidade: Não realizar ação
  }

  Serial.print("Regando:                            ");
  Serial.println(IsWatering);
  Serial.print("Resfriando:                         ");
  Serial.println(IsCooling);
  Serial.println("==========================================");
  Serial.println("");
 
  delay(5000); // aguarda 5000 milisegundos (5 segundos) para o próximo loop
}
