#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

const int NFClen = 10;
String masterID = "";
String NFCID[10];
bool isMasterMode = false;

void setup() 
{ 
  // init LED_BUILTIN pin as LED
  pinMode(LED_BUILTIN, OUTPUT);
	digitalWrite(LED_BUILTIN, HIGH);

  // for NFCID
  for (int i = 0; i < NFClen; i++)
    NFCID[i] = "";

  Serial.begin(9600);     					// Initiate a serial communication
  SPI.begin();            					// Initiate SPI bus
  mfrc522.PCD_Init();     					// Initiate MFRC522

  Serial.println("NFC reader ready!");
}

void loop() 
{
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  
  //save UID in cardID
  String cardID = "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
    cardID.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    cardID.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  cardID.toUpperCase();
  cardID = cardID.substring(1);

  // If mastedID not set then this card is master card
  if(masterID == ""){
    masterID = cardID;
    Serial.println("Master card register");
    }
  // otherwise (when master card is set) and if master card touched, switch master mode
  else if (cardID == masterID){
    isMasterMode = !isMasterMode;
		// digitalWrite(LED_BUILTIN, isMasterMode ? HIGH : LOW);
    if(isMasterMode){
      digitalWrite(LED_BUILTIN, HIGH);
    }
    else{
      digitalWrite(LED_BUILTIN, HIGH);
    }
    Serial.print("Master mode turned ");
    Serial.println(isMasterMode ? "ON" : "OFF");
    }
  else{
    if(isMasterMode){
      int isFound = -1;

      // check if touched card in array
      for (int i = 0; i < NFClen; i++)
        if(cardID == NFCID[i])
          isFound = i;

      if(isFound != -1){
        NFCID[isFound] = "";
        Serial.println("Card removed from whitelist!");
      }
      else{ //adding card
        // try to find empty slot
        for (int i = 0; i < NFClen; i++){
          if(NFCID[i] == ""){
            NFCID[i] = cardID;
            Serial.println("Card added to whitelist!"); 
						digitalWrite(LED_BUILTIN, HIGH);
            break;
          }
          else if(i == NFClen - 1)
						digitalWrite(LED_BUILTIN, HIGH);
            Serial.println("No more slots for card on whitelist!"); 
        }//for
      }//else
    }// masterMode
    else{
      // checking access
      for (int i = 0; i < NFClen; i++){
        if(NFCID[i] == cardID){
          Serial.println("Access granted");
					digitalWrite(LED_BUILTIN, HIGH);
          break;
        }
        else if(i == NFClen -1){
          Serial.println("Access denied");
					digitalWrite(LED_BUILTIN, HIGH);
				}
      }
    }
  }// main else 

  delay(3000);
}//loop
