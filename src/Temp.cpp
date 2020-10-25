// SerialMon.print("02 FUNCTION: check TNB & ELCB");
// SerialMon.println("");
//   if (A1State != lastA1State) {
//     if (A1State == HIGH){
//      if(ReportMode==String("SMS")){
//      }

//      if(ReportMode==String("MQTT")){
//     //   AllAlarms = AllAlarms+ String("\"TNB\":") + String(1) +String(" ,");
//     //   SerialMon.print("TNB check, AllAlarms: ");
//     //   SerialMon.println(AllAlarms);
//       count=0;
//       checkPinStatus();

//      }
//     }
//     else{
//      if(ReportMode==String("SMS")){
//      }
      
//      if(ReportMode==String("MQTT")){
//     //   AllAlarms = AllAlarms+ String("\"TNB\":") + String(0) +String(" ,");
//     //   SerialMon.print("TNB check, AllAlarms: ");
//     //   SerialMon.println(AllAlarms);
//      }
//     }
//   }
//   else if(ReportMode==String("MQTT"))
//   {
//     if (A1State == HIGH){
//     //   AllAlarms = AllAlarms+ String("\"TNB\":") + String(1) +String(" ,");
//     //   SerialMon.print("TNB check, AllAlarms: ");
//     //   SerialMon.println(AllAlarms);
//     count=0;
//     checkPinStatus();
//     }
//     else{
//     //   AllAlarms = AllAlarms+ String("\"TNB\":") + String(0) +String(" ,");
//     //   SerialMon.print("TNB check, AllAlarms: ");
//     //   SerialMon.println(AllAlarms);
//     }
//   }
  
//   lastA1State = A1State;

//   if (A1State == LOW /*&& A2State != lastA2State*/){
//     if (A2State == HIGH){
//      if(ReportMode==String("SMS")){
//     //   if (A2State != lastA2State)
//     //   {
//     //   SerialMon.println("TRYSENDSMS: ELCB=TRIP");
//     //   myString = String(ID) + "\nELCB=TRIP";
//     //   sendSms(myString);
//     //   }
//      }
//      if(ReportMode==String("MQTT")){
//     //   AllAlarms = AllAlarms+ String("\"ELCB\":") + String(1) +String(" ,");
//     //   SerialMon.print("ELCB check, AllAlarms: ");
//     //   SerialMon.println(AllAlarms);
//     count=0;
//     checkPinStatus();
//      }
//     }
//     else{
//      if(ReportMode==String("SMS")){
//     //  if (A2State != lastA2State)
//     //  {
//     //   SerialMon.println("TRYSENDSMS: ELCB=OK");
//     //   myString = String(ID) + "\nELCB=OK";
//     //   sendSms(myString);
//     //  }
//      }
//      if(ReportMode==String("MQTT")){
//     //   AllAlarms = AllAlarms+ String("\"ELCB\":") + String(0) +String(" ,");
//     //   SerialMon.print("ELCB check, AllAlarms: ");
//     //   SerialMon.print(AllAlarms);
//      }
//     }
//   }
  
  
//   else if(ReportMode==String("MQTT"))
//   {
//     if (A2State == HIGH){
//     //   AllAlarms = AllAlarms+ String("\"ELCB\":") + String(1) +String(" ,");
//     //   SerialMon.print("ELCB check, AllAlarms: ");
//     //   SerialMon.println(AllAlarms);
//      count=0;
//      checkPinStatus();
//     }
//     else{
//     //   AllAlarms = AllAlarms+ String("\"ELCB\":") + String(0) +String(" ,");
//     //   SerialMon.print("ELCB check, AllAlarms: ");
//     //   SerialMon.print(AllAlarms);
//   }
//   }