////////////
#include <CmdMessenger.h>  // CmdMessenger
// Blinking led variables 
bool ledState                   = 0;   // Current state of Led
const int kBlinkLed             = 13;  // Pin of internal Led
int led = 7;

CmdMessenger cmdMessenger = CmdMessenger(Serial);
enum
{
  kSetLed              , // Command to request led to be set in specific state
  kStatus              , // Command to report status
};

void attachCommandCallbacks()
{
  // Attach callback methods
  cmdMessenger.attach(OnUnknownCommand);
  cmdMessenger.attach(kSetLed, OnSetLed);
}

void OnUnknownCommand()
{
  cmdMessenger.sendCmd(kStatus,"Command without attached callback");
}

// Callback function that sets led on or off
void OnSetLed()
{
  // Read led state argument, interpret string as boolean
  ledState = cmdMessenger.readBoolArg();
  // Set led
  digitalWrite(kBlinkLed, ledState?HIGH:LOW);
  // Send back status that describes the led state
  cmdMessenger.sendCmd(kStatus,(int)ledState);
}
////
#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN         9           // Configurable, see typical pin layout above
#define SS_PIN          10          // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

MFRC522::MIFARE_Key key;


/**
 * Initialize.
 */
void setup() {
    Serial.begin(115200);  // Initialize serial communications with the PC
    //
    // Adds newline to every command
  cmdMessenger.printLfCr();   

  // Attach my application's user-defined callback methods
  attachCommandCallbacks();

  // Send the status to the PC that says the Arduino has booted
  // Note that this is a good debug function: it will let you also know 
  // if your program had a bug and the arduino restarted  
  cmdMessenger.sendCmd(kStatus,"El arduino esta vivo,ctm!!");

  // set pin for blink LED
  pinMode(kBlinkLed, OUTPUT);
    ///
    while (!Serial);    // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
    SPI.begin();        // Init SPI bus
    mfrc522.PCD_Init(); // Init MFRC522 card
    pinMode (7, OUTPUT);
    // Prepare the key (used both as key A and as key B)
    // using FFFFFFFFFFFFh which is the default at chip delivery from the factory
    for (byte i = 0; i < 6; i++) {
        key.keyByte[i] = 0xFF;
    }

    Serial.println(F("Scan a MIFARE Classic PICC to demonstrate read and write."));
    Serial.print(F("Using key (for A and B):"));
    dump_byte_array(key.keyByte, MFRC522::MF_KEY_SIZE);
    Serial.println();
    
    Serial.println(F("BEWARE: Data will be written to the PICC, in sector #1"));
}

/**
 * Main loop.
 */
void loop() {
    cmdMessenger.feedinSerialData();
    // Look for new cards
    if ( ! mfrc522.PICC_IsNewCardPresent())
        return;

    // Select one of the cards
    if ( ! mfrc522.PICC_ReadCardSerial())
        return;

    // Show some details of the PICC (that is: the tag/card)
    Serial.print(F("Card UID:"));
    dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
    Serial.println();
//    Serial.print(F("PICC type: "));
//    byte piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
//    Serial.println(mfrc522.PICC_GetTypeName(piccType));

    // Check for compatibility
//    if (    piccType != MFRC522::PICC_TYPE_MIFARE_MINI
//        &&  piccType != MFRC522::PICC_TYPE_MIFARE_1K
//        &&  piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
//        Serial.println(F("This sample only works with MIFARE Classic cards."));
//        return;
//    }

    // In this sample we use the second sector,
    // that is: sector #1, covering block #4 up to and including block #7
    byte sector         = 1;
    byte blockAddr      = 4;
    byte dataBlock[]    = {
        0x01, 0x02, 0x03, 0x04, //  1,  2,   3,  4,
        0x05, 0x06, 0x07, 0x08, //  5,  6,   7,  8,
        0x08, 0x09, 0xff, 0x0b, //  9, 10, 255, 12,
        0x0c, 0x0d, 0x0e, 0x0f  // 13, 14,  15, 16
    };
    byte trailerBlock   = 7;
    byte status;
    byte buffer[18];
    byte size = sizeof(buffer);

//    // Authenticate using key A
//    Serial.println(F("Authenticating using key A..."));
//    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
//    if (status != MFRC522::STATUS_OK) {
//        Serial.print(F("PCD_Authenticate() failed: "));
//        Serial.println(mfrc522.GetStatusCodeName(status));
//        return;
//    }

       
//    Serial.print(F("Data in block ")); Serial.print(blockAddr); Serial.println(F(":"));
//    dump_byte_array(buffer, 16); Serial.println();
//    Serial.println();

    

    // Halt PICC
    mfrc522.PICC_HaltA();
    // Stop encryption on PCD
    mfrc522.PCD_StopCrypto1();
}

/**
 * Helper routine to dump a byte array as hex values to Serial.
 */
void dump_byte_array(byte *buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
}
