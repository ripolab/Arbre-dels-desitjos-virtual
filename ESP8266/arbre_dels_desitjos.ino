/*
  #RETO NAVIDEÑO. Arbre dels desitjos

  Ripolab Hacklab Diciembre 2017

  by @akirasan
  ripolab.org

*/

#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>

#define PIN               D3                      // Pin conectada tira leds WS2812b
#define TIEMPO_MAX_DESEO  100                     // Tiempo maximo activo de un deseo --- expresado en ciclos
#define NUM_LEDS          300                     // Numero de leds total
#define NUM_DESEOS        30                      // Numero de deseos maximos
const byte LEDS_SLOTS =   NUM_LEDS / NUM_DESEOS;  // Reparto de leds por el numero de deseos
#define NUM_DESEOS_MIN    10                      // Minimo numero de deseos siempre activos (se generan falsos deseos)

// Configura la desaparicion de un deseo
#define UMBRAL1           50                      // Tiempo (expresado en ciclos) para cuando atenuar el deseo


//========================================== CONFIGURACION CONEXION WIFI
//Globales configuracion conexion Wifi
const char* ssid     = "AQUÍ_TU_SSID_WIFI";
const char* password = "AQUÍ_PASSWORD_WIFI";


Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);

byte deseos_colores[NUM_DESEOS][3]; //Gestion del color de un deseo 0:r, 1:g, 2:b
int deseos_tiempo[NUM_DESEOS];      // Gestion de tiempo vida de un deseo activo --- expresado en ciclos
String id_ultimo_deseo = "";        // ID-web del ultimo deseo registrado en web
byte contador_deseos = 0;           // Numero de deseos recibidos y activos
boolean inicializar_deseos = true;  // TRUE el primer inicio
boolean nuevo_deseo = false;        // TRUE cuando ha llegado un deseo nuevo


// ----- Gestion de efectos por tiempo
#define MINUTOS_EFECTO 15           // Cada cuanto tiempo (expresado en minutos) saltará un evento de efectos
unsigned long tiempo_encendido;     // Gestion del tiempo activo del proceso (milisegundos)
unsigned int minuto_efecto = 0;     // Gestion del tiempo activo del proceso (segundos)



// ===================================
// ================================= SETUP/LOOP
// ===================================

void setup() {
  Serial.begin(115000);
  pixels.begin(); // Inicializar NeoPixel
  pixels.setBrightness(200); // Brillo para todo
  conexion_wifi();
  iniciar_deseos();

}

void loop() {

  // Lanzamos efectos de test
  // Primer inicio generamos un mínimo de deseos falsos
  if (inicializar_deseos) {
    for (int x = 0; x < 500; x++) {
      Sparkle(0xff, 0xff, 0xff, 0);
      Sparkle(0xff, 0, 0, 0);
      Sparkle(0, 0xff, 0, 0);
      Sparkle(0, 0, 0xff, 0);
    }
    for (int i = 0; i < NUM_DESEOS_MIN; i++) {
      generar_deseo_falso();
    }
    inicializar_deseos = false;
  }

  nuevo_deseo = false;
  mostrar_deseos_actuales();
  Serial.print("NUMERO DESEOS: "); Serial.println(contador_deseos);

  reducir_tiempo_deseos();
  efecto_runner();
  //delay(1000);

  // Verificamos si hay espacio para cargar mas deseos y si hay espacios libres revisamos la web
  if (contador_deseos < NUM_DESEOS) {
    int ant_contador_deseos = contador_deseos;
    leer_deseo_web();
    if (ant_contador_deseos != contador_deseos) {
      nuevo_deseo = true;  // para no generar un deseo falso nada mas añadir uno verdadero
    }
  }
  if ((!nuevo_deseo) && (contador_deseos < NUM_DESEOS_MIN)) {
    generar_deseo_falso();
  }

  // Gestionar los efectosFX cada MINUTOS_EFECTOS
  tiempo_encendido = millis();
  tiempo_encendido = trunc(tiempo_encendido / 60000);        // pasamos millisegundos a minutos
  Serial.println(tiempo_encendido);
  if (((tiempo_encendido % MINUTOS_EFECTO) == 0) && (minuto_efecto != tiempo_encendido)) {
    Serial.println("EFECTO");
    minuto_efecto = tiempo_encendido;
    lanzar_efecto();
  }

}

// ===================================
// ===================================
// ===================================


// Todo a ZERO!!!
void iniciar_deseos() {
  for (int deseo = 0; deseo < NUM_DESEOS; deseo++) {
    deseos_colores[deseo][0] = 0;    // R
    deseos_colores[deseo][1] = 0;    // G
    deseos_colores[deseo][2] = 0;    // B
    deseos_tiempo[deseo] = 0;        // TIEMPO
  }
}


void conexion_wifi() {
  Serial.print("Conectando a ");

  Serial.println(ssid);
  if (WiFi.status() == WL_CONNECTED) {
    WiFi.disconnect();
  }

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi Conectado");
  Serial.println(WiFi.localIP());
}


void colorear_todos(byte r, byte g, byte b) {
  for (int i = 0; i < NUM_LEDS; i++) {
    pixels.setPixelColor(i, pixels.Color(r, g, b));
  }
}


void efecto_nuevo_deseo_todos(byte r, byte g, byte b) {
  for (int i = 0; i < 10; i++) {
    colorear_todos(r, g, b);
    pixels.show(); // This sends the updated pixel color to the hardware.
    delay(500);
    colorear_todos(0, 0, 0);
    pixels.show(); // This sends the updated pixel color to the hardware.
    delay(500);
  }
}

void colorear_slot_forzado(int deseo, byte r, byte g, byte b) {
  int led_inicial = LEDS_SLOTS * deseo;
  int led_final = led_inicial + LEDS_SLOTS - 1;
  for (int led = led_inicial; led <= led_final; led++) {
    pixels.setPixelColor(led, pixels.Color(r, g, b));
  }
}

void efecto_nuevo_deseo_slot(int deseo) {
  for (int i = 0; i < 5; i++) {
    colorear_slot_forzado(deseo, 0, 0, 0);
    pixels.show();
    delay(1000);
    colorear_slot_forzado(deseo, deseos_colores[deseo][0], deseos_colores[deseo][1], deseos_colores[deseo][2]);
    pixels.show();
    delay(1000);
  }
}


void add_deseo(int deseo, byte r, byte g, byte b) {
  efecto_nuevo_deseo_todos(r, g, b);
  mostrar_deseos_actuales();

  // añadimos el nuevo deseo
  deseos_colores[deseo][0] = r; deseos_colores[deseo][1] = g; deseos_colores[deseo][2] = b;
  deseos_tiempo[deseo] = TIEMPO_MAX_DESEO;

  // efecto sobre el deseo
  efecto_nuevo_deseo_slot(deseo);
}

void update_deseo_sin_efecto(int deseo, byte r, byte g, byte b, int TIEMPO_MAX) {
  // añadimos el nuevo deseo
  deseos_colores[deseo][0] = r; deseos_colores[deseo][1] = g; deseos_colores[deseo][2] = b;
  deseos_tiempo[deseo] = TIEMPO_MAX;
}


void colorear_slot(int deseo) {
  int led_inicial = LEDS_SLOTS * deseo;
  int led_final = led_inicial + LEDS_SLOTS - 1;

  for (int led = led_inicial; led <= led_final; led++) {
    pixels.setPixelColor(led, pixels.Color(deseos_colores[deseo][0], deseos_colores[deseo][1], deseos_colores[deseo][2]));
  }
}



void mostrar_deseos_actuales() {
  // Muestra y actualiza el contador de deseos activos
  contador_deseos = 0;

  for (int i = 0; i < NUM_DESEOS; i++) {
    if (deseos_tiempo[i] > 0) {
      colorear_slot(i);
      contador_deseos++;
    }
  }
  pixels.show();
}

int buscar_slot_vacio() {
  // ramdom de slot vacio teniendo en cuenta los ya ocupados
  while (contador_deseos < NUM_DESEOS) {
    int slot = random(NUM_DESEOS);
    delay(5);
    if (deseos_tiempo[slot] <= 0) {
      return (slot);
    }
  }
}


void leer_deseo_web() {
  // llamar a ULR para recuperar deseos
  String sURL;
  String result;

  WiFiClient client;
  char *servername = "ripolab.org";
  const int httpPort = 80;

  sURL = "GET http://nadal.ripolab.org/api/deseos/read_last.php";
  result = "";

  if (!client.connect(servername, httpPort)) {  //starts client connection, checks for connection
    Serial.println("conexion a Web fallida"); //error message if no client connect
    return;
  }

  Serial.println("Conectado!!!");
  client.println(sURL);
  client.println("Host: nadal.ripolab.org");
  client.println("User-Agent: ArduinoWiFi/1.1");
  client.println("Conexion: cerrada");
  client.println("\n");

  while (client.connected() && !client.available()) delay(1); //waits for data
  //Serial.println("Esperando datos...");

  while (client.connected() || client.available()) { //connected or data available
    char c = client.read(); //gets byte from ethernet buffer
    result = result + c;
  }

  client.stop(); //stop client
  result.replace('[', ' '); result.replace(']', ' ');
  Serial.println(result);

  String id_deseo;
  String hex_color_deseo;
  char jsonArray [result.length() + 1];
  result.toCharArray(jsonArray, sizeof(jsonArray));

  jsonArray[result.length() + 1] = '\0';
  StaticJsonBuffer<128> json_buf;
  JsonObject &root = json_buf.parseObject(jsonArray);
  if (!root.success())
  {
    Serial.println("parseObject() fallido - No asignamos color");

  }

  String id = root["id"];
  String color = root["color"];
  String ttl = root["ttl"];
  String timeS = root["date_add"];

  id_deseo = id;
  hex_color_deseo = color;

  Serial.println(id_deseo);
  Serial.println(hex_color_deseo);

  if (hex_color_deseo[0] == '#' && id_ultimo_deseo != id_deseo) {
    byte r, g, b;

    r = hexToDec(hex_color_deseo.substring(1, 3));
    g = hexToDec(hex_color_deseo.substring(3, 5));
    b = hexToDec(hex_color_deseo.substring(5, 8));

    id_ultimo_deseo = id_deseo;

    add_deseo(buscar_slot_vacio(), r, g, b);
  }
}


byte hexToDec(String hexString) {
  unsigned long decValue = 0;
  char nextInt;
  for ( long i = 0; i < hexString.length(); i++ ) {
    nextInt = toupper(hexString[i]);
    if ( isxdigit(nextInt) ) {
      if (nextInt >= '0' && nextInt <= '9') nextInt = nextInt - '0';
      if (nextInt >= 'A' && nextInt <= 'F') nextInt = nextInt - 'A' + 10;
      decValue = (decValue << 4) + nextInt;
    }
  }
  return decValue;
}


void morir_deseo(int deseo) {
  for (int i = 0; i < 10; i++) {
    colorear_slot_forzado(deseo, 0, 0, 0);
    pixels.show();
    delay(10);
    colorear_slot_forzado(deseo, deseos_colores[deseo][0], deseos_colores[deseo][1], deseos_colores[deseo][2]);
    pixels.show();
    delay(10);
  }
  colorear_slot_forzado(deseo, 0, 0, 0);
  pixels.show();
  update_deseo_sin_efecto(deseo, 0, 0, 0, -1);   // liberar slot/deseo
}

void reducir_tiempo_deseos() {
  // reduce el tabla de tiempos de deseos
  // podría reducir la intensidad o hacer efecto de apagado llegado un mínimo
  byte r, g, b;

  for (int deseo = 0; deseo < NUM_DESEOS; deseo++) {
    if (deseos_tiempo[deseo] == 0) {     // Muerte del deseo
      morir_deseo(deseo);
    }
    else {
      if (deseos_tiempo[deseo] > 0) {
        int vida = deseos_tiempo[deseo] - 1;
        //intensidad = 250;
        if (vida == UMBRAL1) {
          byte intensidad = 127;
          r = intensidad * deseos_colores[deseo][0] / 255;
          g = intensidad * deseos_colores[deseo][1] / 255;
          b = intensidad * deseos_colores[deseo][2] / 255;
        }
        else
        {
          r = deseos_colores[deseo][0];
          g = deseos_colores[deseo][1];
          b = deseos_colores[deseo][2];
        }
        update_deseo_sin_efecto(deseo, r , g, b, vida);
      }
    }
  }
}




void efecto_runner() {
  uint32_t pixel1, pixel2, pixel3;
  for (int vueltas = 0; vueltas < 1; vueltas++) {
    for (int i = 2; i < NUM_LEDS; i++) {
      // guardamos colores actuales
      pixel1 = pixels.getPixelColor(i);
      pixel2 = pixels.getPixelColor(i - 1);
      pixel3 = pixels.getPixelColor(i - 2);

      // sobreescribimos color runner blanco con degrado
      pixels.setPixelColor(i, 255, 255, 255);
      pixels.setPixelColor(i - 1, 127, 127, 127);
      pixels.setPixelColor(i - 2, 63, 63, 63);

      pixels.show();
      delay(1);

      //recuperamos colores anteriores
      pixels.setPixelColor(i, pixel1);
      pixels.setPixelColor(i - 1, pixel2);
      pixels.setPixelColor(i - 2, pixel3);
      pixels.show();
      delay(1);
    }
  }
}



void generar_deseo_falso() {
  int slot_asignado = buscar_slot_vacio();
  byte intensidad = 127;

  byte r = intensidad * random(255) / 255; delay(5);
  byte g = intensidad * random(255) / 255; delay(5);
  byte b = intensidad * random(255) / 255; delay(5);

  byte tiempo_max = random(TIEMPO_MAX_DESEO);
  update_deseo_sin_efecto(slot_asignado, r, g, b, tiempo_max);
}


void lanzar_efecto() {
  // Ejecutaremos en cada ocasion un efecto/s al azar
  byte efecto = random(1, 3);
  switch (efecto) {
    case 1:         // estrellas + fuego + estrellas
      for (int x = 0; x < 1000; x++) {
        Sparkle(0xff, 0xff, 0xff, 0);
      }
      for (int x = 0; x < 800; x++) {
        Fire(30, 200, 5);
      }
      for (int x = 0; x < 1000; x++) {
        Sparkle(0xff, 0xff, 0xff, 0);
      }
      pixels.clear();
      break;
    case 2:  // Random colors
      TwinkleRandom(200, 100, false);
      for (int x = 0; x < 500; x++) {
        Sparkle(0xff, 0xff, 0xff, 0);
      }
      pixels.clear();
      break;
    case 3:  // Arcoiris
      rainbowCycle(20);
      for (int x = 0; x < 1000; x++) {
        Sparkle(0xff, 0xff, 0xff, 0);
      }
      pixels.clear();
      break;
    default:
      Serial.println("SIN EFECTOS FX");
      break;
  }
}

// =================
// ================= FRAMEWORK EFECTOS
//
// Ejemplos extraidos de https://www.tweaking4all.com/hardware/arduino/adruino-led-strip-effects/#LEDEffects
// =================

void showStrip() {
  pixels.show();
}

void setPixel(int Pixel, byte red, byte green, byte blue) {
  pixels.setPixelColor(Pixel, pixels.Color(red, green, blue));
}

void setAll(byte red, byte green, byte blue) {
  for (int i = 0; i < NUM_LEDS; i++ ) {
    setPixel(i, red, green, blue);
  }
  showStrip();
}


void Sparkle(byte red, byte green, byte blue, int SpeedDelay) {
  int Pixel = random(NUM_LEDS);
  setPixel(Pixel, red, green, blue);
  showStrip();
  delay(SpeedDelay);
  setPixel(Pixel, 0, 0, 0);
}




void rainbowCycle(int SpeedDelay) {
  byte *c;
  uint16_t i, j;

  for (j = 0; j < 256 * 5; j++) { // 5 cycles of all colors on wheel
    for (i = 0; i < NUM_LEDS; i++) {
      c = Wheel(((i * 256 / NUM_LEDS) + j) & 255);
      setPixel(i, *c, *(c + 1), *(c + 2));
    }
    showStrip();
    delay(SpeedDelay);
  }
}

byte * Wheel(byte WheelPos) {
  static byte c[3];

  if (WheelPos < 85) {
    c[0] = WheelPos * 3;
    c[1] = 255 - WheelPos * 3;
    c[2] = 0;
  } else if (WheelPos < 170) {
    WheelPos -= 85;
    c[0] = 255 - WheelPos * 3;
    c[1] = 0;
    c[2] = WheelPos * 3;
  } else {
    WheelPos -= 170;
    c[0] = 0;
    c[1] = WheelPos * 3;
    c[2] = 255 - WheelPos * 3;
  }

  return c;
}


void Fire(int Cooling, int Sparking, int SpeedDelay) {
  static byte heat[NUM_LEDS];
  int cooldown;

  // Step 1.  Cool down every cell a little
  for ( int i = 0; i < NUM_LEDS; i++) {
    cooldown = random(0, ((Cooling * 10) / NUM_LEDS) + 2);

    if (cooldown > heat[i]) {
      heat[i] = 0;
    } else {
      heat[i] = heat[i] - cooldown;
    }
  }

  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for ( int k = NUM_LEDS - 1; k >= 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
  }

  // Step 3.  Randomly ignite new 'sparks' near the bottom
  if ( random(255) < Sparking ) {
    int y = random(7);
    heat[y] = heat[y] + random(160, 255);
    //heat[y] = random(160,255);
  }

  // Step 4.  Convert heat to LED colors
  for ( int j = 0; j < NUM_LEDS; j++) {
    setPixelHeatColor(j, heat[j] );
  }

  showStrip();
  delay(SpeedDelay);
}

void setPixelHeatColor (int Pixel, byte temperature) {
  // Scale 'heat' down from 0-255 to 0-191
  byte t192 = round((temperature / 255.0) * 191);

  // calculate ramp up from
  byte heatramp = t192 & 0x3F; // 0..63
  heatramp <<= 2; // scale up to 0..252

  // figure out which third of the spectrum we're in:
  if ( t192 > 0x80) {                    // hottest
    setPixel(Pixel, 255, 255, heatramp);
  } else if ( t192 > 0x40 ) {            // middle
    setPixel(Pixel, 255, heatramp, 0);
  } else {                               // coolest
    setPixel(Pixel, heatramp, 0, 0);
  }
}

void TwinkleRandom(int Count, int SpeedDelay, boolean OnlyOne) {
  setAll(0, 0, 0);

  for (int i = 0; i < Count; i++) {
    setPixel(random(NUM_LEDS), random(0, 255), random(0, 255), random(0, 255));
    showStrip();
    delay(SpeedDelay);
    if (OnlyOne) {
      setAll(0, 0, 0);
    }
  }

  delay(SpeedDelay);
}

// =================
// =================
// =================

