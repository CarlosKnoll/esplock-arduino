void printMessage( String string2print ){
  Heltec.display->clear();
  Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
  Heltec.display->setFont(ArialMT_Plain_10);
  Heltec.display->drawString(0, 20, "======================");
  Heltec.display->drawString(0, 30, string2print);
  Heltec.display->drawString(0, 40, "======================");
  Heltec.display->display();
}