### ERSTELLUNG DER ZAHLEN:
Für die Darstellung der verschiedenen Zahlen muss zuerst herausgefunden werden welche Bits gesetzt werden müssen. Für die Darstellung benötigt man ein Byte welches in Hexadezimal angegeben werden kann.

Die DIGITS werden als defines gespeichter.

Für die Anzeige der Zeit und Informationen über den Wecker wird eine eigene Funktion "calc_display" erstellt. 
In dieser Methode werden alle Digits in einem Array gespeichert.
Es muss die Einerstellte der Stunde und der Minute sowie die Zehnerstelle der Stunde und der Minute ermittelt werden.
Das Ergebnis wird genommen und die Position im Digits Array ergibt das Bitmuster für die Zahl.

Für die Weckerinformationen sowie das Blinken muss das 7te Bit an verschiedenen Stellen gesetzt werden.
Die Information ob es gesetzt werden soll kommt über die Parameterliste.

In einem uint32_t werden nun die verschiedenen Stellen gespeichert. Man muss hier die Bits so shiften, dass z.b. für die Stunden-Zehnerstelle das 3te Byte und für die Minuten-Einerstelle das 0te Byte verwendet wird.

Im Ergebnis sind nun alle Bits so gesetzt damit die Anzeige richtig erscheint.


### Zeitermittlung
Es wird der Systick / PL_TICKS_PER SECONDS gerechnet und mit dem alten elapsed verglichen. Wenn ungleich dann ist eine Sekunde vergangen. 
In diesem IF-Block werden nun die Sekunden gespeichert und über die Sekunden werden Minuten und Stunden errechnet. 
Hier bin ich mir nicht ganz sicher wie man das besser lössen könnte. Ich hab hier elapsed % 60 == 0 für die Sekunden genommen und mit diesen Sekunden weitergerechnet.

Beim Blinken nimmt man immer jede zweite Sekunde.


### BUTTONS
Bei den Buttons werden wieder Bitmasken eingesetzt. Hier muss man wieder aufpassen dass Stunden nur bis 24 gehen und Minuten bis 60. 
Besonders ist hier, dass man immer den vorherigen Buttonauslöser (also die Bitmaske) speichert weil ansonsten zu schnell inkrementiert wird - Die Schleife läuft ja ziemlich schnell.

Der Alarm ist ähhlnich zu implementieren nur mit etwas anderer Bitmaske

### SWITCHES
Hier muss man mit bitweise UND prüfen ob das 7te Bit gesetzt ist.

### ALARM 
Hier einfach prüfen ob der alarm eingeschaltet ist und ob die Zeit gleich der Alarmzeit ist


### Anzeige
Wenn der Alarmbutton gedrückt wird soll nur die Alarmanzeige erscheinen und wenn sie nicht gedrückt wird die normale Zeitanzeige.
