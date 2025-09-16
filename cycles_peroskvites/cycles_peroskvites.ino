const byte maxChars = 32;
char receivedChars[maxChars];
char tempChars[maxChars];        // temporary array for use when parsing

      // variables to hold the parsed data
char messageFromPC = '0';
int n_Cycles=0;
int t_Purge=0;
int t_CO2=0;
int t_H2=0;
//PINS
int co2 = 10;
int h2 = 11;
int purge = 12;

boolean newData = false;

//============

void setup() 
{
    Serial.begin(9600);
    pinMode(co2, OUTPUT);
    pinMode(h2, OUTPUT);
    pinMode(purge, OUTPUT);
    //pinMode(13, OUTPUT);
}

//============

void loop() 
{
    receiveData(); // <int, int, int, int> CO2, H2, Purge, Cycles
    if (newData == true) {
        strcpy(tempChars, receivedChars);
            // this temporary copy is necessary to protect the original data
            // because strtok() used in parseData() replaces the commas with \0
        parseData();
        cyclesPerovskites();
        newData = false;
        
    }
}

//============

void receiveData() {
    static boolean recvInProgress = false;
    static byte ndx = 0;
    char startMarker = '<';
    char endMarker = '>';
    char rc;

    while (Serial.available() > 0 && newData == false) {
        rc = Serial.read();

        if (recvInProgress == true) {
            if (rc != endMarker) {
                receivedChars[ndx] = rc;
                ndx++;
                if (ndx >= maxChars) {
                    ndx = maxChars - 1;
                }
            }
            else {
                receivedChars[ndx] = '\0'; // terminate the string
                recvInProgress = false;
                ndx = 0;
                newData = true;
            }
        }

        else if (rc == startMarker) {
            recvInProgress = true;
        }
    }
}

//============

void parseData() {      // split the data into its parts

    char * strtokIndx; // this is used by strtok() as an index

    strtokIndx = strtok(tempChars,",");
    t_CO2 = atoi(strtokIndx); // convert to an integer
 
    strtokIndx = strtok(NULL, ","); 
    t_H2 = atoi(strtokIndx);    

    strtokIndx = strtok(NULL, ",");
    t_Purge = atoi(strtokIndx);  

    strtokIndx = strtok(NULL, ",");
    n_Cycles = atoi(strtokIndx);   

}

//============

void cyclesPerovskites() {
  for (int i = 1; i <= n_Cycles; i++) {
    Serial.print("Performing cycle ");
    Serial.print(i);
    Serial.print(" out of ");
    Serial.println(n_Cycles);
    //CO2 Step
    digitalWrite(co2, HIGH);
    delay(t_CO2*60000);
    digitalWrite(co2, LOW);
    //Purge Step
    digitalWrite(purge, HIGH);
    delay(t_Purge*60000);
    digitalWrite(purge, LOW);
    //H2 Step
    digitalWrite(h2, HIGH);
    delay(t_H2*60000);
    digitalWrite(h2, LOW);
    //Purge Step
    digitalWrite(purge, HIGH);
    delay(t_Purge*60000);
    digitalWrite(purge, LOW);
  }
  Serial.print('&');
  Serial.print(n_Cycles);
  Serial.println(" cycles completed");
  
}
