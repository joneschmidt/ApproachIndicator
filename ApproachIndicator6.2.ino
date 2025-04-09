// ApproachIndicator
//
// otherwise known as "Hit the Bell" - train approach detection for a tower
// enable an alert when a train approaches - no alert as it leaves
// relies on DISTANT/HOME detectors for each end of a town or tower trackage
//
// *************************************************************************
// Revision history:
#define Vers "2025/04/09"
//   2025/04/02 13:00 J. Schmidt 6.2 Update short & reset timing
//   2025/01/14 21:00 J. Schmidt 6.1 Major fixes
//   2024/11/10 11:30 J. Schmidt 6.0 Update DontRepeatSecs & RecalibrateRecalTM timers
//   2024/08/20 12:00 J. Schmidt 6.0 Configure for CV locations
//   2024/07/25 15:00 J. Schmidt 6.0 Configure for Nicasio Northern
//   2024/07/02 11:30 J. Schmidt 6.0 Configure for Central Vermont
//   2024/06/28 16:30 J. Schmidt 6.0 Complete changes to 6.0
//                                      update analog sensor sensitivity processing
//   2023/02/25 13:00 J. Schmidt 6.0 Change to an "approach" orientation - each approach
//										is defined with a unique or shared alert
//										each alert can be a timed sound/light
//										or continuous with a reset button
//   2023/01/01 13:00 J. Schmidt 6.0 Universal sensor assignment
// *************************************************************************
// NOTE re MRCS boards
//
// The MRCS boards refer to "Station 0, Station 1," etc.  East & West,
// HOME and DISTANT.  The sketch below provides greater flexibility and
// granularity in defining approaches.
// Note too that Stations 0 & 1 are analog, and 2 & 3 are digital, on the board.
// The sketch only limits analog/digital to a particular approach, not the station.
// So instead of thinking about Station 0 think about Approaches 0 & 1.
// Any of the ports may be repurposed for the reset button input
// *************************************************************************
// Change anything below here at your own risk
// *************************************************************************
// NOTE relative positioning of sensors
//    - pins and connections must match the order below
//
//DISTANT = 0 DISTANT sensor
//HOME    = 1 HOME sensor
#define DISTANT 0
#define HOME    1
//
// sensor type definitions
// analog sensor type
#define TypeAlog 1    
// digital sensor type  
#define TypeDigt 2       
//
// high/low defines for sound - USER-CHANGEABLE
#define SndOff   LOW
#define SndOn    HIGH
//
// ---NowOccStat & PhaseStat values
// - sensor is not occupied
#define StatClr   1
// - sensor is occupied
#define StatOcc   2
// - ignore sensor when exiting train covers
#define StatExiting 3  // wait for exiting train to clear
// wait for HOME sensor to be hit to trigger alert
#define StatPend    4  
// analog sampling controls - needed to average light flicker
// length of sample integration in millisecs
// 34 ms for 60 cycle lighting mains
// 40 ms for 50 cycle lighting mains
#define LoopMS 34
//
// ignore/reset if Distant to Home time is longer than this
#define ShortTripSecs 40 
// reset sensors every RecalMts
#define RecalMts 10 
// hold occupied status for this time to minimize bounce (false clear)
// - for couplers, skeleton cars - in milliseconds
#define HoldOccMS 2000. 
// Sensitivity - alert only if analog sensor drops this % from normal
#define Sentivity 70
//
// DEFINE each approach 
typedef struct {
    //   SnsrType TypeAlog TypeDigt
	// both sensors must be the same type
	int SnsrType;
    //   Sensor addresses: Distant - Home
	int SnsrLst [2];
	// index into Alert table
	int AlrtIdx;
}ApproDef;
//
// DEFINE each alert 
typedef struct {
	// for buzzer or light -- Arduino address digital
	int SndPort;
	// Duration of light or sound -- 0 means keep on until cleared
	int AlrtSecs;
	// Arduino port for reset button if required
	int RstPort;
    unsigned long DontRepeatMS; // expire time for blocking repeat alert in seconds
    unsigned long AlertEndMs;   // expire time for alerts
}AlrtDef;
//
// ttttttttttttttttt Trace & Debug ttttttttttttttttt
#define Trace 0
//
#if Trace > 0
// delay in ms for main loop sampling 
// -- use large value if tracing/debugging
#define MainLoopDlyMS 5000
//
String StatText[5] = {"no","StatClr","StatOcc","StatExiting","StatPend"};
#endif
// if SnsrActiveLED is non-zero, light the LED indicated if any sensor is active
// -- useful during setup – note possible conflict with AlrtPins
#define SnsrActiveLED 0
// ttttttttttttttttt Trace & Debug ttttttttttttttttt
//
// *************************************************************************
// USER Defines
// *************************************************************************
// UNCOMMENT this section to activate testing definitions
// /*
// *************************************************************************
// Definitions for testing
// active approach count
#define ApproCnt 2
// Define the active alert devices
#define AlrtCnt 1
//
// invert flag for digital sensor if HIGH sensor means clear
#define DigInvert false
// NOTE that the reset button sensors are set for INPUT_PULLUP and do not
// use the DigInvert flag.  The buttons should be tied to 5v ground and the 
// digital port.
//
// dont repeat alert for a bit - in seconds - stop false repeats
#define DontRepeatSecs 120
//
// define the alerts
AlrtDef Alrts [AlrtCnt] = {
	// signal port, duration secs, clear port, (DontRepeatMS, AlertEnd)
  	    10,         60,            11,     0,0  // right
};// Alrts
// define the approaches
ApproDef Appro [ApproCnt] = {
	// define each approach
    //   Type  Distant  Home  Alert (2 temp)
	// *** approach 0 alert 0 
  	  TypeAlog, A0, A1, 0,  // left
	  TypeAlog, A3, A4, 0   // right
}; // Appro
// END testing definitions
// */ 
// *************************************************************************
// UNCOMMENT this section to activate Nicasio Northern definitions
/*
// *************************************************************************
// Definitions for Nicasio Northern
// active approach count
#define ApproCnt 1
// Define the active alert devices
#define AlrtCnt 1
//
// invert flag for digital sensor if HIGH sensor means clear
bool DigInvert = false; 
// NOTE that the reset button sensors are set for INPUT_PULLUP and do not
// use the DigInvert flag.  The buttons should be tied to 5v ground and the 
// digital port.
//
// dont repeat alert for a bit - in seconds - stop false repeats
#define DontRepeatSecs 60
//
// define the alerts
AlrtDef Alrts [AlrtCnt] = {
	// signal port, duration secs, clear port, (DontRepeatMS, AlertEnd)
	10, 10, 11,     0,0 // Backdoor
};// Alrts
// define the approaches
ApproDef Appro [ApproCnt] = {
	// define each approach
    //   Type  Distant  Home  Alert
	// *** Backdoor approach 0 alert 0 
    	  TypeAlog, A0, A1,  0 // Backdoor
}; // Appro
// END Nicasio Northern definitions
*/
// *************************************************************************
// UNCOMMENT this section to activate Norwich - Stafford definitions
/* Note: old bootloader
// installed 2/18/25
// *************************************************************************
// Definitions for Norwich - Stafford
// active approach count
#define ApproCnt 4
// Define the active alert devices
#define AlrtCnt 2
//
// invert flag for digital sensor if HIGH sensor means clear
boolean DigInvert = false;  
// NOTE that the reset button sensors are set for INPUT_PULLUP and do not
// use the DigInvert flag.  The buttons should be tied to 5v ground and the 
// digital port.
//
// dont repeat alert for a bit - in seconds - stop false repeats
#define DontRepeatSecs 120
//
// define the alerts
AlrtDef Alrts [AlrtCnt] = {
	// signal port, duration secs, clear port, (DontRepeatMS, AlertEnd)
	11, 60, 3,     0,0, // Norwich
	10, 60, 2,     0,0  // Stafford
};// Alrts
// define the approaches
ApproDef Appro [ApproCnt] = {
	// define each approach
    //   Type  Distant  Home  Alert 
	// *** Norwich approach alert 0
  	  TypeAlog, A0, A1, 0,    // Norwich left
	// *** Norwich approach alert 0
	    TypeAlog, A2, A3, 0,  // Norwich right
	// *** Stafford approach alert 1
  	  TypeAlog, A4, A5, 1,    // Stafford left
	// *** Stafford approach alert 1
	    TypeAlog, A6, A7, 1   // Stafford right
}; // Appro
// END Norwich - Stafford definitions
*/
/*
// *************************************************************************
// UNCOMMENT this section to activate Palmer Willimantic definitions
//  Note: old bootloader
// installed 2/18/25
// *************************************************************************
// Definitions for Palmer Willimantic
// active approach count
#define ApproCnt 4
// Define the active alert devices
#define AlrtCnt 2
//
// invert flag for digital sensor if HIGH sensor means clear
boolean DigInvert = false;  
// NOTE that the reset button sensors are set for INPUT_PULLUP and do not
// use the DigInvert flag.  The buttons should be tied to 5v ground and the 
// digital port.
//
// dont repeat alert for a bit - in seconds - stop false repeats
#define DontRepeatSecs 120
//
// define the alerts
AlrtDef Alrts [AlrtCnt] = {
	// signal port, duration secs, clear port, (DontRepeatMS, AlertEnd)
	13, 20, 3,     0,0, // Palmer
	12, 20, 2,     0,0  // Willimantic
};// Alrts
// define the approaches
ApproDef Appro [ApproCnt] = {
	// define each approach
    //   Type  Distant  Home  Alert
	// *** Palmer approach 0 alert 0  
  	  TypeAlog, A0, A1, 0,   // Palmer left
	// *** Palmer approach 1 alert 0
	  TypeAlog, A2, A3, 0,   // Palmer right
	// *** Willimantic approach 2 alert 1
  	  TypeAlog, A4, A5, 1,   // Willimantic left
	// *** Willimantic approach 3 alert 1
	  TypeAlog, A6, A7, 1    // Willimantic right
}; // Appro
// END Palmer Willimantic definitions
*/
// *************************************************************************
// END USER Defines
// *************************************************************************
// *************************************************************************
// ******************* Variable definitions
#define NumSnsrs ApproCnt*2
//
int APidx, ALidx, Sidx, Lidx, SnsrHome, SnsrDist;
boolean Occpied;
// data arrays for sensors
// NOTE: These arrays are indexed bsed on the Approach index
int Largest     [NumSnsrs]; // largest analog values seen
int PhaseStat   [NumSnsrs]; // prior occupied state
int NowOccStat  [NumSnsrs]; // now occupied state
bool Chngd      [NumSnsrs]; // now occupied state
unsigned long 
    NowOccTM    [NumSnsrs]; // timing when last occupied
// expire time for SnsrActiveLED
unsigned long SnsrActiveMS;  // expire time for SnsrActiveLED
// LOOP variables
unsigned long CycleStrtMs; // cycle start current time
unsigned long Secs;        // current time in seconds
unsigned long Mins;        // current time in minutes
unsigned long RecalTM;     // Recalibrate time
unsigned long Cycles;      // count of loops
unsigned long CycleMs;     // total ms
unsigned long ThisCycleMs; // total ms in this cycle
// *************************************************************************
//
// ============= ClearAlerts ======================
// Clear active alerts
void ClearAlerts (){
// ============= ClearAlerts ======================
boolean ClrAlrt, ManClrAlrt;
int     dsensor;
int iidx;
// look through the whole list of alerts
for (ALidx = 0; ALidx < AlrtCnt; ++ ALidx){
    ClrAlrt = ManClrAlrt = false;
	if (Alrts[ALidx].AlertEndMs > 0 && Alrts[ALidx].AlertEndMs > millis()){
	if (Alrts[ALidx].RstPort > 0) // check cancel button
	  {dsensor = digitalRead(Alrts[ALidx].RstPort);
	  #if Trace > 0
        Serial.print("Alert ");
        Serial.print(ALidx);
        Serial.print(" button ");
        Serial.print(Alrts[ALidx].RstPort);
        Serial.print(" value ");
        Serial.println(dsensor);
        #endif
	   if (dsensor == LOW) {ManClrAlrt = true;}
	  } // if RstPort
	  } // not expired
	if (Alrts[ALidx].AlertEndMs > 0 && Alrts[ALidx].AlertEndMs < millis())
	{ClrAlrt = true; 
     // if alert expired, check if another on same port is still active
    for (iidx = 0; iidx < AlrtCnt; ++iidx){
	   if (Alrts[ALidx].SndPort == Alrts[iidx].SndPort && ALidx != iidx){
		 if (Alrts[iidx].AlertEndMs > millis()) // check not expired
			{ClrAlrt = false;}
		 if (Alrts[iidx].RstPort > 0) // check cancel button
	        {dsensor = digitalRead(Alrts[iidx].RstPort);
	         #if Trace > 0
             Serial.print("Alert ");
             Serial.print(ALidx);
             Serial.print(" button ");
             Serial.print(Alrts[ALidx].RstPort);
             Serial.print(" value ");
             Serial.println(dsensor);
             #endif
	         if (dsensor == LOW) {ManClrAlrt = true;}
			} // RstPort
		} // if ports
	} // for iidx
    } // if expired
	if (ClrAlrt || ManClrAlrt) {       
	  digitalWrite(Alrts[ALidx].SndPort, SndOff);
	  Alrts[ALidx].AlertEndMs = 0; 
	  #if Trace > 0
        Serial.print("Clear alert ");
        Serial.print(ALidx);
        if (ClrAlrt)    {Serial.print(" timing ");}
	    if (ManClrAlrt) {Serial.print(" button ");}
        Serial.println("");
	  #endif 
	  } // clear flags
	} // for ALidx
} // ClearAlerts
// *************************************************************************
void ClearSnsrs (){
for (Sidx = 0; Sidx < NumSnsrs; ++ Sidx){
    Largest     [Sidx] = 0; // largest analog values seen
    PhaseStat   [Sidx] = StatClr; // prior occupied state
    NowOccStat  [Sidx] = StatClr; // now occupied state
    NowOccTM    [Sidx] = 0; // timing last occupied
    Chngd       [Sidx] = false; // sensor chaanged
  } // Sidx 
  #if Trace > 1
    Serial.println("Clear sensors");
  #endif 
} // ClearSnsrs
// *************************************************************************
// ============= ReadSensors ======================
// read the sensors and set Occpied
void ReadSensors (){
// ============= ReadSensors ======================
	int dsensor;
	long int Smpls, Valu, Tpct;
    unsigned long CycleStrt;
	Occpied = false;
for (APidx = 0; APidx < ApproCnt; ++APidx){ // approaches
	for (Lidx = 0; Lidx <= HOME; ++ Lidx){ // DISTANT/HOME
	 Sidx = APidx * 2 + Lidx;
	 Chngd [Sidx] = false;
	 if (Appro[APidx].SnsrType == TypeDigt) 
	 { // read digital sensors
      if (Appro[APidx].SnsrLst[Lidx] > 0){
        dsensor = digitalRead(Appro[APidx].SnsrLst[Lidx]);
        if (DigInvert)
		  {dsensor = (dsensor == HIGH)?LOW:HIGH;}// invert
	      } // if SnsrLst[APidx]> 0
     }// read digital sensors
	//
	// read analog sensors
	if (Appro[APidx].SnsrType == TypeAlog) { 
	   Valu = Smpls = 0.;
	   dsensor = LOW;
 	   CycleStrt = millis();
       do { // do the integration over LoopMS time
          Valu += analogRead(Appro[APidx].SnsrLst[Lidx]);
          ++Smpls;
        } while (CycleStrt + LoopMS > millis());
    Valu /= Smpls; // get average
	// calc min/max
    Largest[Sidx] = max(Largest[Sidx], Valu);
    Tpct = 100. - (Valu * 100.)/Largest[Sidx];
    if (Tpct > Sentivity) // drop is larger than sensitivity 
        {dsensor = HIGH;} // sensitivity
    #if Trace > 0
      Serial.print  ("Read Approach ");
      Serial.print  (APidx);
      Serial.print  (" sensor ");
      Serial.print  (Appro[APidx].SnsrLst[Lidx]);
      Serial.print  (" hi ");
      Serial.print  (Largest [Sidx]);
      Serial.print  (" current ");
      Serial.print  (Valu);
      Serial.print  (" drop ");
      Serial.print  (Tpct);
      Serial.println  ("%");
      #endif  
	} // read analog sensors
	
	if (dsensor == HIGH) // occupied
	   {Occpied = true;
	    if (NowOccStat[Sidx] == StatClr)
		  {Chngd [Sidx] = true;
		   NowOccTM  [Sidx] = millis();
		   NowOccStat[Sidx] = StatOcc;
		   } // been clear		
	   } else if (NowOccStat[Sidx] == StatOcc && NowOccTM [Sidx] + HoldOccMS < millis())
		   // now clear, been occupied & timer?
		    {NowOccStat[Sidx] = StatClr;
			 Chngd [Sidx] = true;} // not occupied// dsensor
    #if Trace > 0
	  if (Chngd [Sidx] == true){
      Serial.print  (" sensor ");
      Serial.print  (Appro[APidx].SnsrLst[Lidx]);
      Serial.print  (" changed to ");
      Serial.print  (StatText[NowOccStat[Sidx]]);
      Serial.println ();
	  } // changed
      #endif  
  } // for Lidx DISTANT/HOME
} // for APidx ApproCnt
//
if (Occpied == true){
    if (SnsrActiveLED > 0)
     {digitalWrite(SnsrActiveLED, HIGH);
      SnsrActiveMS = millis();
      } // SnsrActiveLED
    } // Occpied
}// ReadSensors
// *************************************************************************
// ============= StartAlert ======================
// start the alert
void StartAlert (){
// ============= StartAlert ======================
ALidx = Appro[APidx].AlrtIdx;
if (Alrts[ALidx].DontRepeatMS > 0 && Alrts[ALidx].DontRepeatMS > millis())
  {	
    #if Trace > 0
       Serial.print("Defer alert for ");
       Serial.print  ((Alrts[ALidx].DontRepeatMS - millis())/1000.);
       Serial.println  (" seconds");
	   #endif
   return;}
Alrts[ALidx].AlertEndMs   = millis() + Alrts[ALidx].AlrtSecs * 1000.;
Alrts[ALidx].DontRepeatMS = millis() + DontRepeatSecs * 1000.;
if (Alrts[ALidx].SndPort > 0)
  {digitalWrite(Alrts[ALidx].SndPort, SndOn);
	#if Trace > 0
       Serial.print("Start alert ");
       Serial.println  (ALidx);
	   #endif
  }// SndPort
} // StartAlert
// *************************************************************************
// ============= setup ======================
void setup() {
// ============= setup ======================
  int idx, jdx;
  #if Trace > 0
    Serial.begin(9600);
    Serial.println(".");
    Serial.println("******* Startup version:" Vers);
    #endif
  #if SnsrActiveLED > 0
    pinMode(SnsrActiveLED, OUTPUT); // set relay
    #endif
  
  // initialize the Alrts ports  
  for (idx = 0; idx < AlrtCnt; ++idx){    
    Alrts[idx].DontRepeatMS = Alrts[idx].AlertEndMs = 0; 
	if (Alrts[idx].SndPort > 0){
     pinMode(Alrts[idx].SndPort, OUTPUT); // set sound
     digitalWrite(Alrts[idx].SndPort, SndOn);
     delay(2000);
     digitalWrite(Alrts[idx].SndPort, SndOff);
     #if Trace > 1
       Serial.print("Alert pin: ");
       Serial.println(Alrts[idx].SndPort);
       #endif
     } // if Alrts.SndPort
	if (Alrts[idx].RstPort > 0){
     pinMode(Alrts[idx].RstPort, INPUT_PULLUP); // set button
	} // if RstPort
  } // idx AlrtCnt
// initialize the sensors
  for (idx = 0; idx < ApproCnt; ++idx){
  for (jdx = 0; jdx <= HOME;    ++jdx){
	if (Appro[idx].SnsrLst[jdx] > 0) {
      pinMode(Appro[idx].SnsrLst[jdx], INPUT);}
  }// for jdx
  }// for idx
   
  ClearSnsrs();
  
  SnsrActiveMS = 0;
  Cycles = 0;
  CycleMs = 0;
  RecalTM = millis() + RecalMts * 60000.;

  } // setup =============

// ============= loop ======================
void loop() {
// ============= loop ======================

  CycleStrtMs = millis();
  Secs = CycleStrtMs / 1000.;
  Mins = Secs / 60.;
 
  ReadSensors();
  
// BEGIN loop by Approach
   for (APidx = 0; APidx < ApproCnt; ++ APidx) {
	SnsrHome = APidx * 2 + HOME;
	SnsrDist = APidx * 2 + DISTANT;
    ALidx    = Appro[APidx].AlrtIdx;
	#if Trace > 0
      Serial.print("Work  approach ");
      Serial.print(APidx);
      Serial.print(" distant: ");
      Serial.print(Appro[APidx].SnsrLst[DISTANT]);
      Serial.print(" Now:");
      Serial.print(StatText[NowOccStat[SnsrDist]]);
      Serial.print(" Phase:");
      Serial.print(StatText[PhaseStat [SnsrDist]]);
      Serial.print(" home: ");
      Serial.print(Appro[APidx].SnsrLst[HOME]);
      Serial.print(" Now:");
      Serial.print(StatText[NowOccStat[SnsrHome]]);
      Serial.print(" Phase:");
      Serial.print(StatText[PhaseStat [SnsrHome]]);
	  if (PhaseStat[SnsrHome] == StatPend 
          && NowOccTM[SnsrDist] + ShortTripSecs * 1000. > millis()){ // timeout
        Serial.print(" Pend timer:");
        Serial.print(((NowOccTM[SnsrDist] + ShortTripSecs * 1000.) - millis())/1000.);
        Serial.print(" seconds");
		}
	  if (PhaseStat[SnsrDist] == StatExiting
          && NowOccTM[SnsrHome] + ShortTripSecs * 1000. > millis()){ // timeout
        Serial.print(" Exit timer:");
        Serial.print(((NowOccTM[SnsrHome] + ShortTripSecs * 1000.) - millis())/1000.);
        Serial.print(" seconds");
		} // pend timer
      Serial.println("");
      #endif
	#if Trace > 0
      Serial.print("Alert ");
      Serial.print(ALidx);
	  if (Alrts[ALidx].AlertEndMs > 0)
	  { 
        Serial.print(" active for ");
        Serial.print((Alrts[ALidx].AlertEndMs - millis())/1000.);
        Serial.print(" seconds");
	  } else {
        Serial.print(" inactive");
	  }
	  if (Alrts[ALidx].DontRepeatMS > 0 && Alrts[ALidx].DontRepeatMS > millis())
	    { 
          Serial.print(" dont repeat for ");
          Serial.print((Alrts[ALidx].DontRepeatMS - millis())/1000.);
          Serial.print(" seconds ");
	  }
	  Serial.println("");
      #endif
  
// Rule 9 - if HOME has been pending too long, clear it
if (PhaseStat[SnsrHome] == StatPend 
     && NowOccTM[SnsrDist] + ShortTripSecs * 1000. < millis()) // timeout
   {PhaseStat[SnsrHome] = StatClr;}
if (PhaseStat[SnsrDist] == StatExiting 
     && NowOccTM[SnsrHome] + ShortTripSecs * 1000. < millis()) // timeout
   {PhaseStat[SnsrDist] = StatClr;}

if (Chngd[SnsrDist] == true)
{ // distant changed
  if (NowOccStat[SnsrDist] == StatClr)
  { // SnsrDist now clear
   PhaseStat[SnsrDist] = StatClr;
  } else { // SnsrDist now occupied
   switch (PhaseStat[SnsrDist]){
    case StatClr: // was clear - wait for Home to go Occ
      PhaseStat[SnsrDist] = StatOcc;
      PhaseStat[SnsrHome] = StatPend;
	  break;
    case StatExiting: // was clear - wait for Home to go Occ
	  break;
    default:
	#if Trace > 0
      Serial.print("Unexpected distant stat: ");
      Serial.println(StatText[PhaseStat[SnsrDist]]);
	  #endif
	  break;
   }// switch
  } // SnsrDist now occupied
} // distant changed	

if (Chngd[SnsrHome] == true)
  { // home changed
  if (NowOccStat[SnsrHome] == StatClr)
  { // SnsrHome now clear
   PhaseStat[SnsrHome] = StatClr;
  } else { // SnsrHome now occupied
   switch (PhaseStat[SnsrHome]){
    case StatPend: // set to Exiting
	  StartAlert();
	  break;  
    case StatClr: // was clear - reverse entry
      PhaseStat[SnsrDist] = StatExiting;
	  break;
    default:
	#if Trace > 0
      Serial.print("Unexpected distant stat: ");
      Serial.println(StatText[PhaseStat[SnsrDist]]);
	  #endif
	  break;
   }
   PhaseStat[SnsrHome] = StatExiting;
   } // home Occupied	
} // home changed	

	#if Trace > 0
      Serial.print("After approach ");
      Serial.print(APidx);
      Serial.print(" distant: ");
      Serial.print(Appro[APidx].SnsrLst[DISTANT]);
      Serial.print(" Now:");
      Serial.print(StatText[NowOccStat[SnsrDist]]);
      Serial.print(" Phase:");
      Serial.print(StatText[PhaseStat [SnsrDist]]);
      Serial.print(" home: ");
      Serial.print(Appro[APidx].SnsrLst[HOME]);
      Serial.print(" Now:");
      Serial.print(StatText[NowOccStat[SnsrHome]]);
      Serial.print(" Phase:");
      Serial.print(StatText[PhaseStat [SnsrHome]]);
	  if (PhaseStat[SnsrHome] == StatPend 
          && NowOccTM[SnsrDist] + ShortTripSecs * 1000. > millis()){ // timeout
        Serial.print(" Pend timer:");
        Serial.print(((NowOccTM[SnsrDist] + ShortTripSecs * 1000.) - millis())/1000.);
        Serial.print(" seconds");
		}
	  if (PhaseStat[SnsrDist] == StatExiting
          && NowOccTM[SnsrHome] + ShortTripSecs * 1000. > millis()){ // timeout
        Serial.print(" Exit timer:");
        Serial.print(((NowOccTM[SnsrHome] + ShortTripSecs * 1000.) - millis())/1000.);
        Serial.print(" seconds");
		} // pend timer
      Serial.println("");
     #endif
}// APidx < ApproCnt  END loop by Approach

  ClearAlerts();

  ThisCycleMs = (millis() - CycleStrtMs);
  CycleMs += ThisCycleMs;
  Cycles +=1;

// check if time to recalibrate sensors
  if (RecalTM <= millis() && Occpied == false){
    #if Trace > 0
      Serial.print("****** Reset at: ");
      Serial.print(Mins);
      Serial.print(" minutes. Cycles: ");
      Serial.print(Cycles);
      Serial.print(" Avg Ms: ");      
      Serial.print(CycleMs / Cycles);  
      Serial.print(" Cycles per sec: ");      
      Serial.println(Cycles * 1000./CycleMs);  
      #endif
    for (Lidx = 0; Lidx < NumSnsrs; Lidx++)
	  {Largest[Lidx] = 0;} // for Lidx
    RecalTM = millis() + RecalMts * 60000.;
  }// reset

  #if (Trace > 0)
  if (MainLoopDlyMS > 0 ){
    if ((ThisCycleMs) < MainLoopDlyMS){delay(MainLoopDlyMS-ThisCycleMs);}
    }
	#endif
  if (SnsrActiveLED > 0 && (SnsrActiveMS + 500) < millis())
      {digitalWrite(SnsrActiveLED, LOW);}
//
}// main loop ======================
