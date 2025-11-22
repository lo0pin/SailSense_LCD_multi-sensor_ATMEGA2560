#define buttonone 8
#define buttontwo 9
#define buttonthr 10
#define buttonfou 11
#define buzzerpin 12

#define prime 264
#define terze 330
#define quint 396
#define oktav 528


void setup(){
  pinMode(buttonone, INPUT_PULLUP);
  pinMode(buttontwo, INPUT_PULLUP);
  pinMode(buttonthr, INPUT_PULLUP);
  pinMode(buttonfou, INPUT_PULLUP);
  
  
}

void loop(){
  if(!digitalRead(buttonone)){
    tone(buzzerpin, prime);   
    delay(100);    
  } else if(!digitalRead(buttontwo)){
    tone(buzzerpin, terze);   
    delay(100);    
  } else if(!digitalRead(buttonthr)){
    tone(buzzerpin, quint);   
    delay(100);    
  } else if(!digitalRead(buttonfou)){
    tone(buzzerpin, oktav);   
    delay(100);    
  } else{
    noTone(buzzerpin);
  }

  

}
