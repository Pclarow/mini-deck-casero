// MINI DECK - Controlador DJ (Entrega 1, Diseño de Interfaces UAI)
// Envía por serial: CUE,PLAY,FX,FILTER,FADER   ej: 0,1,0,512,880
const byte PIN_CUE    = 2;
const byte PIN_PLAY   = 3;
const byte PIN_FX     = 4;
const byte PIN_FILTER = A0;
const byte PIN_FADER  = A1;

int ultimo[5] = {-1, -1, -1, -1, -1};
float filtro = 0, fader = 0;
unsigned long ultimoEnvio = 0;

void setup() {
  Serial.begin(115200);
  pinMode(PIN_CUE,  INPUT_PULLUP);   // sin resistencias externas
  pinMode(PIN_PLAY, INPUT_PULLUP);
  pinMode(PIN_FX,   INPUT_PULLUP);
  filtro = analogRead(PIN_FILTER);
  fader  = analogRead(PIN_FADER);
}

void loop() {
  // suavizado: elimina el ruido de las lecturas analógicas
  filtro = filtro * 0.85 + analogRead(PIN_FILTER) * 0.15;
  fader  = fader  * 0.85 + analogRead(PIN_FADER)  * 0.15;

  int v[5];
  v[0] = !digitalRead(PIN_CUE);      // 1 = presionado
  v[1] = !digitalRead(PIN_PLAY);
  v[2] = !digitalRead(PIN_FX);
  v[3] = (int)(filtro + 0.5);        // 0 a 1023
  v[4] = (int)(fader + 0.5);         // 0 a 1023

  bool cambio = false;
  for (int i = 0; i < 5; i++) {
    int tolerancia = (i < 3) ? 0 : 2;
    if (abs(v[i] - ultimo[i]) > tolerancia) cambio = true;
  }

  // envía cuando algo cambia, y cada 250 ms para mantener sincronizado el dashboard
  if (cambio || millis() - ultimoEnvio > 250) {
    for (int i = 0; i < 5; i++) {
      ultimo[i] = v[i];
      Serial.print(v[i]);
      Serial.print(i < 4 ? ',' : '\n');
    }
    ultimoEnvio = millis();
  }
  delay(5);
}